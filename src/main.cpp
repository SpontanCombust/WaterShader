#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "utils/camera.hpp"
#include "utils/shader_program.hpp"
#include "utils/model.hpp"
#include "utils/textureCube.hpp"
#include "utils/framebuffer.hpp"

#include <algorithm>
#include <cstdio>

// settings
const unsigned int WIN_WIDTH = 1280;
const unsigned int WIN_HEIGHT = 720;

const unsigned int REFRACTION_WIDTH = WIN_WIDTH;
const unsigned int REFRACTION_HEIGHT = WIN_HEIGHT;
const unsigned int REFLECTION_WIDTH = WIN_WIDTH / 4;
const unsigned int REFLECTION_HEIGHT = WIN_HEIGHT / 4;

const float WATER_HEIGHT = -0.10f;
const float WATER_WAVE_SPEED = 0.008f;


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
	// glEnable(GL_CULL_FACE);




    Model landscapeModel;
    if(!landscapeModel.load("../assets/models/Landscape.obj"))
    {
        fprintf(stderr, "Failed to load terrain model\n");
        glfwTerminate();
        return -2;
    }
    MeshInstance terrainMesh = *std::find_if(
        landscapeModel.m_meshInstances.begin(), 
        landscapeModel.m_meshInstances.end(), 
        [] (const MeshInstance& inst){
            return inst.getName().find("Terrain") != std::string::npos;
        }
    );
    MeshInstance waterMesh = *std::find_if(
        landscapeModel.m_meshInstances.begin(), 
        landscapeModel.m_meshInstances.end(), 
        [] (const MeshInstance& inst){
            return inst.getName().find("Water") != std::string::npos;
        }
    );

    auto dudv = std::make_shared<Texture2D>();
    dudv->fromFile("../assets/textures/dudv.jpg");


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




    ShaderProgram terrainProgram;
    if(!terrainProgram.fromFiles("../assets/shaders/phong_simple.vs.glsl", "../assets/shaders/phong_simple.fs.glsl"))
    {
        fprintf(stderr, "Failed to load simple mesh shader\n");
        glfwTerminate();
        return -3;
    }

    GLint unifTerrainModel = glGetUniformLocation(terrainProgram.getHandle(), "uModel");
    GLint unifTerrainView = glGetUniformLocation(terrainProgram.getHandle(), "uView");
    GLint unifTerrainProjection = glGetUniformLocation(terrainProgram.getHandle(), "uProjection");
    // GLint unifTerrainCameraPos = glGetUniformLocation(terrainProgram.getHandle(), "uCameraPosition");
    GLint unifTerrainLightDir = glGetUniformLocation(terrainProgram.getHandle(), "uLight.direction");
    GLint unifTerrainLightAmbient = glGetUniformLocation(terrainProgram.getHandle(), "uLight.ambient");
    GLint unifTerrainLightDiffuse = glGetUniformLocation(terrainProgram.getHandle(), "uLight.diffuse");
    // GLint unifTerrainLightSpecular = glGetUniformLocation(terrainProgram.getHandle(), "uLight.specular");
    

    ShaderProgram waterProgram;
    if(!waterProgram.fromFiles("../assets/shaders/water.vs.glsl", "../assets/shaders/water.fs.glsl"))
    {
        fprintf(stderr, "Failed to load water shader\n");
        glfwTerminate();
        return -3;
    }

    GLint unifWaterModel = glGetUniformLocation(waterProgram.getHandle(), "uModel");
    GLint unifWaterView = glGetUniformLocation(waterProgram.getHandle(), "uView");
    GLint unifWaterProjection = glGetUniformLocation(waterProgram.getHandle(), "uProjection");
    GLint unifWaterCameraPosition = glGetUniformLocation(waterProgram.getHandle(), "uCameraPosition");
    GLint unifWaterTextureRefraction = glGetUniformLocation(waterProgram.getHandle(), "uTextureRefraction");
    GLint unifWaterTextureReflection = glGetUniformLocation(waterProgram.getHandle(), "uTextureReflection");
    GLint unifWaterTextureDUDV = glGetUniformLocation(waterProgram.getHandle(), "uDUDV");
    GLint unifWaterWaveMovement = glGetUniformLocation(waterProgram.getHandle(), "uWaveMovement");

    float waveMovement = 0.f;

    waterProgram.bind();
    glUniform1i(unifWaterTextureRefraction, 0);
    glUniform1i(unifWaterTextureReflection, 1);
    glUniform1i(unifWaterTextureDUDV, 2);


    ShaderProgram skyboxProgram;
    if(!skyboxProgram.fromFiles("../assets/shaders/skybox.vs.glsl", "../assets/shaders/skybox.fs.glsl"))
    {
        fprintf(stderr, "Failed to load skybox shader\n");
        glfwTerminate();
        return -3;
    }

    GLint unifSkyboxView = glGetUniformLocation(skyboxProgram.getHandle(), "uView");
    GLint unifSkyboxProjection = glGetUniformLocation(skyboxProgram.getHandle(), "uProjection");

    Framebuffer windowFramebuffer;
    windowFramebuffer.fromWindow(window);
    Framebuffer refractionFramebuffer;
    refractionFramebuffer.fromBlank({ REFRACTION_WIDTH, REFRACTION_HEIGHT });
    Framebuffer reflectionFramebuffer;
    reflectionFramebuffer.fromBlank({ REFLECTION_WIDTH, REFLECTION_HEIGHT });

    waterMesh.getMesh()->setTexture(0, refractionFramebuffer.getOwnedTarget());
    waterMesh.getMesh()->setTexture(1, reflectionFramebuffer.getOwnedTarget());
    waterMesh.getMesh()->setTexture(2, dudv);



    
    Camera camera;
    camera.setPosition({ 0.0f, 2.0f, 2.0f });



    double prevTime, currTime;
    prevTime = currTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) 
    {
        currTime = glfwGetTime();
        double dt = currTime - prevTime;
        waveMovement = std::fmod(waveMovement + WATER_WAVE_SPEED * dt, 1.f);

        glfwPollEvents();

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        camera.handleInput(window, dt);
        camera.update();


        // setup terrain & sky constants
        terrainProgram.bind();
        glUniformMatrix4fv(unifTerrainModel, 1, GL_FALSE, glm::value_ptr(terrainMesh.getTransform()));
        glUniformMatrix4fv(unifTerrainProjection, 1, GL_FALSE, glm::value_ptr(camera.getProjection()));
        glUniform3fv(unifTerrainLightDir, 1, glm::value_ptr(glm::vec3(-1.0f, -0.5f, 0.2f)));
        glUniform3fv(unifTerrainLightAmbient, 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
        glUniform3fv(unifTerrainLightDiffuse, 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
        skyboxProgram.bind();
        glUniformMatrix4fv(unifSkyboxProjection, 1, GL_FALSE, glm::value_ptr(camera.getProjection()));

        auto drawTerrainAndSky = [&] {
            // glEnable(GL_CULL_FACE);
            terrainProgram.bind();
            glUniformMatrix4fv(unifTerrainView, 1, GL_FALSE, glm::value_ptr(camera.getView()));
            terrainMesh.draw();

            // glDisable(GL_CULL_FACE);
            skyboxProgram.bind();
            auto viewNoTranslation = glm::mat4(glm::mat3(camera.getView()));
            glUniformMatrix4fv(unifSkyboxView, 1, GL_FALSE, glm::value_ptr(viewNoTranslation));
            skybox.bind();
            cubeModel.m_meshes[0]->draw();
        };


        //reflection pass
        float dist = 2 * (camera.getPosition().y - WATER_HEIGHT);
        camera.setPosition(camera.getPosition() - glm::vec3(0.f, dist, 0.f));
        camera.setRotation(camera.getYaw(), -camera.getPitch());
        camera.update();

        reflectionFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawTerrainAndSky();

        camera.setPosition(camera.getPosition() + glm::vec3(0.f, dist, 0.f));
        camera.setRotation(camera.getYaw(), -camera.getPitch());
        camera.update();


        // refraction pass
        refractionFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawTerrainAndSky();


        // water pass
        windowFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawTerrainAndSky();

        // glEnable(GL_CULL_FACE);
        waterProgram.bind();
        glUniformMatrix4fv(unifWaterModel, 1, GL_FALSE, glm::value_ptr(waterMesh.getTransform()));
        glUniformMatrix4fv(unifWaterView, 1, GL_FALSE, glm::value_ptr(camera.getView()));
        glUniformMatrix4fv(unifWaterProjection, 1, GL_FALSE, glm::value_ptr(camera.getProjection()));
        glUniform3fv(unifWaterCameraPosition, 1, glm::value_ptr(camera.getPosition()));
        glUniform1f(unifWaterWaveMovement, waveMovement);
        waterMesh.draw();



        prevTime = currTime;
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
