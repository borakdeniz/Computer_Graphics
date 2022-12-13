#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
    for(size_t i = 0 ; i <1024; i++) //ilk ba�ta hepsi 0 yani false keylerin
    {
        keys[i] = 0;
    }
    xChange = 0.0f;
    yChange = 0.0f;
    
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

    for (size_t i = 0; i < 1024; i++) //ilk ba�ta hepsi 0 yani false keylerin
    {
        keys[i] = 0;
    }
    xChange = 0.0f;
    yChange = 0.0f;
}

int Window::Initialise()
{
    if (!glfwInit()) //Initialise glfw
    {
        printf("GLFW initialisation failed.!");
        glfwTerminate();
        return 1;
    }
    //Setup glfw window properties
   // Opengl versions
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //opengl versiyonu 3.3 oldu�ui�in ikiside 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //opengl versiyonu 3.3 oldu�u i�in ikiside 3
    //Core profile = No backwards
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    //Allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    MainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL); //window creation
    if (!MainWindow)
    {
        printf("Window creation failed.");
        glfwTerminate();
        return 1;
    }

    //Buffer size info
    glfwGetFramebufferSize(MainWindow, &bufferWidth, &bufferHeight); 

    //set context for glew
    glfwMakeContextCurrent(MainWindow);

    //Handleing key and mouse inputs
    CreateCallBacks();
    //glfwSetInputMode(MainWindow,GLFW_CURSOR, GLFW_CURSOR_DISABLED);    //mouse simgesini kald�r�yor
    //Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initalisation failed.");
        glfwDestroyWindow(MainWindow);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    //Setting up viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);
    glfwSetWindowUserPointer(MainWindow, this);
}

void Window::CreateCallBacks()
{
    glfwSetKeyCallback(MainWindow, HandleKeys); //callbacklerin i�leyi�ini ��zemedim tam
    glfwSetCursorPosCallback(MainWindow, HandleMouse);
}

GLfloat Window::getXChange()
{
    GLfloat theChange = xChange;
    xChange = 0.0f;
    return theChange;

}
GLfloat Window::getYChange()
{
    GLfloat theChange = yChange;
    yChange = 0.0f;
    return theChange;

}
void Window::HandleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window)); //built in command

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if(key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
        {
            theWindow->keys[key] = true; //hangi keye bas�l�rsa o ascii de�eri true olyor
            //printf("Pressed: %d\n", key);
        }
        else if(action == GLFW_RELEASE)
        {
            theWindow->keys[key] = false; //hangi keyden el �ekilirse o asci de�eri tekrardan false oluyor
            //printf("Released: %d\n", key);
        }
    }

}

void Window::HandleMouse(GLFWwindow* window, double xPos, double yPos)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if(theWindow->mouseFirstMove) // son gitti�i yer ve ilk yer aras�ndaki fark� hesaplayarak mouse hareketini yapiyoruz
    {
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;

        theWindow->mouseFirstMove = false;
    }

    theWindow->xChange = xPos - theWindow->lastX;
    theWindow->yChange = theWindow->lastY - yPos;
    theWindow->lastX = xPos;
    theWindow->lastY = yPos;
    
}
Window::~Window()
{
    glfwDestroyWindow(MainWindow);
    glfwTerminate();
}