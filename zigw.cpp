#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main(int argc, char** argv)
{
    std::vector<std::string> args;
    for(auto i = 1; i < argc; i++)
    {
        args.push_back(argv[i]);
    }

    bool isCheckingLinkerVersion = false;
    std::string fullArg = "";
    for(auto& s : args)
    {
        if(s.find("-Wl,--version") != std::string::npos)
        {
            isCheckingLinkerVersion = true;
            auto result = exec("zig cc --version");
            std::string verIdent = "clang version";
            if(auto pos = result.find(verIdent) != std::string::npos)
            {
                std::string version = result.substr(pos + verIdent.size(),  6);
                std::cout <<  "LLD " << version <<  " (compatible with GNU linkers)\n";
            }
        }else{
            fullArg.append(s);
            fullArg.append(" ");
        }
    }
    if(!isCheckingLinkerVersion)
    {
        std::cout << fullArg;
        system(fullArg.c_str());
    }
    
}