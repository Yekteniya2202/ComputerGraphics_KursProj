#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <assimp/Importer.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "ParticleSystem.h"
#include "Tank.h"
#include "Source.h"
#include "Picker.h"

ModelTransform modelTrans;
Model* backpack;
Shader* selecting_shader, * light_shader, * backpack_shader, * picking_shader, * particle_shader;
bool invokeMousePosition = false;
double xPress, yPress, xRelease, yRelease, xCurrent, yCurrent;
float koef = 1.8611f;
int width = 1280, height = 720;
Picker* picker;
bool renderTankParticle = false;
MissilRejected rejected;
struct Color {
	float r, g, b, a;
};

Color background = { 1.f, 1.f, 1.f, 1.f };

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

Camera camera(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.f, 1.0f, 0.f), 0, -45.0f);

vector<Tank> tanks;


double random(float min, float max)
{
	return (float)(rand()) / RAND_MAX * (max - min) + min;
}


void OnResize(GLFWwindow* win, int width, int height)
{
	glViewport(0, 0, width, height);
	::width = width;
	::height = height;
}



void draw_color_picking() {
	glClearColor(background.r, background.g, background.b, background.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 p = camera.GetProjectionMatrix();
	glm::mat4 v = camera.GetViewMatrix();
	glm::mat4 pv = p * v;
	glm::mat4 model = glm::mat4(1.0f);

	for (auto& kv2 : tanks) {
		kv2.TransInfo(&modelTrans);
		model = glm::mat4(1.0f);
		model = glm::translate(model, modelTrans.position);
		model = glm::rotate(model, glm::radians(modelTrans.rotation.y), glm::vec3(0.f, 1.f, 0.f));
		model = glm::scale(model, modelTrans.scale);
		selecting_shader->use();
		auto pvm = pv * model;
		int i = kv2.GetId();
		int r = (i & 0x000000FF) >> 0;
		int g = (i & 0x0000FF00) >> 8;
		int b = (i & 0x00FF0000) >> 16;
		selecting_shader->setMatrix4F("pvm", pvm);
		selecting_shader->setVec4("PickingColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
		backpack->Draw(selecting_shader);
	}
	glFlush();
	glFinish();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

static void cursor_position_callback(GLFWwindow* win, double xpos, double ypos)
{
	if (invokeMousePosition) {

		glfwGetCursorPos(win, &xCurrent, &yCurrent);

		float x1 = xPress / width * 2 - 1, y1 = -(yPress / height * 2 - 1);
		float x2 = xCurrent / width * 2 - 1, y2 = -(yCurrent / height * 2 - 1);
		float ox1 = x1, ox2 = x2, oy1 = y1, oy2 = y2;
		picker->setPoints(x1, x2, y1, y2);
		picking_shader->use();
		picker->draw();
		glFlush();
		glFinish();
		glfwSwapBuffers(win);

	}

}

void mouse_callback(GLFWwindow* win, int button, int action, int mods)
{

	for (auto& tank : tanks) {
		tank.setNotSelected();
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action) {
			glfwGetCursorPos(win, &xPress, &yPress);
			invokeMousePosition = true;
		}

		else if (GLFW_RELEASE == action) {
			glfwGetCursorPos(win, &xRelease, &yRelease);
			invokeMousePosition = false;

			draw_color_picking();
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);

			int width = fabs(xRelease - xPress);
			int height = fabs(yRelease - yPress);
			double startX = xPress, startY = yPress;
			if (width == 0) width = 1;
			if (height == 0) height = 1;
			if (xRelease < xPress) startX = xRelease;
			if (yRelease > yPress) startY = yRelease;
			unsigned char* data = new unsigned char[width * height * 4];
			glReadPixels(startX, viewport[3] - startY, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);
			for (int i = 0; i < width * height * 4; i += 4) {
				int pickedID = data[i] + data[i + 1] * 256 + data[i + 2] * 256 * 256;
				for (auto& tank : tanks) {
					tank.setSelected(pickedID);
				}
			}

			delete[] data;

			glClearColor(background.r, background.g, background.b, background.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
}


void processInput(GLFWwindow* win, double dt)
{
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, true);
	if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS)
		background = { 1.0f, 0.0f, 0.0f, 1.0f };
	if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS)
		background = { 0.0f, 1.0f, 0.0f, 1.0f };
	if (glfwGetKey(win, GLFW_KEY_3) == GLFW_PRESS)
		background = { 0.0f, 0.0f, 1.0f, 1.0f };
	if (glfwGetKey(win, GLFW_KEY_4) == GLFW_PRESS)
		background = { 0.55f, 0.8f, 0.85f, 1.0f };
	if (glfwGetKey(win, GLFW_KEY_5) == GLFW_PRESS) {
		cout << "Pressed " << xPress << " " << yPress << endl;
		cout << "Released " << xRelease << " " << yRelease << endl;
	}
	if (glfwGetKey(win, GLFW_KEY_6) == GLFW_PRESS) {
		koef += 0.0001;
		cout << "koef " << koef << endl;
	}
	if (glfwGetKey(win, GLFW_KEY_7) == GLFW_PRESS) {
		koef -= 0.0001;
		cout << "koef " << koef << endl;
	}
	if (glfwGetKey(win, GLFW_KEY_Z) == GLFW_PRESS) {
		for (auto& tank : tanks) {
			tank.Rotate(1, dt);
		}
	}
	if (glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS) {
		for (auto& tank : tanks) {
			tank.Rotate(-1, dt);
		}
	}
	if (glfwGetKey(win, GLFW_KEY_C) == GLFW_PRESS) {
		for (auto& tank : tanks) {
			tank.Move(dt);
		}
	}

	if (glfwGetKey(win, GLFW_KEY_V) == GLFW_PRESS) {
		for (auto& tank : tanks) {
			if (tank.getSelected()) {
				rejected.addMissil(tank.shoot());
			}
		}
	}

	if (glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS)
	{
		cout << camera.Position.x << " " << camera.Position.y << " " << camera.Position.z << endl;
		cout << camera.Yaw << " " << camera.Pitch << endl;
	}

	uint32_t dir = 0;

	/*
	if (glfwGetKey(win, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		dir |= CAM_UP;
	if (glfwGetKey(win, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		dir |= CAM_DOWN;
	*/
	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
		dir |= CAM_FORWARD | CAM_UP;
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
		dir |= CAM_BACKWARD | CAM_DOWN;
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
		dir |= CAM_LEFT;
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
		dir |= CAM_RIGHT;

	/*
	double newx = 0.f, newy = 0.f;
	glfwGetCursorPos(win, &newx, &newy);
	static double x = newx, y = newy;
	double xoffset = newx - x;
	double yoffset = newy - y;
	x = newx;
	y = newy;
	*/
	camera.Move(dir, dt);
	//camera.Rotate(xoffset, -yoffset);
}

void OnScroll(GLFWwindow* win, double x, double y)
{
	camera.ChangeFOV(y);
	std::cout << "Scrolled x: " << x << ", y: " << y << ". FOV = " << camera.Fov << std::endl;
}

bool wireframeMode = false;

void UpdatePolygoneMode()
{
	if (wireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void OnKeyAction(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_SPACE:
			wireframeMode = !wireframeMode;
			UpdatePolygoneMode();
			break;
		case GLFW_KEY_X:
			renderTankParticle = true;
		}
	}

	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_X:
			renderTankParticle = false;
		}
	}
}

typedef unsigned char byte;

Light* flashLight, * redLamp, * blueLamp, * sunLight;

int main()
{
#pragma region WINDOW INITIALIZATION
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(width, height, "OpenGL Window", NULL, NULL);
	if (win == NULL)
	{
		std::cout << "Error. Couldn't create window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Error. Couldn't load GLAD!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(win, OnResize);
	glfwSetScrollCallback(win, OnScroll);
	glfwSetKeyCallback(win, OnKeyAction);
	glfwSetMouseButtonCallback(win, mouse_callback);
	glfwSetCursorPosCallback(win, cursor_position_callback);


	glViewport(0, 0, 1280, 720);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	UpdatePolygoneMode();
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CCW);

	backpack = new Model("models/tank/tank.obj", true);
#pragma endregion


#pragma region CUBE INITIALIZATION
	int box_width, box_height, channels;
	byte* data = stbi_load("images\\box.png", &box_width, &box_height, &channels, 0);

	const int verts = 36;

	float cube[] = {
		//position			normal					texture				color			
	-1.0f,-1.0f,-1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	-1.0f,-1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-1.0f,-1.0f,-1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-1.0f, 1.0f,-1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,		0.0f, 1.0f, 0.0f,

	1.0f, 1.0f,-1.0f,	0.0f,  0.0f, -1.0f, 	0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-1.0f,-1.0f,-1.0f,	0.0f,  0.0f, -1.0f, 	1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	-1.0f, 1.0f,-1.0f,	0.0f,  0.0f, -1.0f, 	1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	1.0f, 1.0f,-1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	1.0f,-1.0f,-1.0f,	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	-1.0f,-1.0f,-1.0f,	0.0f,  0.0f, -1.0f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,

	1.0f,-1.0f, 1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,	0.0f, -1.0f,  0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
	1.0f,-1.0f,-1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
	1.0f,-1.0f, 1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,	0.0f, -1.0f,  0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,	0.0f, -1.0f,  0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f,

	-1.0f, 1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	1.0f,-1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
	1.0f,-1.0f, 1.0f,	0.0f,  0.0f, 1.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 1.0f,	1.0f,  0.0f,  0.0f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	1.0f,-1.0f,-1.0f,	1.0f,  0.0f,  0.0f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	1.0f, 1.0f,-1.0f,	1.0f,  0.0f,  0.0f,		1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	1.0f,-1.0f,-1.0f,	1.0f,  0.0f,  0.0f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,	1.0f,  0.0f,  0.0f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	1.0f,-1.0f, 1.0f,	1.0f,  0.0f,  0.0f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,

	1.0f, 1.0f, 1.0f,	0.0f,  1.0f,  0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	1.0f, 1.0f,-1.0f,	0.0f,  1.0f,  0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-1.0f, 1.0f,-1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,	0.0f,  1.0f,  0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	-1.0f, 1.0f,-1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f
	};

	Material cubeMaterials[3] = {
		{
			glm::vec3(0.25, 0.20725, 0.20725),
			glm::vec3(1, 0.829, 0.829),
			glm::vec3(0.296648,	0.296648, 0.296648),
			12.f
		}, // pearl
		{
			glm::vec3(0.25, 0.25, 0.25),
			glm::vec3(0.4, 0.4, 0.4),
			glm::vec3(0.774597,	0.774597, 0.774597),
			77.f
		}, // chrome
		{
			glm::vec3(0.1745, 0.01175, 0.01175),
			glm::vec3(0.61424, 0.04136, 0.04136),
			glm::vec3(0.727811, 0.626959, 0.626959),
			77.f
		} // ruby
	};

	const int cube_count = 200;

	ModelTransform cubeTrans[cube_count];
	int cubeMat[cube_count];
	for (int i = 0; i < cube_count; i++)
	{
		float scale = (rand() % 6 + 1) / 20.0f;
		cubeTrans[i] = {
			glm::vec3((rand() % 201 - 100) / 50.0f, (rand() % 201 - 100) / 50.0f, (rand() % 201 - 100) / 50.0f),
			glm::vec3(rand() / 100.0f, rand() / 100.0f, rand() / 100.0f),
			glm::vec3(scale, scale, scale)
		};
		cubeMat[i] = rand() % 3;

		if ((glm::vec3(0, 0, 0) - cubeTrans[i].position).length() < 0.7f)
			i--;
	}



	unsigned int box_texture;
	glGenTextures(1, &box_texture);

	glBindTexture(GL_TEXTURE_2D, box_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, box_width, box_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, box_width, box_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	unsigned int VBO_polygon, VAO_polygon;
	glGenBuffers(1, &VBO_polygon);
	glGenVertexArrays(1, &VAO_polygon);

	glBindVertexArray(VAO_polygon);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_polygon);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// color
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

#pragma endregion


	light_shader = new Shader("shaders\\light.vert", "shaders\\light.frag");
	backpack_shader = new Shader("shaders\\backpack.vert", "shaders\\backpack.frag");
	selecting_shader = new Shader("shaders\\colorSelect.vert", "shaders\\colorSelect.frag");
	picking_shader = new Shader("shaders\\picker.vert", "shaders\\picker.frag");
	particle_shader = new Shader("shaders\\particle.vert", "shaders\\particle.frag");
	Model ground("models/ground2/lowpoly_floor.obj", false);
	picker = new Picker();
	ParticleGenerator pg(particle_shader, "textures/smoke", "blackSmoke", 5000, 5.0f, 0.01f,  23);
	ParticleGenerator pg_tr(particle_shader, "textures/fire", "pngwing", 500, 0.02f, 0.001f, 1);
	HitBox hb = backpack->getHitBox();
	hb.scale(0.001f);
	//Model backpack("models/nordic-chair/Furniture_Chair_0.obj", false);
	//Model chair("models/chair/chair.obj", false);

	float max = 0;



	ModelTransform lightTrans = {
		glm::vec3(0.f, 0.f, 0.f),			// position
		glm::vec3(0.f, 0.f, 0.f),			// rotation
		glm::vec3(0.01, 0.01f, 0.01f) };	// scale


	ModelTransform particleTrans = {
		glm::vec3(0.f, 0.f, 0.f),			// position
		glm::vec3(0.f, 0.f, 0.f),			// rotation
		glm::vec3(0.001, 0.001f, 0.001f) };	// scale

	ModelTransform particleTracerTrans = {
		glm::vec3(0.f, 0.f, 0.f),			// position
		glm::vec3(0.f, 0.f, 0.f),			// rotation
		glm::vec3(0.0004, 0.0004f, 0.0004f) };	// scale
	double oldTime = glfwGetTime(), newTime, deltaTime;

#pragma region LIGHT INITIALIZATION

	vector<Light*> lights;
	int total_lights = 4;
	int active_lights = 0;

	redLamp = new Light("LampRed", true);
	redLamp->initLikePointLight(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(1.0f, 0.2f, 0.2f),
		glm::vec3(1.0f, 0.2f, 0.2f),
		1.0f, 0.1f, 0.09f);
	lights.push_back(redLamp);

	blueLamp = new Light("LampBlue", true);
	blueLamp->initLikePointLight(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.2f, 0.2f, 1.0f),
		glm::vec3(1.0f, 0.2f, 1.0f),
		1.0f, 0.1f, 0.09f);
	lights.push_back(blueLamp);


	sunLight = new Light("Sun", true);
	sunLight->initLikeDirectionalLight(
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.0f, 0.0f, 0.0f));
	lights.push_back(sunLight);


	flashLight = new Light("FlashLight", true);
	flashLight->initLikeSpotLight(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::radians(10.f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.7f, 0.7f, 0.6f),
		glm::vec3(0.8f, 0.8f, 0.6f),
		1.0f, 0.1f, 0.09f);
	lights.push_back(flashLight);

#pragma endregion

#pragma region TANK INITIALIZATION

	tanks.push_back(Tank(0.2f, 0));
	tanks.push_back(Tank(0.6f, 0));
	tanks.push_back(Tank(1.0f, 0));
#pragma endregion


#pragma region GROUND INITIALIZATION
	ModelTransform groundTrans = {
		glm::vec3(0.f, 0.f, 0.f),			// position
		glm::vec3(-90, 0.f, 0.0f),			// rotation
		glm::vec3(1,1,1) };	// scale
#pragma endregion



	while (!glfwWindowShouldClose(win))
	{
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		oldTime = newTime;

		processInput(win, deltaTime);





		flashLight->position = camera.Position - camera.Up * 0.3f;
		flashLight->direction = camera.Front;

		redLamp->position.x = 0.2f;
		redLamp->position.z = 0.1f * cos(newTime * 2);
		redLamp->position.y = 0.1f * sin(newTime * 2);



		blueLamp->position.x = 0.2f;
		blueLamp->position.z = 0.1f * cos(newTime * 2 + glm::pi<float>());
		blueLamp->position.y = 0.1f * sin(newTime * 2 + glm::pi<float>());

		//groundTrans.rotation.x = glfwGetTime() * 60.0;

		glClearColor(background.r, background.g, background.b, background.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 p = camera.GetProjectionMatrix();
		glm::mat4 v = camera.GetViewMatrix();
		glm::mat4 pv = p * v;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, redLamp->position);
		model = glm::scale(model, glm::vec3(0.01, 0.01f, 0.01f));




		// DRAWING BACKPACK SELECTING




		// DRAWING LAMPS
		light_shader->use();
		light_shader->setMatrix4F("pv", pv);
		glBindVertexArray(VAO_polygon);

		// Red Lamp
		lightTrans.position = redLamp->position;
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightTrans.position);
		model = glm::scale(model, lightTrans.scale);
		light_shader->setMatrix4F("model", model);
		light_shader->setVec3("lightColor", glm::vec3(1.0f, 0.2f, 0.2f));
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// Blue Lamp
		lightTrans.position = blueLamp->position;
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightTrans.position);
		model = glm::scale(model, lightTrans.scale);
		light_shader->setMatrix4F("model", model);
		light_shader->setVec3("lightColor", glm::vec3(0.2f, 0.2f, 1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 36);


		active_lights = 0;


		rejected.Update(deltaTime);
		vector<Missil> reject = rejected.getRejected();

		


		// Red lamp over the tank
		light_shader->use();
		for (auto& kv2 : tanks) {
			kv2.TransInfo(&modelTrans);
			if (kv2.getSelected()) {
				lightTrans.position = modelTrans.position;
				lightTrans.position.y += 0.2f;
				model = glm::mat4(1.0f);
				model = glm::translate(model, lightTrans.position);
				model = glm::scale(model, lightTrans.scale);
				light_shader->setMatrix4F("model", model);
				light_shader->setVec3("lightColor", glm::vec3(1.0f, 0.2f, 0.2f));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		
		//drawing tracer
		for (Missil& missil : reject) {
			glm::vec3 velocity = missil.getVelocity();
			velocity.x *= -0.01f;
			velocity.z *= -0.01f;
			particleTracerTrans.position = missil.getPosition();
			pg_tr.Update(deltaTime, particleTracerTrans.position, velocity, 5, glm::vec3(0.f, 0.1f, 0.f));
			pg_tr.Draw(pv);
		}

		for (auto& kv2 : tanks) {
			kv2.TransInfo(&modelTrans);

			float yaw = kv2.getYaw();
			glm::vec3 dir = kv2.getDir();
			glm::vec3 velocity = dir;
			velocity.x += sin(glm::radians(yaw + ((rand() % 60) - 30)));
			velocity.z += cos(glm::radians(yaw + ((rand() % 60) - 30)));
			glm::vec3 offset = glm::vec3(random(hb.xMin, hb.xMax) * dir.x, 0, random(hb.zMin, hb.zMax) * dir.z);
			velocity.x *= -0.01f;
			velocity.z *= -0.01f;
			velocity.y = 0.01f;
			particleTrans.position = modelTrans.position;

			pg.Update(deltaTime, particleTrans.position, velocity, 10, offset);
			pg.Draw(pv);
		}





		backpack_shader->use();
		for (int i = 0; i < lights.size(); i++)
		{
			active_lights += lights[i]->putInShader(backpack_shader, active_lights);
		}
		backpack_shader->setInt("lights_count", active_lights);


		for (auto& kv2 : tanks) {
			kv2.TransInfo(&modelTrans);

			// DRAWING BACKPACK
			model = glm::mat4(1.0f);
			model = glm::translate(model, modelTrans.position);
			model = glm::rotate(model, glm::radians(modelTrans.rotation.y), glm::vec3(0.f, 1.f, 0.f));
			model = glm::scale(model, modelTrans.scale);


			backpack_shader->setMatrix4F("pv", pv);
			backpack_shader->setMatrix4F("model", model);
			backpack_shader->setFloat("shininess", 64.0f);
			backpack_shader->setVec3("viewPos", camera.Position);
			backpack->Draw(backpack_shader);

		}



		for (int i = -10; i < 10; i++) {
			for (int j = -10; j < 10; j++) {
				groundTrans.position = glm::vec3(i * koef, 0, j * koef);
				model = glm::mat4(1.0f);
				model = glm::translate(model, groundTrans.position);
				model = glm::rotate(model, glm::radians(groundTrans.rotation.x), glm::vec3(1.f, 0.f, 0.f));
				model = glm::rotate(model, glm::radians(groundTrans.rotation.y), glm::vec3(0.f, 1.f, 0.f));
				model = glm::rotate(model, glm::radians(groundTrans.rotation.z), glm::vec3(0.f, 0.f, 1.f));
				model = glm::scale(model, groundTrans.scale);
				backpack_shader->use();
				backpack_shader->setMatrix4F("pv", pv);
				backpack_shader->setMatrix4F("model", model);
				backpack_shader->setFloat("shininess", 64.0f);
				backpack_shader->setVec3("viewPos", camera.Position);

				ground.Draw(backpack_shader);
			}
		}
		glfwSwapBuffers(win);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}