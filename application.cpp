#include "application.h"

#include <learnopengl/model.h>

#include <iostream>
#include <cmath>

using namespace std;

static bool firstMouse = true;
static float lastX = 0;
static float lastY = 0;
static float WIDTH;
static float HEIGHT;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;
static float keyboard_speed = 0.05;

class World {
public:
    Camera camera;
    Shader simpleShader;
    Shader lightingShader;
    Model earth;
    float rotation_x;
    float rotation_y;
    bool alive;

    float sphere_x;
    float sphere_z;
    float sphere_rotate_r;
    float sphere_phase;

    unsigned int vao;
    unsigned int diffuseMap;

    glm::mat4 projection;
    glm::mat4 view;

    World() :
        camera(glm::vec3(0.0f, 0.0f, 10.0f)),
        simpleShader("1.model_loading.vs", "1.model_loading.fs"),
        lightingShader("4.1.lighting_maps.vs", "4.1.lighting_maps.fs"),
        earth(FileSystem::getPath("planet.obj"))
    {
        rotation_x = 0.0f;
        rotation_y = 0.0f;
        sphere_x = 0.0f;
        sphere_z = 0.0f;
        sphere_phase = 0.0f;
        sphere_rotate_r = 0.0f;
        alive = true;
    }
};

static World* world = nullptr;

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    world->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    world->camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        world->alive = false;
    } else if (action == GLFW_PRESS) {
        if (world->alive == false) {
            world->alive = true;
        }
    }
}

void app_create_cube() {
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    world->vao = cubeVAO;

    world->diffuseMap = TextureFromFile("container.png", ".");
}

void app_init(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT) {
    lastX = SCR_WIDTH / 2;
    lastY = SCR_HEIGHT / 2;

    WIDTH = SCR_WIDTH;
    HEIGHT = SCR_HEIGHT;

    world = new World();

    app_create_cube();
}

void printCameraPos() {
    cout << world->camera.Position[0] << "\t";
    cout << world->camera.Position[1] << "\t";
    cout << world->camera.Position[2] << "\t" << endl;

}

float app_time() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (world->alive) {
        return deltaTime;
    } else {
        return 0;
    }
}

void app_keyboard(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        world->camera.ProcessKeyboard(FORWARD, deltaTime);
        printCameraPos();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        world->camera.ProcessKeyboard(BACKWARD, deltaTime);
        printCameraPos();
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        world->camera.ProcessKeyboard(LEFT, deltaTime);
        printCameraPos();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        world->camera.ProcessKeyboard(RIGHT, deltaTime);
        printCameraPos();
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        world->rotation_x += keyboard_speed;
        printCameraPos();
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        world->rotation_x -= keyboard_speed;
        printCameraPos();
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        world->rotation_y += keyboard_speed;
        printCameraPos();
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        world->rotation_y -= keyboard_speed;
        printCameraPos();
    }
}

void app_action(float dt) {
    world->sphere_phase += dt;
    world->sphere_x = 3 * sin(world->sphere_phase);
    world->sphere_z = 3 * cos(world->sphere_phase);
    world->sphere_rotate_r += dt;
}

glm::mat4 app_earth() {
    world->simpleShader.use();
    world->simpleShader.setMat4("projection", world->projection);
    world->simpleShader.setMat4("view", world->view);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 local = glm::translate(model, glm::vec3(world->sphere_x, 0, world->sphere_z)); // translate it down so it's at the center of the scene
    local = glm::rotate(local, world->sphere_phase, glm::vec3(0, 1, 0));
    local = glm::scale(local, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
    world->simpleShader.setMat4("model", local);
    world->earth.Draw(world->simpleShader);
    
    return local;
}

void app_cube(glm::mat4 local, int i) {
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    // be sure to activate shader when setting uniforms/drawing objects
    world->lightingShader.use();
    world->lightingShader.setVec3("light.position", lightPos);
    world->lightingShader.setVec3("viewPos", world->camera.Position);

    // light properties
    world->lightingShader.setVec3("light.ambient", 1.2f, 1.2f, 1.2f);
    world->lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    world->lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // material properties
    world->lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    world->lightingShader.setFloat("material.shininess", 64.0f);
    
    world->lightingShader.setMat4("projection", world->projection);
    world->lightingShader.setMat4("view", world->view);

    float k = world->sphere_phase*1.5;

    switch (i) {
    case 0:
        local = glm::rotate(local, k, glm::vec3(0, 0, 1));
        local = glm::translate(local, glm::vec3(6, 0, 0));
        local = glm::rotate(local, k, glm::vec3(0, 1, 0));
        break;
    case 1:
        local = glm::rotate(local, k, glm::vec3(0, 1, 0));
        local = glm::translate(local, glm::vec3(-4, 0, 0));
        local = glm::rotate(local, k, glm::vec3(0, 0, 1));
        break;
    case 2:
        local = glm::rotate(local, k, glm::vec3(1, 1, 0));
        local = glm::translate(local, glm::vec3(-4, 5, 0));
        local = glm::rotate(local, k, glm::vec3(1, 1, 0));
        break;
    case 3:
        local = glm::rotate(local, k, glm::vec3(1, 0, 0));
        local = glm::translate(local, glm::vec3(0, 7, 0));
        local = glm::rotate(local, k, glm::vec3(1, 0, 0));
        break;
    case 4:
        local = glm::rotate(local, k, glm::vec3(0, 1, 0));
        local = glm::translate(local, glm::vec3(0, 0, 5));
        local = glm::rotate(local, k, glm::vec3(0, 0, 1));
        break;
    case 5:
        local = glm::rotate(local, k, glm::vec3(0, 0, 1));
        local = glm::translate(local, glm::vec3(0, 4, 0));
        local = glm::rotate(local, k, glm::vec3(0, 1, 1));
        break;
    }    
    world->lightingShader.setMat4("model", local);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, world->diffuseMap);

    // render the cube
    glBindVertexArray(world->vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void app_frame(GLFWwindow* window) {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float dt = app_time();

    app_keyboard(window);

    app_action(dt);
            
    // world view/projection transformations
    world->projection = glm::perspective(glm::radians(world->camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    world->view = world->camera.GetViewMatrix();
    world->view = glm::rotate(world->view, world->rotation_x, glm::vec3(1, 0, 0));
    world->view = glm::rotate(world->view, world->rotation_y, glm::vec3(0, 1, 0));

    glm::mat4  earthlocal = app_earth();

    for (int i = 0; i < 6; i++) {
        app_cube(earthlocal, i);
    }

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
}

void app_destroy() {
    delete world;
}