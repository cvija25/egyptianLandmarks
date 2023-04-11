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
#include <map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(5.0f, 4.0f, 15.0f));
float lastX = (float)SCR_WIDTH  / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool mouseEnable = false;
bool mouseEnablePressed = false;

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Egipatske znamenitosti", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse



    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_ALWAYS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

    // build and compile shaders
    // -------------------------
    Shader planeShader("resources/shaders/planeVS.vs", "resources/shaders/planeFS.fs");
    Shader forModel("resources/shaders/model_loading.vs", "resources/shaders/model_loading.fs");

    Model pyramids("resources/objects/pyramids/pyramids.obj");
    Model temple1("resources/objects/temple1/temple1.obj");
    Model temple2("resources/objects/temple2/temple2.obj");
    Model temple3("resources/objects/temple3/temple3.obj");
    Model obelisk("resources/objects/ObeliskPiramide/Obelisk+mini pyramids.obj");

    pyramids.SetShaderTextureNamePrefix("material.");
    temple1.SetShaderTextureNamePrefix("material.");
    temple2.SetShaderTextureNamePrefix("material.");
    temple3.SetShaderTextureNamePrefix("material.");
    obelisk.SetShaderTextureNamePrefix("material.");

    PointLight pointLight;
    pointLight.ambient = glm::vec3(0.4, 0.4, 0.2);
    pointLight.diffuse = glm::vec3(0.6, 0.5, 0.6);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.0f;
    pointLight.quadratic = 0.0f;
    pointLight.position = glm::vec3(4.0, 4.0, 4.0);

    glm::vec3 modelPosition [] = {
            glm::vec3(0.0f, -0.5f, 10.0f),glm::vec3(15.0f, -0.5f, 10.0f), glm::vec3(-15.0f, -0.5f, 10.0f),
            glm::vec3(0.0f, -0.5f, 15.0f), glm::vec3(7.0f, -0.4f, 10.0f)
    };

    float planeVertices[] = {
            // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
            5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
            -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

            5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
            5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // load texture
    unsigned int floorTexture = loadTexture("resources/textures/sand.jpg");

    // shader configuration
    // --------------------
    planeShader.use();
    planeShader.setInt("texture1", 0);

    // render loop
    // -----------
    while(!glfwWindowShouldClose(window))
    {
        if (!mouseEnable) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.6f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        planeShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        planeShader.setMat4("view", view);
        planeShader.setMat4("projection", projection);

        // render floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        planeShader.setMat4("model", glm::scale(model,glm::vec3(5.0f,1.0f,5.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        forModel.use();

        float time = glfwGetTime();

        pointLight.position = glm::vec3(0.0f, 10.0f, 0.0f);
        forModel.setVec3("pointLight.position", pointLight.position);
        forModel.setVec3("pointLight.ambient", pointLight.ambient);
        forModel.setVec3("pointLight.diffuse", pointLight.diffuse);
        forModel.setVec3("pointLight.specular", pointLight.specular);
        forModel.setFloat("pointLight.constant", pointLight.constant);
        forModel.setFloat("pointLight.linear", pointLight.linear);
        forModel.setFloat("pointLight.quadratic", pointLight.quadratic);
        forModel.setVec3("viewPosition", camera.Position);
        forModel.setFloat("material.shininess", 64.0f);

        forModel.setMat4("projection", projection);
        forModel.setMat4("view", view);

        // velike piramide
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPosition[0]);
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        forModel.setMat4("model", model);
        pyramids.Draw(forModel);

        // veliki templ
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPosition[1]);
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        forModel.setMat4("model", model);
        temple1.Draw(forModel);

        // srednji templ
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPosition[2]);
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        forModel.setMat4("model", model);
        temple2.Draw(forModel);

        // mali templ
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPosition[3]);
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        forModel.setMat4("model", model);
        temple3.Draw(forModel);

        // obelisk i male piramide
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPosition[4]);
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        forModel.setMat4("model", model);
        obelisk.Draw(forModel);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
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

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !mouseEnablePressed)
    {
        mouseEnable = !mouseEnable;
        mouseEnablePressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        mouseEnablePressed = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
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
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
