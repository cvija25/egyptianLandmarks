//
// Created by cvija on 3.4.23..
//

#ifndef EGYPTIANLANDMARKS_MESH_H
#define EGYPTIANLANDMARKS_MESH_H

#include <iostream>
#include <vector>

#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 TexCords;

    glm::vec3 tangents;
    glm::vec3 bitangents;
};

struct Texture {
    unsigned int id;
    std::string type;
};

class Mesh {
public:
    unsigned int VAO;

    std::vector<Vertex> verticies;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> verticies, std::vector<unsigned int> indices, std::vector<Texture> textures) {
        this->verticies = verticies;
        this->indices = indices;
        this->verticies = verticies;

        setupMesh();
    }

    void Draw(Shader& shader) {
        // broj svake mape
        unsigned int diffuseNr = 0;
        unsigned int specularNr = 0;
        unsigned int normalNr = 0;
        unsigned int heightNr = 0;

        for (unsigned int i = 1; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i); // mozemo ovako zato sto su ovi makroi svaki za 1 veci u brojevima
            // pravimo konvenciju da teksture budu svaka: npr. texture_diffuse1

            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse") {
                diffuseNr++;
                number = std::to_string(diffuseNr);
            }
            else if (name == "texture_specular") {
                specularNr++;
                number = std::to_string(specularNr);
            }
            else if (name == "texture_normal") {
                normalNr++;
                number = std::to_string(normalNr);
            }
            else if (name == "texture_height") {
                heightNr++;
                number = std::to_string(heightNr);
            }

            shader.setUniform1i(name+number, i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // sad crtamo konacno taj mesh

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

private:
    void setupMesh() {
        unsigned int VBO, EBO;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);

        glBufferData(GL_ARRAY_BUFFER,verticies.size()*sizeof(Vertex),&verticies[0],GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glBindVertexArray(0);
    }
};

#endif //EGYPTIANLANDMARKS_MESH_H
