#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SOIL2/SOIL2.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Zona3.h"
#include "Zona2.h"
#include "Zona1.h"
#include "Skybox.h"
#include "Escenario.h"

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

const GLuint WIDTH = 1280, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;

Camera  camera(glm::vec3(0.0f, 1.6f, 3.5f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

Zona3 zona3;
Zona2 zona2;
Zona1 zona1;
Skybox cielo;
Escenario escenario;

glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f, 1.3f, 0.2f)
};

float vertices[] = {
	-0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f, 0.5f,-0.5f, 0,0,-1,
	 0.5f, 0.5f,-0.5f, 0,0,-1, -0.5f, 0.5f,-0.5f, 0,0,-1, -0.5f,-0.5f,-0.5f, 0,0,-1,
	-0.5f,-0.5f, 0.5f, 0,0, 1,  0.5f,-0.5f, 0.5f, 0,0, 1,  0.5f, 0.5f, 0.5f, 0,0, 1,
	 0.5f, 0.5f, 0.5f, 0,0, 1, -0.5f, 0.5f, 0.5f, 0,0, 1, -0.5f,-0.5f, 0.5f, 0,0, 1,
	-0.5f, 0.5f, 0.5f,-1,0, 0, -0.5f, 0.5f,-0.5f,-1,0, 0, -0.5f,-0.5f,-0.5f,-1,0, 0,
	-0.5f,-0.5f,-0.5f,-1,0, 0, -0.5f,-0.5f, 0.5f,-1,0, 0, -0.5f, 0.5f, 0.5f,-1,0, 0,
	 0.5f, 0.5f, 0.5f, 1,0, 0,  0.5f, 0.5f,-0.5f, 1,0, 0,  0.5f,-0.5f,-0.5f, 1,0, 0,
	 0.5f,-0.5f,-0.5f, 1,0, 0,  0.5f,-0.5f, 0.5f, 1,0, 0,  0.5f, 0.5f, 0.5f, 1,0, 0,
	-0.5f,-0.5f,-0.5f, 0,-1,0,  0.5f,-0.5f,-0.5f, 0,-1,0,  0.5f,-0.5f, 0.5f, 0,-1,0,
	 0.5f,-0.5f, 0.5f, 0,-1,0, -0.5f,-0.5f, 0.5f, 0,-1,0, -0.5f,-0.5f,-0.5f, 0,-1,0,
	-0.5f, 0.5f,-0.5f, 0, 1,0,  0.5f, 0.5f,-0.5f, 0, 1,0,  0.5f, 0.5f, 0.5f, 0, 1,0,
	 0.5f, 0.5f, 0.5f, 0, 1,0, -0.5f, 0.5f, 0.5f, 0, 1,0, -0.5f, 0.5f,-0.5f, 0, 1,0
};

int main()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hanok - Ritual funerario coreano (Equipo 10)", nullptr, nullptr);
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	const char* fuente = "C:\\Windows\\Fonts\\segoeui.ttf";
	if (std::ifstream(fuente).good()) io.Fonts->AddFontFromFileTTF(fuente, 22.0f);
	else                              io.Fonts->AddFontDefault();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader skyboxShader("Shader/skybox.vs", "Shader/skybox.frag");
	cielo.cargar("Models/skybox");

	zona3.cargar();
	zona2.cargar();
	zona1.cargar();
	escenario.cargar();
	zona3.origen = glm::vec3(0.0f, 0.0f, 0.0f);

	GLuint texPiso = (GLuint)TextureFromFile("piso_maru.png", "Models/z3");

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()),
		(GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.05f, 200.0f);

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
		if (SCREEN_WIDTH > 0 && SCREEN_HEIGHT > 0) {
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			projection = glm::perspective(glm::radians(camera.GetZoom()),
				(GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.05f, 200.0f);
		}
		DoMovement();
		zona3.actualizar(deltaTime, camera.GetPosition());
		zona2.actualizar(deltaTime, camera.GetPosition());
		zona1.actualizar(deltaTime, camera.GetPosition());

		glClearColor(0.05f, 0.06f, 0.10f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		lightingShader.Use();
		glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 0);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		glm::vec3 camPos = camera.GetPosition();
		glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camPos.x, camPos.y, camPos.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), 0.85f, -0.45f, 0.30f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.32f, 0.27f, 0.30f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.80f, 0.50f, 0.32f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.10f, 0.10f, 0.10f);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"),
			pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.10f, 0.07f, 0.03f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 1.00f, 0.75f, 0.45f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.30f, 0.25f, 0.20f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.35f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.44f);

		Farol* faroles[2] = { &zona1.farol, &zona2.farol };
		for (int i = 0; i < 2; i++) {
			std::string luz = "pointLights[" + std::to_string(i + 1) + "].";
			glm::vec3 p = faroles[i]->posLuz();
			float k = faroles[i]->intensidad();
			glUniform3f(glGetUniformLocation(lightingShader.Program, (luz + "position").c_str()), p.x, p.y, p.z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, (luz + "ambient").c_str()), 0.04f * k, 0.02f * k, 0.01f * k);
			glUniform3f(glGetUniformLocation(lightingShader.Program, (luz + "diffuse").c_str()), 1.00f * k, 0.55f * k, 0.30f * k);
			glUniform3f(glGetUniformLocation(lightingShader.Program, (luz + "specular").c_str()), 0.30f * k, 0.20f * k, 0.10f * k);
			glUniform1f(glGetUniformLocation(lightingShader.Program, (luz + "constant").c_str()), 1.0f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, (luz + "linear").c_str()), 0.7f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, (luz + "quadratic").c_str()), 1.8f);
		}

		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camPos.x, camPos.y, camPos.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

		glm::mat4 view = camera.GetViewMatrix();
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.05f, 0.0f));
		model = glm::scale(model, glm::vec3(60.0f, 0.1f, 60.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texPiso);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		escenario.dibujar(lightingShader, modelLoc);
		zona2.dibujar(lightingShader, modelLoc);
		zona3.dibujar(lightingShader, modelLoc);
		zona1.dibujar(lightingShader, modelLoc);

		cielo.dibujar(skyboxShader, view, projection);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		zona3.dibujarUI();
		zona2.dibujarUI();
		zona1.dibujarUI();
		ImGui::SetNextWindowPos(ImVec2(12, 12));
		ImGui::SetNextWindowBgAlpha(0.45f);
		ImGui::Begin("##ayuda", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav);
		ImGui::TextDisabled("WASD: moverse   Ratón: mirar   Esc: salir");
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])    camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])  camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)        keys[key] = true;
		else if (action == GLFW_RELEASE) keys[key] = false;
	}

	if (action == GLFW_PRESS)
	{
		zona3.tecla(key);

		zona2.tecla(key);
		zona1.tecla(key);
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = (GLfloat)xPos;
		lastY = (GLfloat)yPos;
		firstMouse = false;
	}
	GLfloat xOffset = (GLfloat)xPos - lastX;
	GLfloat yOffset = lastY - (GLfloat)yPos;
	lastX = (GLfloat)xPos;
	lastY = (GLfloat)yPos;
	camera.ProcessMouseMovement(xOffset, yOffset);
}
