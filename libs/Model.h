//
// Created by cvija on 3.4.23..
//
#ifndef EGYPTIANLANDMARKS_MODEL_H
#define EGYPTIANLANDMARKS_MODEL_H

#include <iostream>
#include <vector>
#include "Mesh.h"

class Model {
public:
    std::vector<Mesh> meshes;

    Model(std::string path) {

    }

    void Draw() {
        for (Mesh& mesh : meshes) {
            mesh.Draw(shader);
        }
    }
};

#endif //EGYPTIANLANDMARKS_MODEL_H
