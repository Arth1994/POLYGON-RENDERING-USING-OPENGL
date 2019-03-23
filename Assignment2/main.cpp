// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include<stdio.h>
#include<string.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


// Other includes
#include "nanogui/nanogui.h"
#include "../Assignment2/View.h"
#include "../Assignment2/CameraOrientation.h"
#include "../Assignment2/ShaderHelper.h"


/*****************

Assignment 2 - Computer Graphics CS 6366, Code Wriiten By Arth Shah axs175430

Sources referred - OpenGL Tutorials, Boiler Plate Code in Assignment 0, Tom Dalling blog's for Ambient Light, Directional Light
and Spot Lighting. Official Site for GLM Mathematics and boiler plate code given in Assignment 1.

The Code Works as per specifications mentioned in the assignment document and satisfies all the requirements.

The Description of the code written is as follows:- 3 header files for camera position/orientation, ShaderHelper.h(For establishing basic.vrt and frag.vrt)

ShaderHelper.h was Shader.cpp in assignment 0.

int Main() has nano gui bar designed to the required specifications.

void Draw() :- mainly contains the output glDrawPloygonMode() && glBindVertexArray() method and rendering in the form of solids points and traingles.

void loadObjectToDrawsInScene() :- Part of this method is taken from Code in Assignment 0 where we have glBufferData(GL_ARRAY_BUFFER, renderer.vao_vertices.size() * sizeof(Renderer::Vertex), &renderer.vao_vertices[0], GL_STATIC_DRAW);
where points are loaded in the scene.

void illuminateScene() - mainly to load camera position and for light amplification.

void rotatePointX() - A method to rotate the point light about the X - axis.

void rotatePointY() - A method to rotate the point light about the Y - axis

void rotatePointZ() - A method to rotate the point light about the Z-axis.

There is also a class Render to hold the structs and intializations for point light and direction light

**********************/



using namespace nanogui;

enum RenderType {
	solid,
	points,
	lines
};
enum Culling {
	CW,
	CCW,
};

enum ShaderValue {
	SMOOTH = 0,
	FLAT = 1,
};

enum DepthValue {
	LESS = 0,
	ALWAYS = 1,
};

class Render {

public:

	struct RenderPoint {
		bool status;
		glm::vec3 position;
		float constant;
		float linear;
		float quadratic;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;

	};

	struct RenderDirection {
		bool status;
		glm::vec3 direction;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

public:

	RenderPoint renderPoint;

	RenderDirection renderDirection;

	void initialize()
	{

		renderDirection.status = true;
		renderDirection.direction = glm::vec3(0.0f, -1.0f, -1.0f);
		renderDirection.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
		renderDirection.diffuse = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
		renderDirection.specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		renderPoint.status = true;
		renderPoint.position = glm::vec3(1.2f, 1.0f, 2.0f);
		renderPoint.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		renderPoint.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		renderPoint.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		renderPoint.constant = 1.0f;
		renderPoint.linear = 0.09f;
		renderPoint.quadratic = 0.032f;

	};
};

GLFWwindow* window;
Screen* screen = nullptr;
float degree = 90.0f;
CameraOrientation* cameraOrientation = new CameraOrientation();
bool values[256];
std::vector<Renderer> objectToDrawArray;


std::string modelName = "./models/cyborg.obj";
ShaderValue shaderValue = SMOOTH;
DepthValue  depthValue = LESS;
bool status = false;
RenderType renderType = lines;
Culling culling = CW;
bool reset = true;
GLfloat timeSlice = 0.0f;
GLfloat endFrame = 0.0f;
glm::vec4 backgroundColor = glm::vec4(0.1, 0.1, 0.1, 0.1);
bool enabled = true;
float X = 0, Y = 0;
double pie = 3.14159;
float floatPie = (float)pie;
bool dirLightStatus = true;
bool pointLightStatus = false;
GLfloat shininess = 32.0f;
glm::vec3 pointLightPosition = cameraOrientation->originalPos;

glm::vec3 centerOfObject;
glm::vec3 dynamicPosition;
float tempAngle = glm::radians(10.f);


Color colorValue(0.5f, 0.15f, 0.15f, 1.f);
Color colorValueAmbient(0.5f, 0.15f, 0.05f, 1.f);
Color colorValueDiffuse(0.5f, 0.15f, 0.25f, 1.f);
Color colorValueSpecular(0.5f, 0.25f, 0.15f, 1.f);
Color colorValuePointAmbient(0.5f, 0.25f, 0.15f, 1.f);
Color colorValuePointDiffuse(0.5f, 0.15f, 0.15f, 1.f);
Color colorValuePointSpecular(0.5f, 0.05f, 0.15f, 1.f);

bool rotateOnX = false, rotateOnY = false, rotateOnZ = false;

Render* render = new Render();


void rotatePointX() {
	printf("IN X\n");
	pointLightPosition = pointLightPosition - centerOfObject;
	printf("%f %f %f", pointLightPosition[0], pointLightPosition[1], pointLightPosition[2]);
	dynamicPosition[0] = pointLightPosition[0];
	dynamicPosition[1] = pointLightPosition[1] * cos(tempAngle) - pointLightPosition[2] * sin(tempAngle);
	dynamicPosition[2] = pointLightPosition[1] * sin(tempAngle) + pointLightPosition[2] * cos(tempAngle);
	pointLightPosition = dynamicPosition;
	pointLightPosition = dynamicPosition + centerOfObject;
}

void rotatePointY() {
	printf("IN Y\n");
	pointLightPosition = pointLightPosition - centerOfObject;
	printf("%f %f %f", pointLightPosition[0], pointLightPosition[1], pointLightPosition[2]);
	dynamicPosition[0] = pointLightPosition[0] * cos(tempAngle) - pointLightPosition[2] * sin(tempAngle);
	dynamicPosition[1] = pointLightPosition[1];
	dynamicPosition[2] = pointLightPosition[0] * sin(tempAngle) + pointLightPosition[2] * cos(tempAngle);
	pointLightPosition = dynamicPosition;
	pointLightPosition = dynamicPosition + centerOfObject;
}

void rotatePointZ() {
	printf("IN Z\n");
	pointLightPosition = pointLightPosition - centerOfObject;
	printf("%f %f %f", pointLightPosition[0], pointLightPosition[1], pointLightPosition[2]);
	dynamicPosition[0] = pointLightPosition[0] * cos(tempAngle) - pointLightPosition[1] * sin(tempAngle);
	dynamicPosition[1] = pointLightPosition[0] * sin(tempAngle) + pointLightPosition[1] * cos(tempAngle);
	dynamicPosition[2] = pointLightPosition[2];
	pointLightPosition = dynamicPosition;
	pointLightPosition = dynamicPosition + centerOfObject;
}


void draw(Shader& shader, Renderer& objectToDraw)
{
	glBindVertexArray(objectToDraw.vao);

	glUniform3f(glGetUniformLocation(shader.program, "m_object.object_color"), objectToDraw.objectColor[0], objectToDraw.objectColor[1], objectToDraw.objectColor[2]);
	glUniform1f(glGetUniformLocation(shader.program, "m_object.shininess"), shininess);
	glClear(GL_COLOR_BUFFER_BIT);
	if (renderType == solid) {
		glPolygonMode(GL_FRONT_AND_BACK, GLU_VERTEX);
		glDrawArrays(GL_TRIANGLES, 0, objectToDraw.vao_vertices.size());
	}
	else if (renderType == points) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_POINTS, 0, objectToDraw.vao_vertices.size());
	}
	else if (renderType == lines) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, objectToDraw.vao_vertices.size());
	}
	glBindVertexArray(0);
}


void loadObjectToDrawsInScene(std::string modelName) {
	objectToDrawArray.clear();
	Renderer renderer(modelName);
	renderer.objectColor = glm::vec4(colorValue.r(), colorValue.g(), colorValue.b(), colorValue.w());
	renderer.objectName = "mainObjectToDraw";
	centerOfObject = renderer.objectCenter;
	glGenVertexArrays(1, &renderer.vao);
	glGenBuffers(1, &renderer.vbo);
	glBindVertexArray(renderer.vao);
	glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
	glBufferData(GL_ARRAY_BUFFER, renderer.vao_vertices.size() * sizeof(Renderer::Vertex), &renderer.vao_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Renderer::Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Renderer::Vertex), (GLvoid*)offsetof(Renderer::Vertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer::Vertex), (GLvoid*)offsetof(Renderer::Vertex, TexCoords));
	glBindVertexArray(0);
	objectToDrawArray.push_back(renderer);
}


void drawEntireScene(Shader& shader)
{
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);

	glEnable(GL_DEPTH_TEST);
	if (depthValue == 0) {
		glDepthFunc(GL_LESS);
	}
	else if (depthValue == 1) {
		glDepthFunc(GL_ALWAYS);
	}


	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	if (culling == CW) {
		glFrontFace(GL_CW);
	}
	else if (culling == CCW)
	{
		glFrontFace(GL_CCW);
	}

	int i = 0;

	while (i < objectToDrawArray.size())
	{
		if (objectToDrawArray[i].objectName == "mainObjectToDraw")
		{
			glm::mat4 mainObjectToDraw_model_mat = glm::mat4();
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(mainObjectToDraw_model_mat));
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(cameraOrientation->loadCameraViewMatrix()));
			objectToDrawArray[i].objectColor = glm::vec4(colorValue[0], colorValue[1], colorValue[2], colorValue[3]);
			printf("%f, %f, %f, %f \n\n", colorValue[0], colorValue[1], colorValue[2], colorValue[3]);
			draw(shader, objectToDrawArray[i]);
		}
		else {
			std::cout << "The Entry is not valid" << std::endl;
		}
		i++;
	}
}


void illuminateScene(Shader& shader, bool pointLightStatus, bool directionLightStatus)
{

	glUniform3f(glGetUniformLocation(shader.program, "camera_pos"), cameraOrientation->pos.x, cameraOrientation->pos.y, cameraOrientation->pos.z);
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(cameraOrientation->loadCameraProjectionMatrix()));


	if (directionLightStatus) {
		glUniform1i(glGetUniformLocation(shader.program, "dir_light.status"), directionLightStatus);
		glUniform3f(glGetUniformLocation(shader.program, "dir_light.direction"), render->renderDirection.direction[0], render->renderDirection.direction[1], render->renderDirection.direction[2]);
		glUniform3f(glGetUniformLocation(shader.program, "dir_light.ambient"), colorValueAmbient[0], colorValueAmbient[1], colorValueAmbient[2]);
		glUniform3f(glGetUniformLocation(shader.program, "dir_light.diffuse"), colorValueDiffuse[0], colorValueDiffuse[1], colorValueDiffuse[2]);
		glUniform3f(glGetUniformLocation(shader.program, "dir_light.specular"), colorValueSpecular[0], colorValueSpecular[1], colorValueSpecular[2]);
	}


	else {
		glUniform1i(glGetUniformLocation(shader.program, "dir_light.status"), directionLightStatus);
		glUniform3f(glGetUniformLocation(shader.program, "dir_light.direction"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader.program, "dir_light.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader.program, "dir_light.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader.program, "dir_light.specular"), 0.0f, 0.0f, 0.0f);

	}

	if (pointLightStatus) {

		glUniform1i(glGetUniformLocation(shader.program, "point_light.status"), pointLightStatus);
		glUniform3f(glGetUniformLocation(shader.program, "point_light.position"), pointLightPosition[0], pointLightPosition[1], pointLightPosition[2]);
		glUniform3f(glGetUniformLocation(shader.program, "point_light.ambient"), colorValuePointAmbient[0], colorValuePointAmbient[1], colorValuePointAmbient[2]);
		glUniform3f(glGetUniformLocation(shader.program, "point_light.diffuse"), colorValuePointDiffuse[0], colorValuePointDiffuse[1], colorValuePointDiffuse[2]);
		glUniform3f(glGetUniformLocation(shader.program, "point_light.specular"), colorValuePointSpecular[0], colorValuePointSpecular[1], colorValuePointSpecular[2]);
		glUniform1f(glGetUniformLocation(shader.program, "point_light.constant"), render->renderPoint.constant);
		glUniform1f(glGetUniformLocation(shader.program, "point_light.linear"), render->renderPoint.linear);
		glUniform1f(glGetUniformLocation(shader.program, "point_light.quadratic"), render->renderPoint.quadratic);
	}

	else {
		glUniform1i(glGetUniformLocation(shader.program, "point_light.status"), pointLightStatus);
		glUniform3f(glGetUniformLocation(shader.program, "point_light.position"), pointLightPosition[0], pointLightPosition[1], pointLightPosition[2]);
		glUniform3f(glGetUniformLocation(shader.program, "point_light.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader.program, "point_light.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader.program, "point_light.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(shader.program, "point_light.constant"), render->renderPoint.constant);
		glUniform1f(glGetUniformLocation(shader.program, "point_light.linear"), render->renderPoint.linear);
		glUniform1f(glGetUniformLocation(shader.program, "point_light.quadratic"), render->renderPoint.quadratic);
	}


}

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	cameraOrientation->init();
	render->initialize();

	window = glfwCreateWindow(cameraOrientation->width, cameraOrientation->height, "ASSIGNMENT1", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	screen = new Screen();
	screen->initialize(window, true);

	int width, height;

	glfwGetFramebufferSize(window, &width, &height);
	int desiredHeight = cameraOrientation->height;
	int desiredWidth = cameraOrientation->width;
	glViewport(0, 0, desiredWidth, desiredHeight);
	glfwSwapInterval(0);
	glfwSwapBuffers(window);

	//Setting Shaders for Flat Shading and Smooth Shading
	Shader sharderCurrent = Shader("./shader/basic.vert", "./shader/basic.frag");
	Shader flat = Shader("./shaderFlat/basic.vert", "./shaderFlat/basic.frag");
	Shader shader = Shader("./shader/basic.vert", "./shader/basic.frag");

	FormHelper *myGui = new FormHelper(screen);
	ref<nanogui::Window> nanoGuiWindow = myGui->addWindow(Eigen::Vector2i(0, 0), "NANO GUI CONTROL BAR 1");


	FormHelper *myGui1 = new FormHelper(screen);
	ref<nanogui::Window> nanoGuiWindow1 = myGui1->addWindow(Eigen::Vector2i(270, 0), "NANO GUI CONTROL BAR 2");


	myGui1->addGroup("LIGHTING");

	myGui1->addVariable("COLOR", colorValue);

	myGui1->addVariable("OBJECT SHININESS", shininess);

	myGui1->addVariable("DIRECTION LIGHT STATUS", dirLightStatus);
	myGui1->addVariable("DIRECTION LIGHT AMBIENT COLOR", colorValueAmbient);
	myGui1->addVariable("DIRECION LIGHT DIFFUSE COLOR", colorValueDiffuse);
	myGui1->addVariable("DIRECTION LIGHT SPECULAR COLOR", colorValueSpecular);

	myGui1->addVariable("POINT LIGHT STATUS", pointLightStatus);
	myGui1->addVariable("POINT LIGHT AMBIENT COLOR", colorValuePointAmbient);
	myGui1->addVariable("POINT LIGHT DIFFUSE COLOR", colorValuePointDiffuse);
	myGui1->addVariable("POINT LIGHT SPECULAR COLOR", colorValuePointSpecular);


	myGui1->addVariable("POINT LIGHT ROTATE ON X", rotateOnX);
	myGui1->addVariable("POINT LIGHT ROTATE ON Y", rotateOnY);
	myGui1->addVariable("POINT LIGHT ROTATE ON Z", rotateOnZ);


	myGui->addGroup("POSITION");
	static auto X = myGui->addVariable("X", cameraOrientation->pos.x);
	X->setSpinnable(true);
	static auto Y = myGui->addVariable("Y", cameraOrientation->pos.y);
	Y->setSpinnable(true);
	static auto Z = myGui->addVariable("Z", cameraOrientation->pos.z);
	Z->setSpinnable(true);

	myGui->addGroup("ROTATE");
	myGui->addVariable("ROTATE VALUE", degree)->setSpinnable(true);

	myGui->addButton("ROTATE RIGHT+", []()
	{
		cameraOrientation->rotX(glm::radians(degree));
	});
	myGui->addButton("ROTATE RIGHT-", []()
	{
		cameraOrientation->rotX(-glm::radians(degree));
	});
	myGui->addButton("ROTATE UP+", []()
	{
		cameraOrientation->rotY(glm::radians(degree));
	});
	myGui->addButton("ROTATE UP-", []()
	{
		cameraOrientation->rotY(-glm::radians(degree));
	});
	myGui->addButton("ROTATE FRONT+", []()
	{
		cameraOrientation->rotZ(glm::radians(degree));
	});
	myGui->addButton("ROTATE FRONT-", []()
	{
		cameraOrientation->rotZ(-glm::radians(degree));
	});

	myGui->addGroup("CONFIGURATION");
	myGui->addVariable("Z NEAR", cameraOrientation->zoomIn)->setSpinnable(true);
	myGui->addVariable("Z FAR", cameraOrientation->zoomOut)->setSpinnable(true);
	myGui->addVariable("RENDER TYPE", renderType, enabled)->setItems({ "SOLID", "POINTS", "LINES" });
	myGui->addVariable("CULLING TYPE", culling, enabled)->setItems({ "CW", "CCW" });
	myGui->addVariable("SHADING", shaderValue, true)->setItems({ "SMOOTH", "FLAT" });
	myGui->addVariable("Depth", depthValue, true)->setItems({ "LESS", "ALWAYS" });
	myGui->addVariable("MODEL NAME", modelName);


	myGui1->addButton("RESET POINT LIGHT", []() {
		loadObjectToDrawsInScene(modelName);
		pointLightPosition = cameraOrientation->originalPos;
	});

	myGui->addButton("RELOAD MODEL", []()
	{
		loadObjectToDrawsInScene(modelName);
		cameraOrientation->reset();
	});
	myGui->addButton("RESET", []()
	{
		cameraOrientation->reset();
	});

	screen->setVisible(true);
	screen->performLayout();

	glfwSetCursorPosCallback(window,
		[](GLFWwindow *window, double x, double y) {
		screen->cursorPosCallbackEvent(x, y);
	}
	);

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow *, int button, int action, int modifiers) {
		screen->mouseButtonCallbackEvent(button, action, modifiers);
		X->setValue(cameraOrientation->pos[0]);
		Y->setValue(cameraOrientation->pos[1]);
		Z->setValue(cameraOrientation->pos[2]);

	}
	);

	glfwSetKeyCallback(window,
		[](GLFWwindow *window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if (key >= 0 && key < 256)
		{
			if (action == GLFW_PRESS)
			{
				values[key] = true;

			}
			else if (action == GLFW_RELEASE)
				values[key] = false;
		}
		X->setValue(cameraOrientation->pos[0]);
		Y->setValue(cameraOrientation->pos[1]);
		Z->setValue(cameraOrientation->pos[2]);
	}
	);

	glfwSetCharCallback(window,
		[](GLFWwindow *, unsigned int codepoint) {
		screen->charCallbackEvent(codepoint);
	}
	);

	glfwSetDropCallback(window,
		[](GLFWwindow *, int count, const char **filenames) {
		screen->dropCallbackEvent(count, filenames);
	}
	);

	glfwSetScrollCallback(window,
		[](GLFWwindow *, double x, double y) {
		screen->scrollCallbackEvent(x, y);

	}
	);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow *, int width, int height) {
		screen->resizeCallbackEvent(width, height);
	}
	);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		loadObjectToDrawsInScene(modelName);
		if (shaderValue == 0) {
			shader = sharderCurrent;
		}
		else {
			shader = flat;
		}
		shader.use();
		illuminateScene(shader, pointLightStatus, dirLightStatus);
		if (rotateOnX) {
			rotatePointX();
		}
		else if (rotateOnY) {
			rotatePointY();
		}
		else if (rotateOnZ) {
			rotatePointZ();
		}
		drawEntireScene(shader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		screen->drawWidgets();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return EXIT_SUCCESS;
}
