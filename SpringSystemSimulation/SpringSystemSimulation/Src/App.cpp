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

std::vector<Particle> GenerateParticles(int numberOfParticles, float stringLength);
std::vector<Particle> GenerateParticles2D(int rows, int columns, float stringLength);
Particle InitializeParticle(float x_pos, float y_pos);
Particle InitializeParticle2D(float x_pos, float y_pos, float z_pos);

std::vector<Plane> InitializePlanes(float dist);


void GetPlaneCollisions(Particle& p, std::vector<Plane>& planes, std::vector<float>& distBefore, std::vector<float>& distNew, float& frictionCoef);
void GetTriangleCollision(Particle& p, Triangle& tri, float& distBefore, float& distNew, float& frictionCoef);
void GetSpehereCollision(Particle& p, Sphere& sphere, float& frictionCoef);

void ComputeForces1D(std::vector<Particle>& particles, float elasticCoef, float dampingCoef, float stringLength);
void ComputeForces2D(std::vector<Particle>& particles, int rows, int columns, float elasticCoef, float dampingCoef, float stringLength);
glm::vec3 ComputeDampingForce(Particle& p1, Particle& p2, float dampingCoef);
glm::vec3 ComputeElasticForce(Particle& p1, Particle& p2, float elasticCoef, float stringLength);
void ComputeParticleForce(std::vector<Particle>& particles, std::vector<glm::vec3>& particlesForces, int i, int j, float elasticCoef, float dampingCoef, float stringLength);


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
int solverUsed = 2;
bool oneDim = true;

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
	int numberOfParticles = 5;
	int numberOfStrings = numberOfParticles - 1;
	float stringLength = 2.0f;
	float distanceToPlane = 10.0f;
	//float frictionCoef = 0.3f;
	float frictionCoef = 0.8f;
	float elasticCoef = 30.0f;
	float dampingCoef = 0.5f;

	//2D:
	int rows = 7;
	int columns = 7;
	int numberOfParticles2D = rows * columns;
	std::vector<Particle> particles2D = GenerateParticles2D(rows, columns, stringLength);
	std::vector< std::vector<float>> oldDistances2D;
	std::vector< std::vector<float>> newDistances2D;
	oldDistances2D.resize(numberOfParticles2D * sizeof(float));
	newDistances2D.resize(numberOfParticles2D * sizeof(float));

	srand(time(NULL));

	std::vector<Plane> planes = InitializePlanes(distanceToPlane);
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

	std::vector<Particle> particles = GenerateParticles(numberOfParticles, stringLength);
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

		particlesColor.push_back(glm::vec3(rand01(), rand01(), rand01()));
	}

	for (unsigned int i = 0; i < numberOfParticles2D; i++)
	{

		oldDistances2D[i].resize(6 * sizeof(float));
		newDistances2D[i].resize(6 * sizeof(float));

		for (unsigned int j = 0; j < planes.size(); j++)
		{
			newDistances2D[i][j] = planes[j].distPoint2Plane(particles2D[i].getCurrentPosition());
		}

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
		if (oneDim)
		{
			ComputeForces1D(particles, elasticCoef, dampingCoef, stringLength);

			for (unsigned int i = 0; i < numberOfParticles; i++)
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

				GetPlaneCollisions(particles[i], planes, oldDistances[i], newDistances[i], frictionCoef);
				GetSpehereCollision(particles[i], sphere, frictionCoef);

				particles[i].setLifetime(particles[i].getLifetime() - dt);
			}
		}
		else
		{
			ComputeForces2D(particles2D, rows, columns, 4.0f, 5.0f, stringLength);

			for (unsigned int i = 0; i < numberOfParticles2D; i++)
			{
				switch (solverUsed)
				{
				case 0:
					particles2D[i].updateParticle(dt, Particle::UpdateMethod::EulerOrig);
					break;
				case 1:
					particles2D[i].updateParticle(dt, Particle::UpdateMethod::EulerSemi);
					break;
				case 2:
					particles2D[i].updateParticle(dt, Particle::UpdateMethod::Verlet);
					break;

				}

				GetPlaneCollisions(particles2D[i], planes, oldDistances2D[i], newDistances2D[i], frictionCoef);
				GetSpehereCollision(particles2D[i], sphere, frictionCoef);

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

			//Triangle
			//model = glm::mat4(1.0f);
			//mvp = proj * view * model;
			//shader_triangle.Bind();
			//shader_triangle.SetUniform1i("isTriangle", 1);
			//shader_triangle.SetUniform3fv("lightColor", lightColor);
			//shader_triangle.SetUniform3fv("lightPos", lightPos);
			//shader_triangle.SetUniform3fv("viewPos", camera.Position);
			//shader_triangle.SetUniformMat4f("mvp", mvp);
			//shader_triangle.SetUniformMat4f("model", model);
			//renderer.Draw_Triangle(triangle_va, shader_triangle);
			//renderer.Draw_Triangle(triangle2_va, shader_triangle);

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
			if (oneDim)
			{

				for (unsigned int i = 0; i < numberOfParticles; i++)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, particles[i].getCurrentPosition());
					model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
					mvp = proj * view * model;
					shader_model.Bind();
					shader_model.SetUniformMat4f("mvp", mvp);
					glm::vec3 color(0.0, 1.0, 0.0);
					shader_model.SetUniform3fv("particleColor", color);
					ourModel.Draw(shader_model);
				}

				model = glm::mat4(1.0f);
				mvp = proj * view * model;
				shader_model.SetUniformMat4f("mvp", mvp);

				for (unsigned int i = 0; i < numberOfParticles; i++)
				{
					if (i < numberOfParticles - 1) renderer.Draw_Line(particles[i].getCurrentPosition(), particles[i + 1].getCurrentPosition());
				}

			}
			else
			{

				for (unsigned int i = 0; i < numberOfParticles2D; i++)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, particles2D[i].getCurrentPosition());
					model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
					mvp = proj * view * model;
					shader_model.Bind();
					shader_model.SetUniformMat4f("mvp", mvp);
					glm::vec3 color(0.0, 1.0, 0.0);
					shader_model.SetUniform3fv("particleColor",color);
					ourModel.Draw(shader_model);
				}

				model = glm::mat4(1.0f);
				mvp = proj * view * model;
				shader_model.SetUniformMat4f("mvp", mvp);
				

				for (unsigned int i = 0; i < rows; i++)
				{
					for (unsigned int j = 0; j < columns; j++)
					{
						int numParticle = (columns * i) + j;
						int numParticle_PlusJ = (columns * i) + (j + 1);
						int numParticle_PlusI = (columns * (i+1)) + j;
						int numParticle_Diag = (columns * (i + 1)) + (j+1);

						if (j < columns - 1) renderer.Draw_Line(particles2D[numParticle].getCurrentPosition(), particles2D[numParticle_PlusJ].getCurrentPosition());
						if (i < rows - 1) renderer.Draw_Line(particles2D[numParticle].getCurrentPosition(), particles2D[numParticle_PlusI].getCurrentPosition());
						if (i < rows - 1 && j < columns - 1) renderer.Draw_Line(particles2D[numParticle].getCurrentPosition(), particles2D[numParticle_Diag].getCurrentPosition());
					}
				}
			}

			//ImGui-----------------------------------------------------------------------
			ImGui::Begin("Particles Simulation");
				//ImGui::SetWindowFontScale(1.2);
				ImGui::SetWindowPos(ImVec2(10.0, 10.0));
				ImGui::SetWindowSize(ImVec2(220.0, 200.0));

				if(oneDim) ImGui::Text("1D Simulation");
				else ImGui::Text("2D Simulation");


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

				if (oneDim)
				{
					ImGui::Text("Number of Particles: %i", numberOfParticles);
					ImGui::Text("Bouncing coeficient: %.1f", particles[0].getBouncing());
					ImGui::Text("Elastic coeficient: %.1f", elasticCoef);
					ImGui::Text("Damping coeficient: %.1f", dampingCoef);
				}
				else
				{
					ImGui::Text("Number of Particles: %i", rows*columns);
					ImGui::Text("Bouncing coeficient: %.1f", particles[0].getBouncing());
					ImGui::Text("Elastic coeficient: %.1f", 4.0f);
					ImGui::Text("Damping coeficient: %.1f", 5.0f);
				}

				bool reset = ImGui::Button("Reset Simulation");
				if (reset && oneDim)
				{
					particles = GenerateParticles(numberOfParticles, stringLength);
				}

				if (reset && !oneDim)
				{
					particles2D = GenerateParticles2D(rows, columns, stringLength);
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


Particle InitializeParticle(float x_pos, float y_pos)
{
	Particle p(0.0, 0.0, 0.0);

	p.setPosition(x_pos, y_pos, 0.0f);
	p.setVelocity(0.0f, 0.0f, 0.0f);

	p.setLifetime(5.0f * rand01());
	p.setBouncing(0.5f);
	//p.addForce(0.0f, -9.8f, 0.0f);

	return p;
}

Particle InitializeParticle2D(float x_pos, float y_pos, float z_pos)
{
	Particle p(0.0, 0.0, 0.0);

	p.setPosition(x_pos, y_pos, z_pos);
	p.setVelocity(0.0f, 0.0f, 0.0f);

	//if (solverUsed == 2) p.setPreviousPosition(p.getCurrentPosition());

	p.setLifetime(5.0f * rand01());
	p.setBouncing(0.8f);
	//p.addForce(0.0f, -9.8f, 0.0f);

	return p;
}

std::vector<Particle> GenerateParticles(int numberOfParticles, float stringLength)
{
	std::vector<Particle> particles;

	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		particles.push_back(InitializeParticle(stringLength * i, 0.0f));
	}

	return particles;
}

std::vector<Particle> GenerateParticles2D(int rows, int columns, float stringLength)
{
	std::vector<Particle> particles;

	float moveFirstPartX = (int)((columns - 1) / 2) * stringLength;
	float moveFirstPartY = 6.0f;
	float moveZ = 1.0f;
	float moveY = sqrt(pow(stringLength, 2) - pow(moveZ, 2));

	if (mode == 0)
	{
		for (unsigned int i = 0; i < rows; i++)
		{
			for (unsigned int j = 0; j < columns; j++)
			{
				particles.push_back(InitializeParticle2D(-moveFirstPartX + stringLength * j, moveFirstPartY - moveY * i, moveZ * i));
				//particles.push_back(InitializeParticle2D(stringLength * j, 5.0f, stringLength * i));
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < rows; i++)
		{
			for (unsigned int j = 0; j < columns; j++)
			{
				particles.push_back(InitializeParticle2D(-moveFirstPartX + stringLength * j, 5.0f, -6.0f +stringLength * i));
			}
		}

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

glm::vec3 ComputeElasticForce(Particle& p1, Particle& p2, float elasticCoef, float stringLength)
{
	glm::vec3 posDif = p2.getCurrentPosition() - p1.getCurrentPosition();
	glm::vec3 dir = posDif / glm::length(posDif);
	glm::vec3 elasticForce = elasticCoef * (glm::length(posDif) - stringLength) * dir;

	//std::cout << "elastic" << elasticForce.x << " " << elasticForce.y << " " << elasticForce.z << " " << std::endl;

	return elasticForce;
}

glm::vec3 ComputeDampingForce(Particle& p1, Particle& p2, float dampingCoef)
{
	glm::vec3 posDif = p2.getCurrentPosition() - p1.getCurrentPosition();
	glm::vec3 vecDif = p2.getVelocity() - p1.getVelocity();
	glm::vec3 dir = posDif / glm::length(posDif);
	glm::vec3 dampingForce = dampingCoef * glm::dot(vecDif, dir) * dir;

	//std::cout << "damping" << dampingForce.x << " " << dampingForce.y << " " << dampingForce.z << " " << std::endl;

	return dampingForce;
}


void ComputeForces1D(std::vector<Particle>& particles, float elasticCoef, float dampingCoef, float stringLength)
{
	std::vector<glm::vec3> particlesForces;
	//particlesForces.resize(particles.size() * sizeof(float));

	for (unsigned int i = 0; i < particles.size(); i++)
	{
		particlesForces.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	for (unsigned int i = 0; i < particles.size(); i++)
	{		
		if(i != 0) particlesForces[i] += glm::vec3(0.0, -9.8f, 0.0f);

		if (i < particles.size() - 1)
		{
			glm::vec3 elasticForce = ComputeElasticForce(particles[i], particles[i + 1], elasticCoef, stringLength);
			glm::vec3 dampingForce = ComputeDampingForce(particles[i], particles[i + 1], dampingCoef);

			particlesForces[i] += (elasticForce + dampingForce);
			particlesForces[i+1] -= (elasticForce + dampingForce);
		}
		particles[i].setForce(particlesForces[i]);
	}

	particles[0].setForce(0.0f, 0.0f, 0.0f);
}

void ComputeForces2D(std::vector<Particle>& particles, int rows, int columns, float elasticCoef, float dampingCoef, float stringLength)
{
	std::vector<glm::vec3> particlesForces;
	//particlesForces.resize(particles.size() * sizeof(float));

	for (unsigned int i = 0; i < particles.size(); i++)
	{
		particlesForces.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	float stringDiagDist = sqrt(2 * pow(stringLength, 2));


	for (unsigned int i = 0; i < rows; i++)
	{
		for (unsigned int j = 0; j < columns; j++)
		{
			int numParticle = (columns * i) + j;
			int numParticlePlus_i = (columns * (i + 1)) + j;
			int numParticleMinus_i = (columns * (i - 1)) + j;
			int numParticlePlus_j = (columns * i) + (j+1);
			int numParticleMinus_j = (columns * i) + (j-1);
			int numParticlePlus_2i = (columns * (i + 2)) + j;
			int numParticleMinus_2i = (columns * (i - 2)) + j;
			int numParticlePlus_2j = (columns * i) + (j + 2);
			int numParticleMinus_2j = (columns * i) + (j - 2);
			int numParticleDiag1 = (columns * (i + 1)) + (j - 1);
			int numParticleDiag2 = (columns * (i - 1)) + (j + 1);
			int numParticleDiag3 = (columns * (i + 1)) + (j + 1);
			int numParticleDiag4 = (columns * (i - 1)) + (j - 1);

			//Gravity
			if (i != 0) particlesForces[numParticle] += glm::vec3(0.0, -9.8f/2.0f, 0.0f);

			//Streach
			if (i < rows - 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticlePlus_i, elasticCoef, dampingCoef, stringLength);

			if (i > 0)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleMinus_i, elasticCoef, dampingCoef, stringLength);

			if (j < columns - 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticlePlus_j, elasticCoef, dampingCoef, stringLength);

			if (j > 0)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleMinus_j, elasticCoef, dampingCoef, stringLength);

			//Shear
			if(i < rows - 1 && j > 0)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleDiag1, elasticCoef, dampingCoef, stringDiagDist);

			if(i > 0 && j < columns - 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleDiag2, elasticCoef, dampingCoef, stringDiagDist);

			if(i < rows - 1 && j < columns - 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleDiag3, elasticCoef, dampingCoef, stringDiagDist);

			if(i > 0 && j > 0)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleDiag4, elasticCoef, dampingCoef, stringDiagDist);

			//Bend
			if(i < rows - 2)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticlePlus_2i, elasticCoef , dampingCoef , 2.0f*stringLength);

			if (i > 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleMinus_2i, elasticCoef, dampingCoef , 2.0f * stringLength);

			if (j < columns - 2)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticlePlus_2j, elasticCoef , dampingCoef , 2.0f * stringLength);

			if (j > 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleMinus_2j, elasticCoef, dampingCoef , 2.0f * stringLength);

		}

	}

	for (unsigned int i = 0; i < rows; i++)
	{
		for (unsigned int j = 0; j < columns; j++)
		{
			int numParticle = (columns * i) + j;
			if (i != 0) particles[numParticle].setForce(particlesForces[numParticle]);
			else particles[numParticle].setForce(0.0, 0.0, 0.0);
		}
	}
}

void ComputeParticleForce(std::vector<Particle>& particles, std::vector<glm::vec3>& particlesForces, int i, int j, float elasticCoef, float dampingCoef, float stringLength)
{
	glm::vec3 elasticForce = ComputeElasticForce(particles[i], particles[j], elasticCoef, stringLength);
	glm::vec3 dampingForce = ComputeDampingForce(particles[i], particles[j], dampingCoef);

	particlesForces[i] += (elasticForce + dampingForce);
	particlesForces[j] -= (elasticForce + dampingForce);
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
