#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "utils/camera.hpp"
#include "utils/shader_program.hpp"
#include "utils/model.hpp"
#include "utils/textureCube.hpp"
#include "utils/framebuffer.hpp"

#include <algorithm>
#include <cstdio>

namespace imgui = ImGui;


// settings
const unsigned int WIN_WIDTH = 1280;
const unsigned int WIN_HEIGHT = 720;

const unsigned int REFRACTION_WIDTH = WIN_WIDTH;
const unsigned int REFRACTION_HEIGHT = WIN_HEIGHT;
const unsigned int REFLECTION_WIDTH = WIN_WIDTH / 4;
const unsigned int REFLECTION_HEIGHT = WIN_HEIGHT / 4;

const float WATER_HEIGHT = 0.0f;

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


    imgui::CreateContext();
    imgui::StyleColorsDark();
    ImGuiIO& io = imgui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    imgui::StyleColorsDark();




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
    GLint unifTerrainClipPlane = glGetUniformLocation(terrainProgram.getHandle(), "uClipPlane");
    

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
    GLint unifWaterWaveStrength = glGetUniformLocation(waterProgram.getHandle(), "uWaveStrength");
    GLint unifWaterWaveMovement = glGetUniformLocation(waterProgram.getHandle(), "uWaveMovement");
    GLint unifWaterReflecivity = glGetUniformLocation(waterProgram.getHandle(), "uReflecivity");
    GLint unifWaterTint = glGetUniformLocation(waterProgram.getHandle(), "uTint");

    float waveMovement = 0.f;
    float waterWaveSpeed = 0.008f;
    float waveStrength = 0.01f;
    float waterReflecivity = 1.f;
    glm::vec4 waterTint(0.f, 0.05, 0.15f, 1.f);

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
        waveMovement = std::fmod(waveMovement + waterWaveSpeed * dt, 1.f);

        glfwPollEvents();


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        imgui::NewFrame();
        imgui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        {
            imgui::Text("W, A, S, D - move around");
            imgui::Text("Q, E - disable/enable camera rotation");
            imgui::NewLine();
            imgui::SliderFloat("Water wave strength", &waveStrength, 0.f, 0.5f);
            imgui::SliderFloat("Water wave speed", &waterWaveSpeed, 0.f, 0.1f);
            imgui::SliderFloat("Water reflecivity", &waterReflecivity, 0.f, 8.f);
            imgui::ColorEdit4("Water tint", glm::value_ptr(waterTint));
        }
        imgui::End();
        imgui::Render();



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

        auto drawTerrainAndSky = [&] (const glm::vec4& terrainClipPlane) {
            // glEnable(GL_CULL_FACE);
            terrainProgram.bind();
            glUniform4fv(unifTerrainClipPlane, 1, glm::value_ptr(terrainClipPlane));
            glUniformMatrix4fv(unifTerrainView, 1, GL_FALSE, glm::value_ptr(camera.getView()));
            terrainMesh.draw();

            // glDisable(GL_CULL_FACE);
            skyboxProgram.bind();
            auto viewNoTranslation = glm::mat4(glm::mat3(camera.getView()));
            glUniformMatrix4fv(unifSkyboxView, 1, GL_FALSE, glm::value_ptr(viewNoTranslation));
            skybox.bind();
            cubeModel.m_meshes[0]->draw();
        };


        glEnable(GL_CLIP_DISTANCE0);

        //reflection pass
        float dist = 2 * (camera.getPosition().y - WATER_HEIGHT);
        camera.setPosition(camera.getPosition() - glm::vec3(0.f, dist, 0.f));
        camera.setRotation(camera.getYaw(), -camera.getPitch());
        camera.update();

        reflectionFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawTerrainAndSky(glm::vec4(0.f, 1.f, 0.f, -WATER_HEIGHT));

        camera.setPosition(camera.getPosition() + glm::vec3(0.f, dist, 0.f));
        camera.setRotation(camera.getYaw(), -camera.getPitch());
        camera.update();


        // refraction pass
        refractionFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawTerrainAndSky(glm::vec4(0.f, -1.f, 0.f, WATER_HEIGHT));



        // water pass
        glDisable(GL_CLIP_DISTANCE0);
        windowFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawTerrainAndSky(glm::vec4(0.f, -1.f, 0.f, 15.f));

        // glEnable(GL_CULL_FACE);
        waterProgram.bind();
        glUniformMatrix4fv(unifWaterModel, 1, GL_FALSE, glm::value_ptr(waterMesh.getTransform()));
        glUniformMatrix4fv(unifWaterView, 1, GL_FALSE, glm::value_ptr(camera.getView()));
        glUniformMatrix4fv(unifWaterProjection, 1, GL_FALSE, glm::value_ptr(camera.getProjection()));
        glUniform3fv(unifWaterCameraPosition, 1, glm::value_ptr(camera.getPosition()));
        glUniform1f(unifWaterWaveStrength, waveStrength);
        glUniform1f(unifWaterWaveMovement, waveMovement);
        glUniform1f(unifWaterReflecivity, waterReflecivity);
        glUniform4fv(unifWaterTint, 1, glm::value_ptr(waterTint));
        waterMesh.draw();



        prevTime = currTime;
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
