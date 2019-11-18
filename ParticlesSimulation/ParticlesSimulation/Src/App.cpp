#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include<iostream>
#include <vector>
#include <map>
#include <random>

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "My_Texture.h"
#include "camera.h"
#include "Cubemap.h"
#include "Model.h"
#include "Particle.h"
#include "Geometry.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include <time.h>

#define rand01() ((float)std::rand()/RAND_MAX)
//---------------------------------
/* Settings and Variables: */

//Functions definition
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

std::vector<Particle> GenerateParticles(int numberOfParticles, int mode);
Particle InitializeParticle(int mode);
std::vector<Plane> InitializePlanes(float dist);
void GetPlaneCollisions(Particle& p, std::vector<Plane>& planes, std::vector<float>& distBefore, std::vector<float>& distNew, float& frictionCoef);
void GetTriangleCollision(Particle& p, Triangle& tri, float& distBefore, float& distNew, float& frictionCoef);
void GetSpehereCollision(Particle& p, Sphere& sphere, float& frictionCoef);

// Window settings
const unsigned int SCR_WIDTH = 1280;		// Before: 800
const unsigned int SCR_HEIGHT = 720;	// Before: 600

//Paths:
std::string ShaderPath_Basic = "Res/shaders/basic.shader";
std::string ShaderPath_Model = "Res/shaders/model.shader";
std::string ShaderPath_Cube = "Res/shaders/cube.shader";
std::string ShaderPath_Triangle = "Res/shaders/triangle.shader";

std::string TexturePath = "Res/textures/text1.jpg";
std::string TexturePath2 = "Res/textures/tex5.jpg";
std::string TexturePath2_2 = "Res/textures/tex5_2.jpg";
std::string TexturePathSphere = "Res/textures/tex6.jpg";

//Timing:
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int frameNumber = 0;

//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 35.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
bool movecamera = true;
bool setcursorpos = false;

//Mouse:
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Light Properties:
glm::vec3 lightPos = glm::vec3(0.0, 5.0, 0.0);
glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);

//Sim vars:
float dt = 0.01f;  //simulation time
float tini = 0.0f;
float tfinal = 6.0f; //final time of simulation 

//Mode
int mode = 0;
int solverUsed = 0;

//---------------------------------

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Particles Simulation", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/*Mouse Capture*/
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	//Enable Depth test and Blend
	glEnable(GL_DEPTH_TEST);   
	//glDisable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	//---------------------------------
	float cube_vertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		//-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		// 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		// 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		//-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		//-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left     
		// bottom face
		//-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		// 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		// 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		//-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		//-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};

	float cubeBottom_vertices[] = {
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f // top-right
	};

	float cubeLat_vertices[] = {
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left  
	};

	float quad_vertices[] = { 
	// positions   // texCoords
	-1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, 0.0f,-1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,

	 1.0f, 0.0f,-1.0f, 1.0f, 0.0f,
	 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	-1.0f, 0.0f,-1.0f, 0.0f, 0.0f
	};

	//float triangle_vertices[] =
	//{
	//10.0f,  -5.0f, -10.0f, 0.0f, -400.0f, 0.0f,
	//10.0f,  -5.0f,  10.0f, 0.0f, -400.0f, 0.0f,
	//-10.0f, -5.0f, -10.0f, 0.0f, -400.0f, 0.0f
	//};

	float triangle_vertices[] =
	{
	6.0f,  -5.0f, 0.0f,  0.0f, -400.0f, 0.0f,
	8.0f,  -2.0f,  -8.0f, 0.0f, -400.0f, 0.0f,
	1.0f,  -5.0f, -5.0f,  0.0f, -400.0f, 0.0f
	};

	float triangle_vertices2[] =
	{
	-6.0f,  -5.0f, 0.0f,  0.0f, -400.0f, 0.0f,
	-8.0f,  -2.0f,  -8.0f, 0.0f, -400.0f, 0.0f,
	-1.0f,  -5.0f, -5.0f,  0.0f, -400.0f, 0.0f
	};
	//---------------------------------

	//Cube VAO and VBO
	VertexArray cube_va;
	VertexBuffer cube_vb(cube_vertices, sizeof(cube_vertices));
	VertexBufferLayout cube_layout;

	cube_layout.Push<float>(3); //Position
	cube_layout.Push<float>(3); //Normal
	cube_layout.Push<float>(2); //Texture
	cube_va.AddBuffer(cube_vb, cube_layout);

	VertexArray cubeLat_va;
	VertexBuffer cubeLat_vb(cubeLat_vertices, sizeof(cubeLat_vertices));
	VertexBufferLayout cubeLat_layout;

	cubeLat_layout.Push<float>(3); //Position
	cubeLat_layout.Push<float>(3); //Normal
	cubeLat_layout.Push<float>(2); //Texture
	cubeLat_va.AddBuffer(cubeLat_vb, cubeLat_layout);

	//Cube Bottom VAO and VBO
	VertexArray cubeBottom_va;
	VertexBuffer cubeBottom_vb(cubeBottom_vertices, sizeof(cubeBottom_vertices));
	VertexBufferLayout cubeBottom_layout;

	cubeBottom_layout.Push<float>(3); //Position
	cubeBottom_layout.Push<float>(3); //Normal
	cubeBottom_layout.Push<float>(2); //Texture
	cubeBottom_va.AddBuffer(cubeBottom_vb, cubeBottom_layout);

	//Quad VAO and VBO
	VertexArray quad_va;
	VertexBuffer quad_vb(quad_vertices, sizeof(quad_vertices));
	VertexBufferLayout quad_layout;

	quad_layout.Push<float>(3); //Position
	quad_layout.Push<float>(2); //Texture
	quad_va.AddBuffer(quad_vb, quad_layout);

	//Triangle
	VertexArray triangle_va;
	VertexBuffer triangle_vb(triangle_vertices, sizeof(triangle_vertices));
	VertexBufferLayout triangle_layout;

	triangle_layout.Push<float>(3); //Position
	triangle_layout.Push<float>(3); //Normal
	triangle_va.AddBuffer(triangle_vb, triangle_layout);

	VertexArray triangle2_va;
	VertexBuffer triangle2_vb(triangle_vertices2, sizeof(triangle_vertices2));
	VertexBufferLayout triangle2_layout;

	triangle2_layout.Push<float>(3); //Position
	triangle2_layout.Push<float>(3); //Normal
	triangle2_va.AddBuffer(triangle2_vb, triangle2_layout);

	//Model
	Model ourModel("Res/models/particle.obj");
	Model sphereModel("Res/models/sphere.obj");

	My_Texture texture1(TexturePath);
	My_Texture texture2(TexturePath2);
	My_Texture texture2_2(TexturePath2_2);

	//Shader init
	Shader shader_basic(ShaderPath_Basic);
	shader_basic.Bind();

	Shader shader_model(ShaderPath_Model);
	shader_model.Bind();

	Shader shader_cube(ShaderPath_Cube);
	shader_cube.Bind();
	shader_cube.SetUniform1i("texture1", 0);

	Shader shader_triangle(ShaderPath_Triangle);
	shader_triangle.Bind();

	//Renderer
	Renderer renderer;

	//Simulation Variables--------------	
	int numberOfParticles = 150;
	float distanceToPlane = 10.0f;
	float frictionCoef = 0.3f;

	srand(time(NULL));

	std::vector<Plane> planes = InitializePlanes(distanceToPlane);
	//glm::vec3 triangleP1(10.0f,  -5.0f, -10.0f);
	//glm::vec3 triangleP2(10.0f,  -5.0f, 10.0f);
	//glm::vec3 triangleP3(-10.0f, -5.0f, -10.0f);
	glm::vec3 triangleP1(6.0f, -5.0f, 0.0f);
	glm::vec3 triangleP2(8.0f, -2.0f, -8.0);
	glm::vec3 triangleP3(1.0f, -5.0f, -5.0f);
	Triangle tri(triangleP1, triangleP2, triangleP3);

	glm::vec3 triangle2P1(-6.0f, -5.0f, 0.0f);
	glm::vec3 triangle2P2(-8.0f, -2.0f, -8.0);
	glm::vec3 triangle2P3(-1.0f, -5.0f, -5.0f);
	Triangle tri2(triangle2P1, triangle2P2, triangle2P3);

	glm::vec3 spherePoint(0.0f, -5.0f, 0.0f);
	float radius = 4.0f;
	Sphere sphere(spherePoint, radius);

	std::vector<Particle> particles = GenerateParticles(numberOfParticles, mode);
	std::vector<glm::vec3> particlesColor;
	std::vector< std::vector<float>> oldDistances;
	std::vector< std::vector<float>> newDistances;
	std::vector<float> oldDistancesTri, oldDistancesTri2;
	std::vector<float> newDistancesTri, newDistancesTri2;
	
	oldDistances.resize(numberOfParticles * sizeof(float));
	newDistances.resize(numberOfParticles * sizeof(float));
	oldDistancesTri.resize(numberOfParticles * sizeof(float));
	newDistancesTri.resize(numberOfParticles * sizeof(float));
	oldDistancesTri2.resize(numberOfParticles * sizeof(float));
	newDistancesTri2.resize(numberOfParticles * sizeof(float));

	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		oldDistances[i].resize(6 * sizeof(float));
		newDistances[i].resize(6 * sizeof(float));

		for (unsigned int j = 0; j < planes.size(); j++)
		{
			newDistances[i][j] = planes[j].distPoint2Plane(particles[i].getCurrentPosition());
		}

		Plane plane(tri.vertex1, tri.vertex2, tri.vertex3);
		newDistancesTri[i] = plane.distPoint2Plane(particles[i].getCurrentPosition());

		Plane plane2(tri2.vertex1, tri2.vertex2, tri2.vertex3);
		newDistancesTri2[i] = plane2.distPoint2Plane(particles[i].getCurrentPosition());

		//newDistancesSphere[i] = sphere.distanceToSpehere(particles[i].getCurrentPosition());

		particlesColor.push_back(glm::vec3(rand01(), rand01(), rand01()));
	}

	int count = 0;
	float time = tini;

	//ImGui-----------------------------
	ImGui::CreateContext();
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();

	//---------------------------------
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//Per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Input
		processInput(window);

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//Simulation

		for (unsigned int i = 0; i < numberOfParticles; i++)
		{
			if (particles[i].getLifetime() > 0) 
			{
				switch (solverUsed)
				{
					case 0:
						particles[i].updateParticle(dt, Particle::UpdateMethod::EulerOrig);
						break;
					case 1:
						particles[i].updateParticle(dt, Particle::UpdateMethod::EulerSemi);
						break;
					case 2:
						particles[i].updateParticle(dt, Particle::UpdateMethod::Verlet);
						break;

				}
				particles[i].updateParticle(dt, Particle::UpdateMethod::Verlet);
				GetPlaneCollisions(particles[i], planes, oldDistances[i], newDistances[i], frictionCoef);
				GetTriangleCollision(particles[i], tri, oldDistancesTri[i], newDistancesTri[i], frictionCoef);
				GetTriangleCollision(particles[i], tri2, oldDistancesTri2[i], newDistancesTri2[i], frictionCoef);
				GetSpehereCollision(particles[i], sphere, frictionCoef);

				particles[i].setLifetime(particles[i].getLifetime() - dt);
			}
			else
			{
				particles[i] = InitializeParticle(mode);
			}
		}

		time = time + dt; //increase time counter

		{
			//Rendering-------------------------------------------------------------------
			glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 model = glm::mat4(1.0f);

			//Cube
			model = glm::scale(model, glm::vec3(10.0f, 10.0, 10.0f));
			glm::mat4 mvp = proj * view * model;

			texture1.Bind();
			shader_cube.Bind();
			shader_cube.SetUniform1i("isTriangle", 0);
			shader_cube.SetUniform3fv("lightColor", lightColor);
			shader_cube.SetUniform3fv("lightPos", lightPos);
			shader_cube.SetUniform3fv("viewPos", camera.Position);
			shader_cube.SetUniformMat4f("mvp", mvp);
			shader_cube.SetUniformMat4f("model", model);
			renderer.Draw_NoIb(cubeBottom_va, shader_cube);
			texture1.Unbind();

			texture2.Bind();
			renderer.Draw_NoIb(cube_va, shader_cube);
			texture2.Unbind();
			//texture2_2.Bind();
			//renderer.Draw_NoIb(cubeLat_va, shader_cube);
			//shader_triangle.Bind();
			//shader_triangle.SetUniform1i("isTriangle", 0);
			//shader_triangle.SetUniform3fv("lightColor", lightColor);
			//shader_triangle.SetUniform3fv("lightPos", lightPos);
			//shader_triangle.SetUniform3fv("viewPos", camera.Position);
			//shader_triangle.SetUniformMat4f("mvp", mvp);
			//shader_triangle.SetUniformMat4f("model", model);
			//renderer.Draw_NoIb(cube_va, shader_triangle);

			//Triangle
			model = glm::mat4(1.0f);
			mvp = proj * view * model;
			shader_triangle.Bind();
			shader_triangle.SetUniform1i("isTriangle", 1);
			shader_triangle.SetUniform3fv("lightColor", lightColor);
			shader_triangle.SetUniform3fv("lightPos", lightPos);
			shader_triangle.SetUniform3fv("viewPos", camera.Position);
			shader_triangle.SetUniformMat4f("mvp", mvp);
			shader_triangle.SetUniformMat4f("model", model);
			renderer.Draw_Triangle(triangle_va, shader_triangle);
			renderer.Draw_Triangle(triangle2_va, shader_triangle);

			//Sphere
			glm::vec3 scaleVec(radius, radius, radius);
			model = glm::mat4(1.0f);
			model = glm::translate(model, spherePoint);
			model = glm::scale(model, scaleVec);
			mvp = proj * view * model;
			shader_model.Bind();
			shader_model.SetUniformMat4f("mvp", mvp);
			glm::vec3 sphereColor = glm::vec3(0.0f, 0.3f, 1.0f);
			shader_model.SetUniform3fv("particleColor", sphereColor);
			sphereModel.Draw(shader_model);

			//Particles
			for (unsigned int i = 0; i < numberOfParticles; i++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, particles[i].getCurrentPosition());
				model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
				mvp = proj * view * model;
				shader_model.Bind();
				shader_model.SetUniformMat4f("mvp", mvp);
				shader_model.SetUniform3fv("particleColor", particlesColor[i]);
				ourModel.Draw(shader_model);
			}

			//ImGui-----------------------------------------------------------------------
			ImGui::Begin("Particles Simulation");
				//ImGui::SetWindowFontScale(1.2);
				ImGui::SetWindowPos(ImVec2(10.0, 10.0));
				ImGui::SetWindowSize(ImVec2(220.0, 150.0));

				switch (mode)
				{
					case 0:
						ImGui::Text("Mode: Fountain");
						break;
					case 1:
						ImGui::Text("Mode: Waterfall");
						break;
					case 2:
						ImGui::Text("Mode: Explosion");
						break;
				}

				switch (solverUsed)
				{
				case 0:
					ImGui::Text("Solver: Euler Orig");
					break;
				case 1:
					ImGui::Text("Solver: Euler Semi");
					break;
				case 2:
					ImGui::Text("Solver: Verlet");
					break;
				}

				ImGui::Text("Number of Particles: %i", numberOfParticles);
				ImGui::Text("Bouncing coeficient: %.1f", particles[0].getBouncing());
				bool reset = ImGui::Button("Reset Simulation");
				if (reset)
				{
					particles = GenerateParticles(numberOfParticles, mode);
				}

			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}


Particle InitializeParticle(int mode)
{
	Particle p(0.0, 0.0, 0.0);
	float speed = 15.0f;
	float alpha, beta;
	glm::vec3 vel;

	switch (mode)
	{
		case 0: //Fountain
			p.setPosition(0.0, 3.0, 0.0);
			vel = glm::vec3(8.0f * (rand01() - 0.5), 10.0f, 8.0f * (rand01() - 0.5));
			p.setVelocity(vel);
			if (solverUsed == 2) p.setPreviousPosition(p.getCurrentPosition() - dt * vel);
			break;
		case 1: //Waterfall
			p.setPosition(0.0, 5.0, 0.0);
			vel = glm::vec3(3.0f * (rand01() - 0.5), 0.0, 3.0f * (rand01() - 0.5));
			p.setVelocity(vel);
			if (solverUsed == 2) p.setPreviousPosition(p.getCurrentPosition() - dt * vel);
			break;
		case 2: //Explosion
			alpha = 360 * (rand01() - 0.5);
			beta = 180 * (rand01() - 0.5);
			p.setPosition(0.01 * cos(alpha) * cos(beta), 0.01 * sin(alpha), 0.01 * sin(alpha) * cos(beta));
			vel = glm::vec3(speed * cos(alpha) * cos(beta), speed * sin(alpha), speed * sin(alpha) * cos(beta));
			p.setVelocity(vel);
			if (solverUsed == 2) p.setPreviousPosition(p.getCurrentPosition() - dt * vel);
			break;
		//case 3: //Semi-Sphere
		//	alpha = 360 * (rand01() - 0.5);
		//	beta = 90 * rand01();
		//	p.setPosition(cos(alpha)*cos(beta), sin(alpha), sin(alpha)*cos(beta));
		//	p.setVelocity(speed * cos(alpha) * cos(beta), speed * sin(alpha),speed * sin(alpha) * cos(beta));

		default:
			p.setPosition(0.0, 0.0, 0.0);
	}

	p.setLifetime(5.0f * rand01());
	p.setBouncing(0.9f);
	p.addForce(0.0f, -9.8f, 0.0f);

	return p;
}

std::vector<Particle> GenerateParticles(int numberOfParticles, int mode)
{
	std::vector<Particle> particles;

	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		particles.push_back(InitializeParticle(mode));
	}

	return particles;
}

std::vector<Plane> InitializePlanes(float dist)
{
	std::vector<Plane> planes_temp;

	planes_temp.push_back(Plane(glm::vec3(-dist), glm::vec3(0, 1, 0)));
	planes_temp.push_back(Plane(glm::vec3(dist), glm::vec3(0, -1, 0)));
	planes_temp.push_back(Plane(glm::vec3(dist), glm::vec3(-1, 0, 0)));
	planes_temp.push_back(Plane(glm::vec3(-dist), glm::vec3(1, 0, 0)));
	planes_temp.push_back(Plane(glm::vec3(dist), glm::vec3(0, 0, -1)));
	planes_temp.push_back(Plane(glm::vec3(-dist), glm::vec3(0, 0, 1)));

	return planes_temp;
}

void GetPlaneCollisions(Particle& p, std::vector<Plane>& planes, std::vector<float>& distBefore, std::vector<float>& distNew, float& frictionCoef)
{
	for (unsigned int i = 0; i < planes.size(); i++)
	{
		distBefore[i] = distNew[i];
		distNew[i] = planes[i].distPoint2Plane(p.getCurrentPosition());
		if (distNew[i] * distBefore[i] < 0.0f)
		{
			glm::vec3 newPosition = p.getCurrentPosition() - (1.0f + p.getBouncing()) * (glm::dot(planes[i].normal, p.getCurrentPosition()) + planes[i].dconst ) * planes[i].normal;
			glm::vec3 newVelocity = p.getVelocity() - (1.0f + p.getBouncing()) * glm::dot(planes[i].normal, p.getVelocity()) * planes[i].normal;
			glm::vec3 normVelocity = glm::dot(planes[i].normal, p.getVelocity()) * planes[i].normal;
			glm::vec3 tgVelocity = p.getVelocity() - normVelocity;
			p.setPosition(newPosition);
			p.setVelocity(newVelocity - frictionCoef *tgVelocity);
			distNew[i] = -distNew[i];

			if(solverUsed == 2)
			{
				glm::vec3 newPrevPos = p.getPreviousPosition() - (1.0f + p.getBouncing()) * (glm::dot(planes[i].normal, p.getPreviousPosition()) + planes[i].dconst) * planes[i].normal;
				p.setPreviousPosition(newPrevPos);
			}
		}
	}
}

void GetTriangleCollision(Particle& p, Triangle& tri, float& distBefore, float& distNew, float& frictionCoef)
{
	Plane plane(tri.vertex1, tri.vertex2, tri.vertex3);
	
	distBefore = distNew;
	distNew = plane.distPoint2Plane(p.getCurrentPosition());
	if (distNew * distBefore < 0.0f) //Plane colision
	{
		if(tri.isInside(p.getCurrentPosition())) //Inside Triangle
		{
			glm::vec3 newPosition = p.getCurrentPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getCurrentPosition()) + plane.dconst) * plane.normal;
			glm::vec3 newVelocity = p.getVelocity() - (1.0f + p.getBouncing()) * glm::dot(plane.normal, p.getVelocity()) * plane.normal;
			glm::vec3 normVelocity = glm::dot(plane.normal, p.getVelocity()) * plane.normal;
			glm::vec3 tgVelocity = p.getVelocity() - normVelocity;
			p.setPosition(newPosition);
			p.setVelocity(newVelocity - frictionCoef * tgVelocity);
			distNew = -distNew;

			if (solverUsed == 2)
			{
				glm::vec3 newPrevPos = p.getPreviousPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getPreviousPosition()) + plane.dconst) * plane.normal;
				p.setPreviousPosition(newPrevPos);
			}
		}
	}
}

void GetSpehereCollision(Particle& p, Sphere& sphere, float& frictionCoef)
{
	if (sphere.isInside(p.getCurrentPosition())) //Inside sphere
	{

		//Compute Plane
		glm::vec3 pointP = sphere.computePointP(p.getPreviousPosition(), p.getCurrentPosition());
		glm::vec3 normP(pointP.x - sphere.center.x, pointP.y - sphere.center.y, pointP.z - sphere.center.z);
		Plane plane(pointP, normP);

		//Plane Collision:
		
		glm::vec3 newPosition = p.getCurrentPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getCurrentPosition()) + plane.dconst) * plane.normal;
		glm::vec3 newVelocity = p.getVelocity() - (1.0f + p.getBouncing()) * glm::dot(plane.normal, p.getVelocity()) * plane.normal;
		p.setPosition(newPosition);
		p.setVelocity(newVelocity);

		if (solverUsed == 2)
		{
			glm::vec3 newPrevPos = p.getPreviousPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getPreviousPosition()) + plane.dconst) * plane.normal;
			p.setPreviousPosition(newPrevPos);
		}
	}
}

//Key Orders:
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		mode = 0;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		mode = 1;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		mode = 2;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		solverUsed = 0;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		solverUsed = 1;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		solverUsed = 2;

	//Enable disable Cursor
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		movecamera = false;
		setcursorpos = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		movecamera = true;
		if (setcursorpos == true)
		{
			glfwSetCursorPos(window, lastX, lastY); //When the curso dissapear returns it to the position it was before
			setcursorpos = false;
		}
	}

}

//Changing the window size:
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Mouse call:
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(movecamera)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset, true);
	}
}

//Scroll call:
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
