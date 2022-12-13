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
GLFWwindow* window;

// Include GLM
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"

using namespace glm;

const float toRadian = 3.14159265f / 180.0f; // used for rotation matrix

vector<Mesh*> MeshList; //vector of meshes objects
vector<Shader> ShaderList; //shaders list
Window MainWindow; //ana window
Camera camera;

Texture brickTexture;
Texture dirtTexture;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

float curAngle = 0.0f; //used for rotation

// Vertex Shader code
static const char* vShader = "Shaders/shader.vert"; //vertex shader�n pathwayi: filedan a�mak i�in, pathwayi de�i�tirmene gerek yok san�rsam direk projenin i�inde ��nk�

// filedan �ekiyoruz shaderlar� filelar proje dosyas�n�n i�inde notepadle editliyebiliyoruz filelar� b�ylece daha kolay maini kar��t�rmadan shaderlarla oynayabilyioruz

// Fragment Shader
static const char* fShader = "Shaders/shader.frag"; //fragment shader�n pathway:i filedan a�mak i�in, pathwayi de�i�tirmene gerek yok san�rsam direk projenin i�inde ��nk�


void CreateObjects()
{
    unsigned int indices[] =   
    {
        0,3,1,
        1,3,2,
        2,3,0,
        0,1,2
    };
    GLfloat vertices[] = 
    {                                       //3d piramitin k��elerin kordinatlar� 
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,     //0 �nc� 
        0.0f, -1.0f,1.0f, 0.5f, 0.0f,       // 1.
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,      //2.
        0.0f, 1.0f,0.0f, 0.5f, 1.0f         //3. bu bi �stteki indicisdeki numaralar s�rayla bu sat�rlara kar��l�k bu k��eleri birle�tirerek yan y�zleri olu�turuyoruz her bi y�z� de indicese s�raliyoruz

    };
//bu fonksiyona yukar�daki global define edilen vertices, indices ve array uzunluklar�n� verdi�imiz zaman bize o objeyi yaratiyor ve obje listemize pushluyor.
//Mesh.cpp deki create meshi kullaniyoruz

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 20, 12);
    MeshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 20, 12);
    MeshList.push_back(obj2);
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
    MainWindow = Window(800, 600); //window.cpp yle window a�iyoruz
    MainWindow.Initialise();

    CreateObjects();
    CreateShader();

    camera = Camera(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.01f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTexture();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTexture();

    GLuint  uniformModel = 0, uniformProjection = 0, uniformView = 0;

    mat4 projection = perspective(45.0f, MainWindow.getBufferWidth() / MainWindow.getBufferHeight(), 0.1f, 100.0f); //projection i�in, width ve height d���ndaki parametreleri tam ��zemedim

    while (!MainWindow.getShouldClose()) //window a��k kald��� s�rece
    {

        GLfloat now = glfwGetTime(); // wasd ye bas�nca hareketi hesaplamak i�in ge�en s�reyi hesapliyoruz bu �ekilde
        deltaTime = now - lastTime;
        lastTime = now;


        //Get and handle the user inputs
        glfwPollEvents();
        
        camera.keyControl(MainWindow.getKeys(), deltaTime); // key control i�in window.cpp deki fonksiyonu �a��r�yoz
        camera.mouseControl(MainWindow.getXChange(), MainWindow.getYChange()); // mouse control i�in x ve y eksenindeki de�i�melere bakiyoruz

        //clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ShaderList[0].UseShader(); //shader.cpp deki fonksiyonlar� kullaniyoruz shader� �al��t�rmak i�in
        uniformModel = ShaderList[0].GetModelLocation();
        uniformProjection = ShaderList[0].GetProjectionLocation();
        uniformView = ShaderList[0].GetViewLocation();

        mat4 model(1.0f);
       
       //obj 1 i�in
        model = translate(model, vec3(0.0f, 0.0f, -2.5f));
        //model = rotate(model, curAngle* toRadian, vec3(0.0f, 0.0f, 1.0f));
        model = scale(model, vec3(0.4f, 0.4f, 1.0f));
       
        glUniformMatrix4fv(uniformModel,1, GL_FALSE,value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, value_ptr(camera.calculateViewMatrix()));
        brickTexture.UseTexture();
        MeshList[0]->RenderMesh();

        model = mat4(1.0f); //modeli temizliyoruz obj 2 i�in

        //obj 2 i�in 
        model = translate(model, vec3(0.0f, 1.0f, -2.5f));
        //model = rotate(model, curAngle * toRadian, vec3(0.0f, 0.0f, 1.0f));
        model = scale(model, vec3(0.4f, 0.4f, 1.0f));
       
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, value_ptr(model));

        dirtTexture.UseTexture();
        MeshList[1]->RenderMesh();


        glUseProgram(0);

        MainWindow.swapBuffers();
    }

    return 0;
} 






