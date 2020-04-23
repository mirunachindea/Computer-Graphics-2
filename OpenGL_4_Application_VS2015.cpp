//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include "stdafx.h"
#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/ext.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"

using namespace std;
int glWindowWidth = 640;
int glWindowHeight = 480;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

// directional light
glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
int dirLightOn = 1;

// point light
glm::vec3 lightDir2;
GLuint lightDirLoc2;
glm::vec3 lightColor2;
GLuint lightColorLoc2;
float intensityPoint = 1;
GLuint intensityLoc;
int dirIntensity = 0;

// spotlight
glm::vec3 lightDir3;
GLuint lightDirLoc3;
glm::vec3 lightPos3;
GLuint lightPosLoc3;
glm::vec3 lightColor3;
GLuint lightColorLoc3;
float cutOff;
GLuint cutOffLoc;
float outerCutOff;
GLuint outerCutOffLoc;
int spotLightOn = 0;


gps::Camera myCamera(glm::vec3(2.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
GLfloat cameraSpeed = 0.1f;

bool pressedKeys[1024];
GLfloat angle;
GLfloat angle2;
GLfloat lightAngle;
GLfloat lightAngle2;

std::vector<glm::mat4> models(25);

gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D aircraft1;
gps::Model3D aircraft;
gps::Model3D r2d2;
gps::Model3D c3po;
gps::Model3D millenium_falcon;
gps::Model3D sandtrooper;
gps::Model3D corvette;
gps::Model3D death_star;
gps::Model3D floating_city;
gps::Model3D sphere;
gps::Model3D sunglyder;
gps::Model3D bb8;
gps::Model3D bb8body;
gps::Model3D bb8head;
gps::Model3D robot;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
//glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float delta = 0;
float delta2 = 0;
float delta3 = 0;
float angle3 = 0.0f;
int dird2 = 0;
float movementSpeed = 1;
float movementSpeed2 = 0.5;
float movementSpeed3 = 1.0;
float rotationspeed = 85;

int enableanimationsphere = 0;
int enableanimation = 0;
int enableanimationbb8 = 0;
int enablecameraanimation = 0;
double pressedAnimButton;

int polygonal;
GLuint polygonalLoc;

std::vector<std::vector <glm::vec3>> bboxes;


int computedBB = 0;


void updateDelta(double elapsedSeconds) {
	if (enableanimation)
		delta += movementSpeed * elapsedSeconds;
}

void updateDelta2(double elapsedSeconds) {
	if (enableanimationsphere)
		delta2 += movementSpeed2 * elapsedSeconds;
}

void updateDelta3(double elapsedSeconds) {
	if (enableanimationbb8)
		delta3 += movementSpeed3 * elapsedSeconds;
}

void updateAnglebb8(double elapsedSeconds) {
	if (enableanimationbb8) {
		angle3 += rotationspeed * elapsedSeconds;
		if (angle3 > 360.0f)
			angle3 -= 360.0f;
	}
}

double lastTimeStamp = glfwGetTime();

void computeBoundingBox(gps::Model3D object, glm::mat4 mod) {
	std::vector<glm::vec3> bbox;
	// min
	glm::vec3 bboxmin = object.getBoundingBox()[0];
	glm::vec4 bboxtransf = mod * glm::vec4(bboxmin, 1.0);
	bboxmin.x = bboxtransf.x; bboxmin.y = bboxtransf.y; bboxmin.z = bboxtransf.z;
	// max
	glm::vec3 bboxmax = object.getBoundingBox()[1];
	bboxtransf = mod * glm::vec4(bboxmax, 1.0);
	bboxmax.x = bboxtransf.x; bboxmax.y = bboxtransf.y; bboxmax.z = bboxtransf.z;
	// add in bbox list of object
	bbox.push_back(bboxmin);
	bbox.push_back(bboxmax);
	bboxes.push_back(bbox);
	// empty bbox list
	bbox.pop_back();
	bbox.pop_back();
}


void computeAllBoundingBoxes() {
	//computeBoundingBox(r2d2, models.at(0));
	//cout << "size of models" << models.size() << "\n";
	//cout <<"model aircraft"<< glm::to_string(models[1])<<"\n";
	//cout << "model r2d2" << glm::to_string(models[3]) << "\n";
	computeBoundingBox(aircraft, models.at(0));
	computeBoundingBox(aircraft, models.at(1));
	computeBoundingBox(aircraft, models.at(2));
	computeBoundingBox(r2d2, models.at(3));
	computeBoundingBox(c3po, models.at(4));
	//computeBoundingBox(ground, models.at(5));
	computeBoundingBox(bb8body, models.at(5));
	computeBoundingBox(bb8head, models.at(6));
}

int canMove(gps::Camera cam) {
	glm::vec3 pos = cam.getCameraPositionAll();
	cout << "BOUNDING BOXES SIZE"<<bboxes.size()<<"\n";
	for (int i = 0; i < bboxes.size(); i++) {
		std::vector<glm::vec3> bbox = bboxes[i];
		cout << "camera pos: " << pos.x << " " << pos.y << " " << pos.z << "\n";
		cout << "bounding box min: " << bbox[0].x << " " << bbox[0].y << " " << bbox[0].z << "\n";
		cout << "bounding box max: " << bbox[1].x << " " << bbox[1].y << " " << bbox[1].z << "\n";
		if ( ((pos.x > bbox[0].x && pos.x < bbox[1].x) &&
			(pos.y > bbox[0].y && pos.y < bbox[1].y) &&
			(pos.z > bbox[0].z && pos.z < bbox[1].z)) || 
			(abs(pos.x - bbox[0].x) < 1 || abs(pos.x - bbox[1].x) < 1) && 
			(abs(pos.y - bbox[0].y) < 1 || abs(pos.y - bbox[1].y) < 1) &&
			(abs(pos.z - bbox[0].z) < 1|| abs(pos.z - bbox[1].z) < 1))
			{
			cout << "INAUNTRUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU la "<<i<<"\n";
			return 0;
		}
	}
	return 1;
}


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.5;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;


	myCamera.rotate(pitch, yaw);

}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

	if (yoffset > 0) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed * 50);
	}
	else {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed * 50);
	}

}

void startCameraAnimation(double elapsedSeconds) {

	if (enablecameraanimation == 1) {
		if (elapsedSeconds < 2) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 2);
			myCamera.moveUD(0, cameraSpeed / 32);
		}
		else if (elapsedSeconds < 4) {
			myCamera.moveUD(0, cameraSpeed / 4);
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 3);
		}
		else if (elapsedSeconds < 8) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 3);
		}
		
		else if (elapsedSeconds < 16) {
			myCamera.moveUD(0, cameraSpeed / 8);
			//angle += 0.12f;
			yaw += 0.12f;
			myCamera.rotate(pitch, yaw);
		}
		else if (elapsedSeconds < 20) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 1.5);
			myCamera.moveUD(1, cameraSpeed / 1.5);
		}
		else if (elapsedSeconds < 36) {
			//angle += 0.15f;
			yaw += 0.15f;
			myCamera.rotate(pitch, yaw);
			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed / 2);
			if (elapsedSeconds > 32)
				enableanimationsphere = 1;
		}
		else if (elapsedSeconds < 38) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed / 2);
			//angle += 0.15f;
			yaw += 0.15f;
			myCamera.rotate(pitch, yaw);
			enableanimation = 1;
		}
		else if (elapsedSeconds < 64) {
			//angle += 0.35f;
			yaw += 0.35f;
			myCamera.rotate(pitch, yaw);
		}

	}

}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		yaw -= 1.0f;
		myCamera.rotate(pitch, yaw);
	}

	if (pressedKeys[GLFW_KEY_E]) {
		yaw += 1.0f;
		myCamera.rotate(pitch, yaw);
	}

	if (pressedKeys[GLFW_KEY_T]) {
		pitch -= 1.0f;
		myCamera.rotate(pitch, yaw);
	}

	if (pressedKeys[GLFW_KEY_G]) {
		pitch += 1.0f;
		myCamera.rotate(pitch, yaw);
	}


	if (pressedKeys[GLFW_KEY_W]) {
		gps::Camera cameraCopy = gps::Camera(myCamera.getCameraPositionAll(), myCamera.getCameraTarget());
		cameraCopy.move(gps::MOVE_FORWARD, cameraSpeed);
		//if(canMove(cameraCopy) == 1) 
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//else { cout << "CAN't move"; }
		//cout << "camera position: " << myCamera.getCameraPositionAll().x << " " << myCamera.getCameraPositionAll().y << " " << myCamera.getCameraPositionAll().z << "\n";
		
	}

	if (pressedKeys[GLFW_KEY_S]) {
		 myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		 myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		 myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_R]) {
		 myCamera.moveUD(0, cameraSpeed * 8);
	}

	if (pressedKeys[GLFW_KEY_F]) {
		 myCamera.moveUD(1, cameraSpeed * 8);
	}

	if (pressedKeys[GLFW_KEY_M]) {
		enablecameraanimation = 1;
		pressedAnimButton = glfwGetTime();
	}

	if (pressedKeys[GLFW_KEY_P]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_O]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_B]) {
		enableanimationbb8 = (enableanimationbb8 == 0) ? 1 : 0;
		//enableanimationbb8 = 1;
	}

	// move dir light
	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle -= 0.3f;
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	// directional light - on/off
	if (pressedKeys[GLFW_KEY_K]) {
		if (dirLightOn == 1) {
			dirLightOn = 0;
			lightColor = glm::vec3(0.0f, 0.0f, 0.0f); //black light
		}
		else {
			dirLightOn = 1;
			lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
		}
		myCustomShader.useShaderProgram();
		lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
		
	}

	// point light - change intensity
	if (pressedKeys[GLFW_KEY_J]) {
		if (intensityPoint == 1) // go down
			dirIntensity = 0;
		else if (intensityPoint < 0) // go up
			dirIntensity = 1;

		if (dirIntensity == 0) {
			intensityPoint -= 0.1f;
		}
		else {
			intensityPoint += 0.1f;
		}
		myCustomShader.useShaderProgram();
		intensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "intensityPoint");
		glUniform1f(intensityLoc, intensityPoint);
	}

	// directional light - on/off
	if (pressedKeys[GLFW_KEY_H]) {
		if (spotLightOn == 1) {
			spotLightOn = 0;
			lightColor3 = glm::vec3(0.0f, 0.0f, 0.0f); //black light
		}
		else {
			spotLightOn = 1;
			lightColor3 = glm::vec3(0.0f, 1.0f, 0.3f); // blue light
		}
		myCustomShader.useShaderProgram();
		lightColorLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor3");
		glUniform3fv(lightColorLoc3, 1, glm::value_ptr(lightColor3));

	}

	if (pressedKeys[GLFW_KEY_I]) {
		polygonal = (polygonal == 1) ? 0 : 1;
		myCustomShader.useShaderProgram();
		polygonalLoc = glGetUniformLocation(myCustomShader.shaderProgram, "polygonal");
		glUniform1i(polygonalLoc, polygonal);
	}



}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	/*const GLfloat near_plane = 1.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(myCamera.getCameraTarget() + 1.0f * lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));
*/
	const GLfloat near_plane = 0.1f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-70.0f, 70.0f, -70.0f, 70.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(myCamera.getCameraTarget() + 1.0f * lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{
	ground = gps::Model3D("objects/ground/ground.obj", "objects/ground/");
	lightCube = gps::Model3D("objects/cube/sphere.obj", "objects/cube/");
	aircraft1 = gps::Model3D("objects/bb8/Shuttle_obj.obj", "objects/bb8/");
	aircraft = gps::Model3D("objects/aircraft/aircraft.obj", "objects/aircraft/");
	r2d2 = gps::Model3D("objects/r2d2/r2.obj", "objects/r2d2/");
	c3po = gps::Model3D("objects/c3po/C3PO.obj", "objects/C3PO/");
	millenium_falcon = gps::Model3D("objects/milleniumfalcon/millenium-falcon.obj",
		"objects/milleniumfalcon/");
	sandtrooper = gps::Model3D("objects/nanosuit/nanosuit.obj",
		"objects/nanosuit/");
	corvette = gps::Model3D("objects/corvette/corvette.obj", "objects/corvette/");
	death_star = gps::Model3D("objects/death_star/death_star.obj", "objects/death_star/");
	floating_city = gps::Model3D("objects/city2/floating_city.obj", "objects/city2/");
	sphere = gps::Model3D("objects/sphere/sphere.obj", "objects/sphere/");
	sunglyder = gps::Model3D("objects/sunglyder/untitled.obj", "objects/sunglyder/");
	bb8 = gps::Model3D("objects/bb82/bb8.obj", "objects/bb82/");
	bb8body = gps::Model3D("objects/bb82/bb8body.obj", "objects/bb82/");
	bb8head = gps::Model3D("objects/bb82/bb8head.obj", "objects/bb82/");
	robot = gps::Model3D("objects/robot/Robot.obj", "objects/robot/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/objectShader.vert", "shaders/objectShader.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");

	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction - directional light
	lightDir = glm::vec3(-2.0f, 8.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set the light position - point light
	lightDir2 = glm::vec3(-2.0f, 1.0f, 2.0f);
	lightDirLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir2");
	glUniform3fv(lightDirLoc2, 1, glm::value_ptr(lightDir2));
	intensityPoint = 1;
	myCustomShader.useShaderProgram();
	intensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "intensityPoint");
	glUniform1f(intensityLoc, intensityPoint);

	// set the light position - spotlight
	lightDir3 = myCamera.getCameraDirection();
	lightDirLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir3");
	glUniform3fv(lightDirLoc3, 1, glm::value_ptr(lightDir3));
	lightPos3 = myCamera.getCameraPositionAll() + glm::vec3(5.0f, 0.0f, 0.0f);
	lightPosLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos3");
	glUniform3fv(lightPosLoc3, 1, glm::value_ptr(lightPos3));
	cutOff = glm::cos(glm::radians(12.5f));
	cutOffLoc = glGetUniformLocation(myCustomShader.shaderProgram, "cutOff");
	glUniform1f(cutOffLoc, cutOff);
	outerCutOff = glm::cos(glm::radians(15.0f));
	outerCutOffLoc = glGetUniformLocation(myCustomShader.shaderProgram, "outerCutOff");
	glUniform1f(outerCutOffLoc, outerCutOff);

	//set light color - directional
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//set light color - point
	lightColor2 = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor2");
	glUniform3fv(lightColorLoc2, 1, glm::value_ptr(lightColor2));

	//set light color - spotlight
	lightColor3 = glm::vec3(0.0f, 0.0f, 0.0f); //black light
	lightColorLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor3");
	glUniform3fv(lightColorLoc3, 1, glm::value_ptr(lightColor3));

	// set polygonal mode
	polygonal = 0;
	polygonalLoc = glGetUniformLocation(myCustomShader.shaderProgram, "polygonal");
	glUniform1i(polygonalLoc, polygonal);

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	// -----------------------------------------------------------------------------

	//-------------------------------------------------------------------------------
	
	// CREATE MODEL MATRIX FOR AIRCRAFT1
	model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(7.0f, -0.9f, 10.0f));
	model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	aircraft1.Draw(depthMapShader);
	//aircraft1 2
	model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(7.0f, -0.9f, 0.0f));
	model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	aircraft1.Draw(depthMapShader);
	//aircraft1 3
	model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(3.0f, -0.9f, -10.0f));
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	aircraft1.Draw(depthMapShader);
	//-------------------------------------------------------------------------------

	// CREATE MODEL MATRIX FOR AIRCRAFT2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, 5.0f, -10.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	aircraft.Draw(depthMapShader);
	//-------------------------------------------------------------------------------


	// CREATE MODEL MATRIX FOR r2d2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -0.1f, -5.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(0, 1, 0));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	r2d2.Draw(depthMapShader);
	//-------------------------------------------------------------------------------

	// CREATE MODEL MATRIX FOR c3po
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -1.0f, -5.0f));
	model = glm::rotate(model, glm::radians(70.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	c3po.Draw(depthMapShader);
	//-------------------------------------------------------------------------------


	// CREATE MODEL MATRIX FOR millenium falcon
	model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 80.0f, -250.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	millenium_falcon.Draw(depthMapShader);
	//-------------------------------------------------------------------------------


	// CREATE MODEL MATRIX FOR sandtrooper
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, 0.0f, -22.0f));
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0, 1, 0));
	//model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	sandtrooper.Draw(depthMapShader);
	// SANDTROOPER2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -22.5f));
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0, 1, 0));
	//model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	sandtrooper.Draw(depthMapShader);
	//-------------------------------------------------------------------------------

	// CREATE MODEL MATRIX FOR corvette
	model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, -2.5f, -20.0f));
	model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	corvette.Draw(depthMapShader);
	//corvette2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, -2.0f, -12.0f));
	model = glm::rotate(model, glm::radians(-10.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	corvette.Draw(depthMapShader);
	//-------------------------------------------------------------------------------

	// CREATE MODEL MATRIX FOR death star
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-64.0f, 40.0f, -80.0f));
	model = glm::scale(model, glm::vec3(0.0001f, 0.0001f, 0.0001f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	death_star.Draw(depthMapShader);
	//-------------------------------------------------------------------------------

	// CREATE MODEL MATRIX FOR floating city
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.8f, 0.0f));
	model = glm::rotate(model, glm::radians(-80.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	floating_city.Draw(depthMapShader);
	//-------------------------------------------------------------------------------

	// CREATE MODEL MATRIX FOR sphere
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, delta2 + 1, 0));
	model = glm::translate(model, glm::vec3(17.0f, 1.0f, -12.0f));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	sphere.Draw(depthMapShader);
	// sphere 2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, delta2 + 1, 0));
	model = glm::translate(model, glm::vec3(27.0f, -1.0f, -8.0f));
	model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	sphere.Draw(depthMapShader);
	// sphere 3
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, delta2, 0));
	model = glm::translate(model, glm::vec3(20.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	sphere.Draw(depthMapShader);
	//-------------------------------------------------------------------------------

	// CREATE MODEL MATRIX FOR sunglyder
	//model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-delta, 0, 0));
	model = glm::translate(model, glm::vec3(10.0f, 3.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	sunglyder.Draw(depthMapShader);
	//-------------------------------------------------------------------------------

	// CREATE MODEL MATRIX FOR bb8 body
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, delta3));
	model = glm::translate(model, glm::vec3(4.0f, -0.1f, -10.0f));
	model = glm::rotate(model, glm::radians(angle3), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	bb8body.Draw(depthMapShader);
	//-------------------------------------------------------------------------------

	// CREATE MODEL MATRIX FOR bb8 head
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, delta3));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	model = glm::translate(model, glm::vec3(7.9f, -1.7f, -20.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	bb8head.Draw(depthMapShader);
	//-------------------------------------------------------------------------------


	//CREATE MODEL MATRIX FOR GROUND
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
		1,
		GL_FALSE,
		glm::value_ptr(model));
	ground.Draw(depthMapShader);
	//--------------------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//render the scene (second pass)

	myCustomShader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
	
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR AIRCRAFT1
	model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(7.0f, -0.9f, 10.0f));
	model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	aircraft1.Draw(myCustomShader);
	if (computedBB == 0) {
		models[0] = glm::mat4(model);
	}
	/////////////////////////// aircraft1 2
	model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(7.0f, -0.9f, 0.0f));
	model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	aircraft1.Draw(myCustomShader);
	if (computedBB == 0) {
		models[1] = glm::mat4(model);
		//cout << "model matrix aircraft:" << glm::to_string(model) << "\n";
	}
	//////////////////////////// aircraft1 3
	model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(3.0f, -0.9f, -10.0f));
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	aircraft1.Draw(myCustomShader);
	if (computedBB == 0) {
		models[2] = glm::mat4(model);
	}
	//-------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR AIRCRAFT2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, 5.0f, -10.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	aircraft.Draw(myCustomShader);
	//computeBoundingBox(aircraft, model);
	//-----------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR R2D2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f,-0.1f, -5.0f));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	r2d2.Draw(myCustomShader);
	if (computedBB == 0) {
		models[3] = glm::mat4(model);
		//cout << "model matrix r2d2:" << glm::to_string(model) << "\n";
	}
	//computeBoundingBox(r2d2, model);
	//------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR C3PO
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -1.0f, -5.0f));
	model = glm::rotate(model, glm::radians(70.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	c3po.Draw(myCustomShader);
	if (computedBB == 0) {
		models[4] = glm::mat4(model);
	}
	//------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR MILLENIUM FALCON
	model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 80.0f, -250.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	millenium_falcon.Draw(myCustomShader);
	//computeBoundingBox(millenium_falcon, model);
	//------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR SANDTROOPER
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, 0.0f, -22.0f));
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	sandtrooper.Draw(myCustomShader);
	//computeBoundingBox(sandtrooper, model);

	// SANDTROOPER2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -22.5f));
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	sandtrooper.Draw(myCustomShader);
	//computeBoundingBox(sandtrooper, model);
	//------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR CORVETTE
	model = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, -2.5f, -20.0f));
	model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	corvette.Draw(myCustomShader);
	//computeBoundingBox(corvette, model);

	//corvette2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, -2.0f, -12.0f));
	model = glm::rotate(model, glm::radians(-10.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	corvette.Draw(myCustomShader);
	//computeBoundingBox(corvette, model);
	//------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR DEATH STAR
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-64.0f, 40.0f, -80.0f));
	model = glm::scale(model, glm::vec3(0.0001f, 0.0001f, 0.0001f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	death_star.Draw(myCustomShader);
	//computeBoundingBox(death_star, model);
	//------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR FLOATING CITY
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.8f, 0.0f));
	model = glm::rotate(model, glm::radians(-80.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	floating_city.Draw(myCustomShader);
	//computeBoundingBox(floating_city, model);
	//------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR GROUND
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
	//send model matrix data to shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	ground.Draw(myCustomShader);
	if (computedBB == 0) {
		//models[5] = glm::mat4(model);
	}
	//-----------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR sphere
	double currentTimeStamp = glfwGetTime();
	updateDelta2(currentTimeStamp - lastTimeStamp);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, delta2+1, 0));
	model = glm::translate(model, glm::vec3(17.0f, 1.0f, -12.0f));
	model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	sphere.Draw(myCustomShader);
	//computeBoundingBox(sphere, model);

	// sphere 2
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, delta2 + 1, 0));
	model = glm::translate(model, glm::vec3(27.0f, -1.0f, -8.0f));
	model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	sphere.Draw(myCustomShader);
	//computeBoundingBox(sphere, model);

	// sphere 3
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, delta2, 0));
	model = glm::translate(model, glm::vec3(20.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	sphere.Draw(myCustomShader);
	//computeBoundingBox(sphere, model);

	//------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR sunglyder
	//model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	updateDelta(currentTimeStamp - lastTimeStamp);
	//lastTimeStamp = currentTimeStamp;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-delta, 0, 0));
	model = glm::translate(model, glm::vec3(10.0f, 3.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	sunglyder.Draw(myCustomShader);
	//computeBoundingBox(sunglyder, model);
	//------------------------------------------------------------------------------

	//CREATE MODEL MATRIX FOR bb8 body
	updateDelta3(currentTimeStamp - lastTimeStamp);
	updateAnglebb8(currentTimeStamp - lastTimeStamp);
	lastTimeStamp = currentTimeStamp;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, delta3));
	model = glm::translate(model, glm::vec3(4.0f, -0.1f, -10.0f));
	model = glm::rotate(model, glm::radians(angle3), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	bb8body.Draw(myCustomShader);
	if (computedBB == 0) {
		//glm::mat4 modelbb = glm::mat4(model);
		models[5] = glm::mat4(model);
		//gps::Model3D object = gps::Model3D("objects/bb82/bb8body.obj", "objects/bb82/");
		//objects.push_back(object);
		//cout << "model matrix bb8 body:" << glm::to_string(model) << "\n";
		//cout << "model matrix bb8 body:" << glm::to_string(modelbb) << "\n";
	}
	//computeBoundingBox(bb8body, model);
	//------------------------------------------------------------------------------


	//CREATE MODEL MATRIX FOR bb8 head
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, delta3));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	model = glm::translate(model, glm::vec3(7.9f, -1.7f, -20.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	bb8head.Draw(myCustomShader);
	if (computedBB == 0) {
		//glm::mat4 modelbb = glm::mat4(model);
		//models.push_back(modelbb);
		models[6] = glm::mat4(model);
		computedBB = 1;
	}
	//computeBoundingBox(bb8head, model);
	//------------------------------------------------------------------------------
		//CREATE MODEL MATRIX FOR robot
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, delta3));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	model = glm::translate(model, glm::vec3(5.0f, -1.7f, -20.0f));
	//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	//robot.Draw(myCustomShader);
	//if (computedBB == 0) {
	//	glm::mat4 modelbb = glm::mat4(model);
	//	models.push_back(modelbb);
	//	computedBB = 1;
	//}
	//computeBoundingBox(bb8head, model);
	//------------------------------------------------------------------------------

	//draw a purple sphere around the point light
	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle2), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, 1.0f * lightDir2);
	model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.024));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	lightCube.Draw(lightShader);
	//-----------------------------------------------------------------------------

	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	mySkyBox.Draw(skyboxShader, view, projection);

	startCameraAnimation(lastTimeStamp - pressedAnimButton);
}


int main(int argc, const char * argv[]) {


	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();
	glCheckError();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::vector<const GLchar*> faces;
	faces.push_back("textures/skybox/purplenebula_rt.tga");
	faces.push_back("textures/skybox/purplenebula_lf.tga");
	faces.push_back("textures/skybox/purplenebula_up.tga");
	faces.push_back("textures/skybox/purplenebula_dn.tga");
	faces.push_back("textures/skybox/purplenebula_bk.tga");
	faces.push_back("textures/skybox/purplenebula_ft.tga");

	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");

	int computedBBmain = 0;
	while (!glfwWindowShouldClose(glWindow)) {
		// in renderScene() adaugam bounding boxurile fiecarui obiect in lista 
		renderScene();

		if (computedBBmain == 0) {
			for (int i = 0; i < 10; i++) {
				models.push_back(glm::mat4(0.0f));
			}
			computeAllBoundingBoxes();
			computedBBmain = 1;
		}
		processMovement();
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
		// golim lista de bboxuri
		//while (!bboxes.empty()) {
		//	bboxes.pop_back();
		//}
		//cout << "bboxes sizeeeeeeeeeeeeeeeeeee" << bboxes.size() << "\n";
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
