//
// Created by cvija on 2.4.23..
//
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#ifndef EGYPTIANLANDMARKS_SHADER_H
#define EGYPTIANLANDMARKS_SHADER_H

std::string readFileContents (std::string path) {
    std::ifstream in(path);
    std::stringstream buffer;

    buffer << in.rdbuf();
    return buffer.str();
}

class Shader {
    unsigned int shaderId;
public:
    // konstruktor
    Shader(std::string vertexShaderPath, std::string fragmentShaderPath) {
        // dovijamo se za vertexShaderSource
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        std::string vsString = readFileContents(vertexShaderPath);
        if (vsString.empty()) {
            std::cerr << "prazan nam je vertexShader \n";
        }
        const char* vertexShaderSource = vsString.c_str();
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        // proveravamo uspesnost kompilacije
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "greska pri compileShader - vertex" << infoLog << std::endl;
        }

        // za fragmentShaderSource
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fsString = readFileContents(fragmentShaderPath);
        if (fsString.empty()) {
            std::cerr << "prazan nam je fragmentShader \n";
        }
        const char* fragmentShaderSource = fsString.c_str();
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "greska pri compileShader - fragment" << infoLog << std::endl;
        }

        // kreiramo shader program

        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // provera kompilacije
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "linkProgram greska \n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        shaderId = shaderProgram;
    }

    void use() {
        glUseProgram(shaderId);
    }

    void setUniform4f(std::string name, float x, float y, float z, float w) {
        int uniformId = glGetUniformLocation(shaderId, name.c_str());
        glUniform4f(uniformId, x, y, z, w);
    }

    void setUniform1i(std::string name, int value) {
        int uniformId = glGetUniformLocation(shaderId, name.c_str());
        glUniform1i(uniformId,value);
    }
};

#endif //EGYPTIANLANDMARKS_SHADER_H
