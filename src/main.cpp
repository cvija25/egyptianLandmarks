#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <headers/shader.h>
#include <headers/camera.h>
#include <headers/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);
void renderScene(Shader& shader, Model& pyramids, Model& temple1, Model& temple2, Model& temple3, Model& obelisk, Model& palma);

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(15.0f, 10.0f, 35.0f));
float lastX = (float)SCR_WIDTH  / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
bool shadows = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool mouseEnable = false;
bool mouseEnablePressed = false;

unsigned int planeVAO;

glm::vec3 lightPos(0.0f,18.5f,10.0f);

glm::vec3 modelPosition [] = {
        glm::vec3(-20.0f, -0.5f, 35.0f),glm::vec3(23.0f, -0.5f, 20.0f), glm::vec3(5.0f, -0.5f, 0.0f),
        glm::vec3(13.0f, -0.5f, 27.0f), glm::vec3(10.0f, -0.4f, 15.0f),glm::vec3(-3.0f, -1.0f, 13.0f)
};

int main()
{
    // incijalizacija glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw prozor
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Egipatske znamenitosti", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // shaders
    Shader depthShader("resources/shaders/shadows_depth.vs","resources/shaders/shadows_depth.fs","resources/shaders/shadows_depth.gs");
    Shader forModel("resources/shaders/model_loading.vs", "resources/shaders/model_loading.fs");
    Shader skyBoxShader("resources/shaders/skybox.vs","resources/shaders/skybox.fs");
    Shader lightShader("resources/shaders/light.vs","resources/shaders/light.fs");

    Model pyramids("resources/objects/pyramids/pyramids.obj");
    Model temple1("resources/objects/temple1/temple1.obj");
    Model temple2("resources/objects/temple2/temple2.obj");
    Model temple3("resources/objects/temple3/temple3.obj");
    Model obelisk("resources/objects/ObeliskPiramide/Obelisk+mini pyramids.obj");
    Model palma("resources/objects/Hoewa_Forsteriana_OBJ/hoewa_Forsteriana_1.obj");
    // scene floor
    float planeVertices[] = {
            // positions            // normals                                  // texcoords
            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
            10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };

    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int floorTexture = loadTexture("resources/textures/pod1.jpg");

    // skybox
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces {
        "resources/textures/skybox1/right1.png",
        "resources/textures/skybox1/left1.png",
        "resources/textures/skybox1/top1.png",
        "resources/textures/skybox1/bottom1.png",
        "resources/textures/skybox1/front1.png",
        "resources/textures/skybox1/back1.png"
    };

    stbi_set_flip_vertically_on_load(false);
    unsigned int cubemapTexture = loadCubemap(faces);

    float lightVertices[] = {
            -1.0f,0.0f,0.0f,        // 0
            0.0f,-1.0f,0.0f,        // 1
            1.0f,0.0f,0.0f,         // 2

            1.0f,0.0f,0.0f,      // 2
            0.0f,1.0f,0.0f,     // 3
            -1.0f,0.0f,0.0f,    // 0

            -1.0f,0.0f,0.0f,    // 0
            0.0f,-1.0f,0.0f,    // 1
            0.0f,0.0f,1.0f,     // 4

            0.0f,-1.0f,0.0f,    // 1
            1.0f,0.0f,0.0f,     // 2
            0.0f,0.0f,1.0f,     // 4

            1.0f,0.0f,0.0f,     // 2
            0.0f,1.0f,0.0f,     // 3
            0.0f,0.0f,1.0f,     // 4

            0.0f,1.0f,0.0f,     // 3
            -1.0f,0.0f,0.0f,    // 0
            0.0f,0.0f,1.0f,     // 4


    };

    unsigned int lightVBO, lightVAO;
    glGenBuffers(1, &lightVBO);
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // shadow mapping
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // postavaljanje uniforma
    forModel.use();
    forModel.setInt("material.diffuse_texture1", 0);
    forModel.setInt("depthMap", 1);

    skyBoxShader.use();
    skyBoxShader.setInt("skybox",0);

    // petlja renderovanja
    while(!glfwWindowShouldClose(window))
    {
        if (!mouseEnable) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.6f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // matrica projekcije i pogleda iz perspektive svetla
        float near_plane = 1.0f;
        float far_plane  = 100.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
            depthShader.use();
            for (unsigned int i = 0; i < 6; ++i)
                depthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
            depthShader.setFloat("far_plane", far_plane);
            depthShader.setVec3("lightPos", lightPos);
            renderScene(depthShader, pyramids, temple1, temple2, temple3, obelisk, palma);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        forModel.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        forModel.setMat4("projection", projection);
        forModel.setMat4("view", view);

        forModel.setVec3("viewPosition", camera.Position);
        forModel.setVec3("lightPos", lightPos);
        forModel.setInt("shadows", shadows); // enable/disable shadows by pressing 'SPACE'
        forModel.setFloat("far_plane", far_plane);

        forModel.setFloat("material.shininess", 64.0f);

        // aktiviramo teksture za pod i depth mapu i sada renderujemo nazad iz pozicije kamere
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        renderScene(forModel, pyramids, temple1, temple2, temple3, obelisk, palma);

        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(3.0f));
        lightShader.setMat4("model", model);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        // na kraju crtamo skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyBoxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyBoxShader.setMat4("view", view);
        skyBoxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

void renderScene(Shader& shader, Model& pyramids, Model& temple1, Model& temple2, Model& temple3, Model& obelisk, Model& palma) {
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", glm::scale(model,glm::vec3(4.0f,1.0f,4.0f)));
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // piramide
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[0]);
    model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
    shader.setMat4("model", model);
    pyramids.Draw(shader);

    // temple1
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[1]);
    model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
    shader.setMat4("model", model);
    temple1.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[2]);
    shader.setMat4("model", model);
    temple2.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[3]);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f,1.0f,0.0f));
    //model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
    shader.setMat4("model", model);
    temple3.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[4]);
    model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
    shader.setMat4("model", model);
    obelisk.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[5]);
    model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
    shader.setMat4("model", model);
    palma.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(modelPosition[5].x + 5.0f, modelPosition[5].y, modelPosition[5].z));
    model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
    shader.setMat4("model", model);
    palma.Draw(shader);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        lightPos.z -= 0.5f;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        lightPos.z += 0.5f;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        lightPos.x -= 0.5f;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        lightPos.x += 0.5f;

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mouseEnablePressed) {
        mouseEnable = !mouseEnable;
        mouseEnablePressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
        mouseEnablePressed = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
