#include "Shader.h"

Shader::Shader()
{
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
    
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) 
{
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
    // filedan �ekiyoruz shaderlar� filelar proje dosyas�n�n i�inde notepadle editliyebiliyoruz filelar� b�ylece daha kolay maini kar��t�rmadan shaderlarla oynayabilyioruz
    //bu da filedan okumak i�in haz�r kod gene bu fonksiyonlar san�rsam createfromstringi kullanmaya gerek kalmicak filedan �ekersek
    std::string vertexString = ReadFile(vertexLocation);
    std::string fragmentString = ReadFile(fragmentLocation);
    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
    //klasik c++ file okuma fonksiyonu openglle alakas� yok
    std::string content;
    std::ifstream fileStream(fileLocation, std::ios::in);

    if (!fileStream.is_open()) {
        printf("Failed to read %s! File doesn't exist.", fileLocation);
        return "";
    }

    std::string line = "";
    while (!fileStream.eof())
    {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)  //vertex ve fragment codelar� filedan aliyoruz 
{
    shaderID = glCreateProgram(); //program yarat�p shader �d ye kaydediyoruz

    if (!shaderID)
    {
        printf("Failed to create shaders.");
        return;
    }

    AddShader(shaderID, vertexCode, GL_VERTEX_SHADER); //addshadera g�nderiyoruz codelar�yla beraber
    AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    GLint result = 0; //hata verirse diye 
    GLchar eLog[1024] = { 0 }; // hata verirse diye 

    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s'\n", eLog);
        return;
    }

    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
        return;
    }
    uniformModel = glGetUniformLocation(shaderID, "model"); //location� atiyoruz modelin shader�n i�indeki  model variablena
    uniformProjection = glGetUniformLocation(shaderID, "projection"); //location� atiyoruz projection�n gene shader�n i�indeki projection variable�na
    uniformView = glGetUniformLocation(shaderID, "view");
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) 
{
    GLuint theShader = glCreateShader(shaderType); //shader type bize haz�r geliyo compile shader fonksiyonunun i�inde

    const GLchar* theCode[1]; // tam anlayamad�m ama bunla san�rsam fragment veya vertex oldu�unu anliyoruz
    theCode[0] = shaderCode;

    GLint codeLength[1]; //bunu da tam ��zemedim 
    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0; //hata al�rsak diye
    GLchar eLog[1024] = { 0 }; //hata al�rsak diye

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        fprintf(stderr, "Error compiling the %d shader: '%s'\n", shaderType, eLog);
        return;
    }

    glAttachShader(theProgram, theShader);

}
void Shader::UseShader() 
{
    glUseProgram(shaderID);
}

void Shader::ClearShader()
{ //shader� temizlemek i�in memoryle alakal� gene san�rsam
    if (shaderID != 0)
    {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }

    uniformModel = 0;
    uniformProjection = 0;
    
}

GLuint Shader::GetProjectionLocation() 
{
    return uniformProjection;
}
GLuint Shader::GetModelLocation()
{
    return uniformModel;
}

GLuint Shader::GetViewLocation()
{
    return uniformView;
}

Shader::~Shader()
{
    ClearShader();
}