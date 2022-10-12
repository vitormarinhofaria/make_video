#pragma once
#include "pch.h"

std::string readFileToString(const char *fileName)
{
    FILE *file;
    fopen_s(&file, fileName, "r");
    if (!file)
        return std::string("Error");

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::string fileContent;
    fileContent.resize(fileSize);

    fread(&fileContent[0], sizeof(char), fileSize, file);
    fclose(file);
    return fileContent;
}

struct Shader
{
    GLuint id;

    Shader(const char *shaderName)
    {
        std::string vsPath = shaderName;
        vsPath.append(".vs.glsl");
        std::string vsSource = readFileToString(vsPath.c_str());

        std::string fsPath = shaderName;
        fsPath.append(".fs.glsl");
        std::string fsSource = readFileToString(fsPath.c_str());

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        GLuint program = glCreateProgram();

        auto vsSrcPtr = vsSource.c_str();
        glShaderSource(vs, 1, &vsSrcPtr, nullptr);
        glCompileShader(vs);

        auto fsSrcPtr = fsSource.c_str();
        glShaderSource(fs, 1, &fsSrcPtr, nullptr);
        glCompileShader(fs);

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);
        
        id = program;
    }
    ~Shader(){
        glDeleteProgram(id);
        id = 0;
    }

    void setUniform(const char* name, glm::mat4 value){
        int location = glGetUniformLocation(id, name);
        if(location < 0) return;
        
        glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
    }

    void setUniform(const char* name, float value){
        int location = glGetUniformLocation(id, name);
        if(location < 0) return;

        glUniform1f(location, value);
    }

    void use(){
        glUseProgram(id);
    }
};

struct Texture
{
    GLuint id = 0;
    int width = 0;
    int height = 0;
    GLint format = 0;

    Texture(const char *path)
    {
        stbi_set_flip_vertically_on_load(true); 
        int channels = 0;
        unsigned char *img = stbi_load(path, &width, &height, &channels, 4);

        if (channels == 4)
        {
            format = GL_RGBA;
        }
        else if (channels == 3)
        {
            format = GL_RGB;
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(img);
    }

    ~Texture()
    {
        glDeleteTextures(1, &id);
    }

    void use(){
        glBindTexture(GL_TEXTURE_2D, id);
    }
};
