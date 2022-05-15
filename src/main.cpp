#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "utils/camera.hpp"
#include "utils/shader_program.hpp"
#include "utils/model.hpp"
#include "utils/textureCube.hpp"

#include <cstdio>

// settings
const unsigned int WIN_WIDTH = 1280;
const unsigned int WIN_HEIGHT = 900;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Water shader", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwSetWindowPos(window, 300, 100);

    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); 
	glEnable(GL_CULL_FACE);




    // Model terrainModel;
    // if(!terrainModel.load("../assets/models/pirate_island.fbx"))
    // {
    //     fprintf(stderr, "Failed to load terrain model\n");
    //     glfwTerminate();
    //     return -2;
    // }

    const char * skyboxFaces[6] = {
        "../assets/textures/skybox/right.jpg",
        "../assets/textures/skybox/left.jpg",
        "../assets/textures/skybox/top.jpg",
        "../assets/textures/skybox/bottom.jpg",
        "../assets/textures/skybox/front.jpg",
        "../assets/textures/skybox/back.jpg",
    };

    TextureCube skybox;
    if(!skybox.fromFile(skyboxFaces))
    {
        fprintf(stderr, "Failed to load skybox textures\n");
        glfwTerminate();
        return -2;
    }

    Model cubeModel;
    if(!cubeModel.load("../assets/models/cube.obj"))
    {
        fprintf(stderr, "Failed to load cube model\n");
        glfwTerminate();
        return -2;
    }




    // ShaderProgram meshProgram;
    // if(!meshProgram.fromFiles("../assets/shaders/phong_simple.vs.glsl", "../assets/shaders/phong_simple.fs.glsl"))
    // {
    //     fprintf(stderr, "Failed to load simple mesh shader\n");
    //     glfwTerminate();
    //     return -3;
    // }


    // ShaderProgram waterProgram;
    // if(!meshProgram.fromFiles("../assets/shaders/water.vs.glsl", "../assets/shaders/water.fs.glsl"))
    // {
    //     fprintf(stderr, "Failed to load water shader\n");
    //     glfwTerminate();
    //     return -3;
    // }


    ShaderProgram skyboxProgram;
    if(!skyboxProgram.fromFiles("../assets/shaders/skybox.vs.glsl", "../assets/shaders/skybox.fs.glsl"))
    {
        fprintf(stderr, "Failed to load skybox shader\n");
        glfwTerminate();
        return -3;
    }

    GLint unifSkyboxView = glGetUniformLocation(skyboxProgram.getHandle(), "uView");
    GLint unifSkyboxProjection = glGetUniformLocation(skyboxProgram.getHandle(), "uProjection");



    
    Camera camera;




    double prevTime, currTime;
    prevTime = currTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) 
    {
        currTime = glfwGetTime();
        double dt = currTime - prevTime;

        glfwPollEvents();

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        camera.handleInput(window, dt);
        camera.update();



        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // for(auto instance : terrainModel.m_meshInstances)
        // {
        //     instance.draw();
        // }

        glDisable(GL_CULL_FACE);
        skyboxProgram.bind();
        auto viewNoTranslation = glm::mat4(glm::mat3(camera.getView()));
        glUniformMatrix4fv(unifSkyboxView, 1, GL_FALSE, glm::value_ptr(viewNoTranslation));
        glUniformMatrix4fv(unifSkyboxProjection, 1, GL_FALSE, glm::value_ptr(camera.getProjection()));
        skybox.bind();
        cubeModel.m_meshes[0]->draw();


        prevTime = currTime;
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
