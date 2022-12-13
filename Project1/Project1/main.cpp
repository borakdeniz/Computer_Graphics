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

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include "assimp/Importer.hpp"

using namespace glm;

const float toRadian = 3.14159265f / 180.0f; // used for rotation matrix

vector<Mesh*> MeshList; //vector of meshes objects
vector<Shader> ShaderList; //shaders list
Window MainWindow; //ana window
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

float curAngle = 0.0f; //used for rotation

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
        vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

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
}


int main(void)
{
    MainWindow = Window(1366, 768); //window.cpp yle window a�iyoruz
    MainWindow.Initialise();

    CreateObjects();
    CreateShader();

    camera = Camera(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();

    shinyMaterial = Material(1.0f, 32);
    dullMaterial = Material(0.3f, 4);

    mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
                                 0.0f, 0.0f,
                                 0.0f, 0.0f, -1.0f);
    unsigned int pointLightCount = 0;
    pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
                                0.0f, 1.0f,
                                0.0f, 0.0f, 0.0f,
                                0.3f, 0.2f, 0.1f);
    pointLightCount++;
    pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
                                0.0f, 1.0f,
                                -4.0f, 2.0f, 0.0f,
                                0.3f, 0.1f, 0.1f);
    pointLightCount++;

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
        uniformSpecularIntensity = 0, uniformShininess = 0;

    mat4 projection = perspective(radians(45.0f), (GLfloat)MainWindow.getBufferWidth() / MainWindow.getBufferHeight(), 0.1f, 100.0f); //projection i�in, width ve height d���ndaki parametreleri tam ��zemedim

    while (!MainWindow.getShouldClose()) //window a��k kald��� s�rece
    {

        GLfloat now = glfwGetTime(); // wasd ye bas�nca hareketi hesaplamak i�in ge�en s�reyi hesapliyoruz bu �ekilde
        deltaTime = now - lastTime;
        lastTime = now;


        //Get and handle the user inputs
        glfwPollEvents();
        
        camera.keyControl(MainWindow.getsKeys(), deltaTime); // key control i�in window.cpp deki fonksiyonu �a��r�yoz
        camera.mouseControl(MainWindow.getXChange(), MainWindow.getYChange()); // mouse control i�in x ve y eksenindeki de�i�melere bakiyoruz

        //clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ShaderList[0].UseShader();
        uniformModel = ShaderList[0].GetModelLocation();
        uniformProjection = ShaderList[0].GetProjectionLocation();
        uniformView = ShaderList[0].GetViewLocation();
        uniformEyePosition = ShaderList[0].GetEyePositionLocation();
        uniformSpecularIntensity = ShaderList[0].GetSpecularIntensityLocation();
        uniformShininess = ShaderList[0].GetShininessLocation();

        ShaderList[0].SetDirectionalLight(&mainLight);
        ShaderList[0].SetPointLights(pointLights, pointLightCount);

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

        mat4 model(1.0f);
       
       //obj 1 i�in
        model = translate(model, vec3(0.0f, 0.0f, -2.5f));
        //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        brickTexture.UseTexture();
        shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        MeshList[0]->RenderMesh();

        model = mat4(1.0f);
        model = translate(model, vec3(0.0f, 4.0f, -2.5f));
        //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, value_ptr(model));
        plainTexture.UseTexture();
        dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        MeshList[1]->RenderMesh();

        model = mat4(1.0f);
        model = translate(model, vec3(0.0f, -2.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dirtTexture.UseTexture();
        shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        MeshList[2]->RenderMesh();

        glUseProgram(0);

        MainWindow.swapBuffers();
    }

    return 0;
} 






