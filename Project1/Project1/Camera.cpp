#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); // yaw sa� sol pitch yukar� a�a�� x ekseninin frontunu ikisini �arparak hesapliyoruz
	front.y = sin(glm::radians(pitch)); //y ekseni sadece pitch
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)); // z ekseni i�in ikisini �arp�p sin�s aliyoruz bu sefer
	front = glm::normalize(front); //magnitude u kald�r�yoruz ne kadar uzakta oldu�u de�il de frontun ne y�nde oldu�unu anlamak i�in

	right = glm::normalize(glm::cross(front, worldUp)); //front ve world up �n cross product� right'� veriyor
	up = glm::normalize(glm::cross(right, front)); // right ve frontun cross product� da personal up � veriyor
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime; // wasd ye bas�nca hareket h�z�n� maindeki de�i�imi hesaplad�ktan sonra bu fonksiyonda movespeedle �arp�p eski oldu�u locationa ekliyoruz


	if (keys[GLFW_KEY_W])
	{
		position += front * velocity;
	}

	if (keys[GLFW_KEY_S])
	{
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_D])
	{
		position += right * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed; //mouse control i�in tam anlayamad�m bende fonksiyonu
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}
glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up); //ilk parametre nerden bakt���m�z, 2.parametre nereye bakt���m�z, 3.parametre de world up �n nerde oldu�u
	//bakt���m�z yeri hesapliyoruz k�saca bu parametrelerle
}

Camera::~Camera()
{
}
