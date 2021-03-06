#pragma once

// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Planet.h"
#include "Shaders.h"
#include "OrbitalManipulator.h"
#include "Camera.h"
#include "MVP.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Mesh.h"
#include "DisplacementPhongMaterial.h"
#include "PhongMaterial.h"
#include "SelfIlluminMaterial.h"
#include "CubeMap.h"
#include "HeightMapFilter.h"
#include "Noise.h"
#include "Scene.h"

#include "DefaultRenderer.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#define OCTREE_DEPTH 4
#define NB_CHUNK 5

// Properties
GLuint screenWidth = 1600, screenHeight = 900;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();
void wireframe(DefaultRenderer * renderer);
void stopSelection();

// Camera
//Camera* camera = new Camera((float)screenWidth, (float)screenHeight, 0.01f, 100000.f, 0.90f);
Camera* camera = Scene::getInstance()->getCamera();

bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool stopSelect = false;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

unsigned int circleSpeed = 1;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "VArch - Octree", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Camera ==========================================
	camera->transform.setPosition(glm::vec3(16.3f, 10.7f, -6.5f));
	camera->transform.setRotation(glm::vec3(-40.f, 124.f, 0.f));

	OrbitalManipulator* orbitalManipulator = new OrbitalManipulator(camera);
	//camera->setManipulator(orbitalManipulator);
	camera->setSensitivity(.5f);
	camera->setSpeed(100);

	// Scene ==========================================

	Scene* scene = Scene::getInstance();//Scene(root, camera);

	// Set material ==========================================
	PhongMaterial *mat = new PhongMaterial();
	mat->ambient = glm::vec3(1);
	mat->diffuse = glm::vec3(1);
	mat->specular = glm::vec3(1);
	mat->shininess = 32;

	SelfIlluminMaterial *SIMat = new SelfIlluminMaterial();

	// HeightMap ==========================================
	NoiseProperties np = NoiseProperties(NB_CHUNK * 0.3, 0.5, 4);
	CubeMap cubeMap(pow(2, OCTREE_DEPTH) * NB_CHUNK * 2);
	cubeMap.generateSimplex(np);
	//cubeMap.transformInterval(-0.5f, 0.5f);

	// Light ==========================================
	DirectionalLight* light = new DirectionalLight;
	light->direction = glm::vec3(-1, -1, -1);
	light->diffuse = glm::vec3(.5f);
	//light->transform.translate(glm::vec3(0, 2, 0));

	PointLight* pointLight = new PointLight;
	pointLight->position = glm::vec3(20, 20, 20);
	pointLight->diffuse = glm::vec3(1, .5f, .5f);

	PointLight* pointLight2 = new PointLight;
	pointLight2->position = glm::vec3(20, -20, 20);
	pointLight2->diffuse = glm::vec3(0, 0.5, 1);

	scene->addLight(light);
	//scene.addLight(pointLight);
	//scene.addLight(pointLight2);

	// LandMark ==========================================
	Mesh* refCube = new Mesh(mat);
	Mesh* landMarkMesh = new Mesh(SIMat);
	landMarkMesh->generateLandmark();

	Object *landMark = new Object();
	landMark->addComponent(landMarkMesh);

	//landMark->transform.translate(glm::vec3(-1, -1, -1));

	//root->addChild(landMark);

	// Planet ==========================================
	ColorTable colorTable = ColorTable::Nature(128);
	Planet planet(cubeMap, colorTable, OCTREE_DEPTH, NB_CHUNK);

	
	planet.addMeshesToObject(scene->getRoot());

	// Diametre equivalent to earth : 3678

	// Cubes
	Mesh* cubeMesh = new Mesh();
	cubeMesh->generateCube(2, glm::vec3(0), glm::vec3(0.8));
	Object* cube = new Object();
	cube->addComponent(cubeMesh);
	cube->transform.setScale(glm::vec3(20, 0.1f, 20));
	cube->transform.translate(glm::vec3(0, 0, 0));

	scene->addObject(cube);

	cubeMesh = new Mesh();
	cubeMesh->generateCube(2, glm::vec3(0), glm::vec3(1, 0, 0));
	cube = new Object();
	cube->addComponent(cubeMesh);
	cube->transform.translate(glm::vec3(5, 1, 5));

	scene->addObject(cube);

	cubeMesh = new Mesh();
	cubeMesh->generateCube(2, glm::vec3(0), glm::vec3(0, 0, 1));
	cube = new Object();
	cube->addComponent(cubeMesh);
	cube->transform.translate(glm::vec3(5, 2, -5));

	scene->addObject(cube);

	// Renderer ==========================================

	DefaultRenderer* renderer = DefaultRenderer::getInstance();// (&scene);
	renderer->setWireframe(false);

	//Shaders::getInstance()->useShader(BuiltInShader::PHONG);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();
		wireframe(renderer);
		stopSelection();

		light->direction = glm::normalize(camera->front() + glm::vec3(0.5, 0, 0));

		renderer->render();

		// Swap the buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose

	glfwTerminate();
	return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	float speed = 1.f;

	// Camera controls
	if (keys[GLFW_KEY_W])
		camera->processMove(CameraMovement::FORWARD, deltaTime / speed);
	if (keys[GLFW_KEY_S])
		camera->processMove(CameraMovement::BACKWARD, deltaTime / speed);
	if (keys[GLFW_KEY_A])
		camera->processMove(CameraMovement::LEFTWARD, deltaTime / speed);
	if (keys[GLFW_KEY_D])
		camera->processMove(CameraMovement::RIGHTWARD, deltaTime / speed);

}

void stopSelection()
{
	// Camera controls
	if (keys[GLFW_KEY_H])
		stopSelect = false;
	if (keys[GLFW_KEY_G])
		stopSelect = true;
}

void wireframe(DefaultRenderer * renderer)
{
	// Camera controls
	if (keys[GLFW_KEY_F])
		renderer->setWireframe(false);
	if (keys[GLFW_KEY_Z])
		renderer->setWireframe(true);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera->processLook(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->processZoom(yoffset / 100.f);
}