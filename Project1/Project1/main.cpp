#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>                
#include <string.h>
#include <cmath>
#include <vector>
using namespace std;
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/include/GLFW/glfw3.h>

// Include GLM
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"
#include "SpotLight.h"
#include "Model.h"

#include "assimp/Importer.hpp"

using namespace glm;

const float toRadians = 3.14159265f / 180.0f;// used for rotation matrix
GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0, uniformOmniLightPos = 0, uniformFarPlane = 0;

Window MainWindow; //ana window

std::vector<Mesh*> MeshList; //vector of meshes objects

std::vector<Shader> ShaderList; //shaders list
Shader directionalShadowShader;
Shader omniShadowShader;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

Model xwing;
Model SeaHawk;
Model Audi;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat SeaHawkAngle = 0.0f;
GLfloat xwingAngle = 0.0f;


// Vertex Shader code
static const char* vShader = "Shaders/shader.vert"; //vertex shader�n pathwayi: filedan a�mak i�in, pathwayi de�i�tirmene gerek yok san�rsam direk projenin i�inde ��nk�

// filedan �ekiyoruz shaderlar� filelar proje dosyas�n�n i�inde notepadle editliyebiliyoruz filelar� b�ylece daha kolay maini kar��t�rmadan shaderlarla oynayabilyioruz

// Fragment Shader
static const char* fShader = "Shaders/shader.frag"; //fragment shader�n pathway:i filedan a�mak i�in, pathwayi de�i�tirmene gerek yok san�rsam direk projenin i�inde ��nk�

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
    unsigned int vLength, unsigned int normalOffset)
{
    for (size_t i = 0; i < indiceCount; i += 3)
    {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;
        vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        vec3 normal = cross(v1, v2);
        normal = normalize(normal);

        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
        vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
        vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
        vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
    }

    for (size_t i = 0; i < verticeCount / vLength; i++)
    {
        unsigned int nOffset = i * vLength + normalOffset;
        vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = normalize(vec);
        vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
    }
}

//create objects are reduced into a single function
void CreateObjects()
{
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
        //	x      y      z			u	  v			nx	  ny    nz
            -1.0f, -1.0f, -0.6f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
        -10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
    };

    calcAverageNormals(indices, 12, vertices, 32, 8, 5);


    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 32, 12);
    MeshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 32, 12);
    MeshList.push_back(obj2);

    Mesh* obj3 = new Mesh();
    obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
    MeshList.push_back(obj3);
}


void CreateShader()
{
    //shader.cpp de a��klad�m bu k�sm� file �m�z var proje klas�r�nde b�t�n bu headerlar�n mainin oldu�u yerde shaders klas�r�i�inde fragment ve vertex var o filelar�
    //notepadle editliyip shaderlar� de�i�tirebiliyoruz kolayca maini bozmadan. Filelar� a��p okumay� i�lemeyi shader.cpp deki fonksiyonlar hallediyor.
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    ShaderList.push_back(*shader1);
    
    directionalShadowShader = Shader();
    directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
    
    omniShadowShader = Shader();
    omniShadowShader.CreateFromFiles("Shaders/omni_directional_shadow_map.vert", "Shaders/omni_directional_shadow_map.geom", "Shaders/omni_directional_shadow_map.frag");
}

void RenderScene()
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    MeshList[0]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    MeshList[1]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    MeshList[2]->RenderMesh();


    xwingAngle += 0.1f;
    if (xwingAngle > 360.0f)
    {
        xwingAngle = 0.1f;
    }

    model = glm::mat4(1.0f);
    model = glm::rotate(model, -xwingAngle* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 10.0f));
    model = glm::rotate(model, toRadians, glm::vec3(1.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    xwing.RenderModel();


    SeaHawkAngle -= 0.1f;
    if (SeaHawkAngle > 360.0f)
    {
        SeaHawkAngle = 0.1f;
    }

    model = glm::mat4(1.0f);
    model = glm::rotate(model, -SeaHawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-8.0f, 0.0f, 0.0f));
    model = glm::rotate(model, toRadians, glm::vec3(1.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    SeaHawk.RenderModel();


    /* model = glm::mat4(1.0f);
     model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 10.0f));
     model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
     glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
     shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
     Audi.RenderModel(); */
}


void DirectionalShadowMapPass(DirectionalLight* light)
{
    directionalShadowShader.UseShader();

    glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

    light->GetShadowMap()->Write();
    glClear(GL_DEPTH_BUFFER_BIT);

    uniformModel = directionalShadowShader.GetModelLocation();

    mat4 lightTransform = light->CalculateLightTransform();
    directionalShadowShader.SetDirectionalLightTransform(&lightTransform);

    directionalShadowShader.Validate();
    RenderScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void OmniShadowMapPass(PointLight* light)
{
    glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

    omniShadowShader.UseShader();
    uniformModel = omniShadowShader.GetModelLocation();
    uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
    uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

    light->GetShadowMap()->Write();

    glClear(GL_DEPTH_BUFFER_BIT);

    glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
    glUniform1f(uniformFarPlane, light->GetFarPlane());
    omniShadowShader.SetOmniLightMatrices(light->CalculateLightTransform());

    omniShadowShader.Validate();
    RenderScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(mat4 projectionMatrix, mat4 viewMatrix)
{
    ShaderList[0].UseShader();

    uniformModel = ShaderList[0].GetModelLocation();
    uniformProjection = ShaderList[0].GetProjectionLocation();
    uniformView = ShaderList[0].GetViewLocation();
    uniformEyePosition = ShaderList[0].GetEyePositionLocation();
    uniformSpecularIntensity = ShaderList[0].GetSpecularIntensityLocation();
    uniformShininess = ShaderList[0].GetShininessLocation();

    glViewport(0, 0, 1366, 768);

    // Clear the window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

    ShaderList[0].SetDirectionalLight(&mainLight);
    ShaderList[0].SetPointLights(pointLights, pointLightCount, 3, 0);
    ShaderList[0].SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);

    mat4 mainLightTransform = mainLight.CalculateLightTransform();
    ShaderList[0].SetDirectionalLightTransform(&mainLightTransform);

    mainLight.GetShadowMap()->Read(GL_TEXTURE2);


    ShaderList[0].SetTexture(1);
    ShaderList[0].SetDirectionalShadowMap(2);

    glm::vec3 lowerLight = camera.getCameraPosition();
    lowerLight.y -= 0.3f;
    spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

    ShaderList[0].Validate();
    RenderScene();
}

int main()
{
    MainWindow = Window(1366, 768); //window.cpp yle window a�iyoruz
    MainWindow.Initialise();

    CreateObjects();
    CreateShader();

    camera = Camera(vec3(.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();

    shinyMaterial = Material(1.0f, 32);
    dullMaterial = Material(0.3f, 4);

    xwing = Model();
    xwing.LoadModel("Models/x-wing.obj");

    SeaHawk = Model();
    SeaHawk.LoadModel("Models/Seahawk.obj"); 

    mainLight = DirectionalLight(2048, 2048,
        1.0f, 1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, -15.0f, -10.0f);

    pointLights[1] = PointLight(1024, 1024,
        0.1f, 100.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.4f,
        2.0f, 2.0f, 0.0f,
        0.3f, 0.01f, 0.01f);
    pointLightCount++;

    pointLights[0] = PointLight(1024, 1024,
        0.1f, 100.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.4f,
        -2.0f, 2.0f, 0.0f,
        0.3f, 0.01f, 0.01f);
    pointLightCount++;

    spotLights[0] = SpotLight(1024, 1024,
        0.1f, 100.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 2.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        20.0f);
    spotLightCount++;

    spotLights[1] = SpotLight(1024, 1024,
        0.1f, 100.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, -1.5f, 0.0f,
        -100.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        20.0f);
    spotLightCount++;

   
    mat4 projection = perspective(radians(60.0f), (GLfloat)MainWindow.getBufferWidth() / MainWindow.getBufferHeight(), 0.1f, 100.0f); //projection i�in, width ve height d���ndaki parametreleri tam ��zemedim

   

    while (!MainWindow.getShouldClose()) //window a��k kald��� s�rece
    {

        GLfloat now = glfwGetTime(); // wasd ye bas�nca hareketi hesaplamak i�in ge�en s�reyi hesapliyoruz bu �ekilde
        deltaTime = now - lastTime;
        lastTime = now;


        //Get and handle the user inputs
        glfwPollEvents();

        camera.keyControl(MainWindow.getsKeys(), deltaTime);
        camera.mouseControl(MainWindow.getXChange(), MainWindow.getYChange());

        if (MainWindow.getsKeys()[GLFW_KEY_L])
        {
            spotLights[0].Toggle();
            MainWindow.getsKeys()[GLFW_KEY_L] = false;
        }

        
        DirectionalShadowMapPass(&mainLight);

        for (size_t i = 0; i < pointLightCount; i++)
        {
            OmniShadowMapPass(&pointLights[i]);
        }
        for (size_t i = 0; i < spotLightCount; i++)
        {
            OmniShadowMapPass(&spotLights[i]);
        }
        RenderPass(projection, camera.calculateViewMatrix());

        glUseProgram(0);

        MainWindow.swapBuffers();
    }

    return 0;
} 






