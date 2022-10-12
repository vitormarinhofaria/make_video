#include "pch.hpp"
#include "shader.hpp"

struct Transform
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;

    glm::mat4 Matrix()
    {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, position);
        return glm::scale(mat, scale);
    }
};

const int SW = 1280;
const int SH = 720;
const int FBW = 1920;
const int FBH = 1080;

GLuint create_fb(GLuint *textureColorBuffer, GLuint *rbo)
{
    GLuint fbo;
    glGenFramebuffers(1, &fbo);

    glGenTextures(1, textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, *textureColorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FBW, FBH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, FBW, FBH);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *textureColorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer Created\n");
    }
    else
    {
        printf("Failed to create framebuffer\n");
    }
    return fbo;
}

GLuint create_screen_quad(int id = 0)
{
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    float screenQuad[20];
    if (id == 1)
    {
        float quad[] = {
            // positions         // texture coords
            1.0f, 1.0f, 1.0f, 1.0f, 1.0f,   // top right
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f,  // bottom right
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, // bottom left
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f   // top left
        };
        memcpy(screenQuad, quad, sizeof(quad));
    }
    else
    {
        float quad[] = {
            // positions         // texture coords
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left
        };
        memcpy(screenQuad, quad, sizeof(quad));
    }
    GLuint vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuad), screenQuad, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return vao;
}
void YUVfromRGB(double& Y, double& U, double& V, const double R, const double G, const double B)
{
    Y =  0.257 * R + 0.504 * G + 0.098 * B +  16;
    U = -0.148 * R - 0.291 * G + 0.439 * B + 128;
    V =  0.439 * R - 0.368 * G - 0.071 * B + 128;
}
unsigned char *ImageBuffer = nullptr;
//unsigned char *ImageBuffer = nullptr;
void savePrint(GLuint image, int frame)
{
    stbi_flip_vertically_on_write(true);
    // glBindTexture(GL_TEXTURE_2D, image);

    size_t imgSize = sizeof(unsigned char) * (FBW * FBH) * 3;
    if (ImageBuffer == nullptr)
    {
        ImageBuffer = (unsigned char *)malloc(imgSize);
    }
    // glGetTexImage(image, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageBuffer);
    // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageBuffer);
    // create buffer
    GLuint pbo;
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_PACK_BUFFER, imgSize, 0, GL_STATIC_READ);

    // get texture image
    glBindTexture(GL_TEXTURE_2D, image);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, (void *)(0));

    // map pixel buffer
    uint8_t *data_ptr = (uint8_t*) glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

    // access the data
    // [...]

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

    std::string fileName = "frames/img_";
    fileName.append(std::to_string(frame));
    fileName.append(".png");
    {
                for( auto y = 0; y < 1920; y++){
                    for(auto x = 0; x < 1080; x++){
                        double Y, Cb, Cr = 0.0;
                        uint8_t R = data_ptr[x * y];
                        uint8_t G = data_ptr[x * y + 8];
                        uint8_t B = data_ptr[x * y + 16];
                        YUVfromRGB(Y, Cb, Cr, R, G, B);
                        ImageBuffer[x * y] = (uint8_t)Y;
                        ImageBuffer[x * y + 8] = (uint8_t)Cb;
                        ImageBuffer[x * y + 16] = (uint8_t)Cr;
                    }
                }
            }
    //stbi_write_png(fileName.c_str(), FBW, FBH, 3, data_ptr, FBW * 3);    
}

int main(int argc, char **argv)
{
    auto res = _chdir("resources");
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    GLFWwindow *window = glfwCreateWindow(SW, SH, "Janela", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
    GLuint fbColor, rbo;
    auto fb = create_fb(&fbColor, &rbo);
    auto fbQuad = create_screen_quad(1);
    Shader fbShader("screen");

    Transform quadTransform{.position = {0.0f, 0.0f, 0.0f}, .scale = {1.0f, 1.0f, 1.0f}};
    auto texQuad = create_screen_quad();
    Texture quadTex("image.png");
    Shader quadShader("quad");

    Transform otherTrans{.position = {0.0f, 0.0f, 0.0f}, .scale = {1.0f, 1.0f, 1.0f}};
    auto quadOther = create_screen_quad();
    Texture imgOther("reaper.png");
    Shader shadOther("quad");

    float scrRatio = (1280.0f / 720.0f);
    // glm::mat4 projection = glm::ortho(-30.0f, +30.0f, -24.0f, +24.0f, 0.1f, 100.0f);
    glm::mat4 projection = glm::ortho(scrRatio * -1.0f, scrRatio, scrRatio * -1.0f, scrRatio, 0.1f, 100.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    std::fstream fileStream( "out.avi", std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary );
    

    int frameCount = 0;
    while (!glfwWindowShouldClose(window) && frameCount < 240)
    {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            printf("Respects\n");
        }

        glm::mat4 modelMat = glm::mat4(1.0f);
        glViewport(0, 0, FBW, FBH);
        glBindFramebuffer(GL_FRAMEBUFFER, fb);
        glEnable(GL_DEPTH_TEST);
        glClearColor(quadTransform.position.x, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            glUseProgram(quadShader.id);

            quadTransform.position.x += 0.01f;
            if (quadTransform.position.x > 2.0f)
                quadTransform.position.x = 0;
            quadShader.setUniform("model", quadTransform.Matrix());
            quadShader.setUniform("view", view);
            quadShader.setUniform("projection", projection);

            glBindVertexArray(texQuad);
            glActiveTexture(GL_TEXTURE0);
            quadTex.use();

            // quadShader.use();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        {

            glUseProgram(shadOther.id);

            otherTrans.position.x -= 0.015f;
            if (otherTrans.position.x < -2.0f)
                otherTrans.position.x = 0;
            shadOther.setUniform("model", otherTrans.Matrix());
            shadOther.setUniform("view", view);
            shadOther.setUniform("projection", projection);

            glActiveTexture(GL_TEXTURE0);
            imgOther.use();
            glBindVertexArray(quadOther);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        if (true)
        {
            glViewport(0, 0, SW, SH);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(fbShader.id);

            glBindVertexArray(fbQuad);
            glBindTexture(GL_TEXTURE_2D, fbColor);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        if (true)
        {
            savePrint(fbColor, frameCount);
            frameCount++;
        }
        glfwSwapBuffers(window);
    }
    
    free(ImageBuffer);
}