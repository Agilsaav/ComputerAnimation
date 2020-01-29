#include <iostream>
#include <cmath>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <time.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Scene.h"
#include "PLYReader.h"
#include "Tilemap.h"
#include "LOD.h"
#include "Pathfinding.h"

#define rand01() ((float)std::rand()/RAND_MAX)

Scene::Scene()
{
	mesh = NULL;
}

Scene::~Scene()
{
	if(mesh != NULL)
		delete mesh;
}


void Scene::init()
{
	initShaders("Res/shaders/basic.vert", "Res/shaders/basic.frag");

	currentTime = 0.0f;

	cube = new TriangleMesh();
	cube->buildCube();
	cube->sendToOpenGL(basicProgram);

	PLYReader reader;

	particleMesh = new TriangleMesh();
	bool Success = reader.readMesh("Res/Models/sphere.ply", *particleMesh);
	if (Success) particleMesh->sendToOpenGL(basicProgram);
	
	bPolygonFill = true;

	//camera.init(glm::vec3(3.0f, 0.5f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	camera.init(glm::vec3(0.0f, 6.0f, 13.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));	   	 

	// Select which font you want to use
	if(!text.init("Res/fonts/OpenSans-Regular.ttf"))
		cout << "Could not load font!!!" << endl;

	floortile = new FloorTile();

	//Particles:
	dt = 0.01f;
	solverUsed = 2;
	colRadius = sqrt(0.55f);
	distanceToPlane = 5.0f;
	frictionCoef = 0.9f;
	elasticCoef = 30.0f;
	dampingCoef = 0.5f;

	//Fluid Simulation contants
	particleMass = 0.02f;
	particleSize = 0.3f;
	Vol = 200.0f;
	numberOfParticles = Vol/(4.0f*PI*pow(particleSize,3));
	fluidDen = 998.2f;	
	viscFluid = 3.5f;
	k = 5.0f;
	surfaceTension = 0.0728f;
	surfaceTensionTresh = 6.0f;

	radiusAction = 0.25f;

	srand(time(NULL));

	int mode = 2;

	planes = InitializePlanes(distanceToPlane);
	particles = GenerateParticles(numberOfParticles, mode);
	particlesColor;

	oldDistances.resize(numberOfParticles * sizeof(float));
	newDistances.resize(numberOfParticles * sizeof(float));

	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		oldDistances[i].resize(6 * sizeof(float));
		newDistances[i].resize(6 * sizeof(float));

		//particles[i].computeParticleMass(fluidDen, Vol, numberOfParticles);
		particles[i].setMass(particleMass);
		particles[i].computePreasure(fluidDen, Vol, k); 

		//Sphere sphere(particles[i].getCurrentPosition(), particleSize);
		//partiSpheres.push_back(sphere);

		for (unsigned int j = 0; j < planes.size(); j++)
		{
			newDistances[i][j] = planes[j].distPoint2Plane(particles[i].getCurrentPosition());
		}

		particlesColor.push_back(glm::vec3(rand01(), rand01(), rand01()));
	}

	std::cout << "Number of Particles: " << numberOfParticles;

	std::cout << std::endl;
	std::cout << "Application ready!" << std::endl;

}

bool Scene::loadMesh(const char *filename)
{
	PLYReader reader;

	mesh->free();
	bool bSuccess = reader.readMesh(filename, *mesh);
	if(bSuccess)
	  mesh->sendToOpenGL(basicProgram);
	
	return bSuccess;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
}

void Scene::render(double fps)
{
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		particles[i].computeNN(particles, radiusAction, i);
		particles[i].computeParticleDensity(radiusAction, fluidDen);	

	}

	computeSimulation();  //Particle Simulation
	
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 proj = camera.getProjectionMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	
	glm::mat4 modelview = view * model;
	modelview = glm::scale(modelview, glm::vec3(10.0f, 10.0f, 10.0f));

	SendUniforms(proj, modelview, glm::vec3(0.9f, 0.9f, 0.95f));
	cube->render();


	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		model = glm::mat4(1.0f);
		modelview = view * model;

		modelview = glm::translate(modelview, particles[i].getCurrentPosition());
		modelview = glm::scale(modelview, glm::vec3(0.3f, 0.3f, 0.3f));
		SendUniforms(proj, modelview, glm::vec3(0.0f, 0.0f, 0.5f));
		particleMesh->render();		
	}

	text.render(std::to_string(fps), glm::vec2(20, 20), 16, glm::vec4(0, 0, 0, 1));
	//text.render("Mode: Mean Vertex ", glm::vec2(20, 40), 16, glm::vec4(0, 0, 0, 1));
}

Camera &Scene::getCamera()
{
  return camera;
}

void Scene::switchPolygonMode()
{
  bPolygonFill = !bPolygonFill;
}

void Scene::initShaders(const std::string &vpath, const std::string &fpath)
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, vpath);
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, fpath);
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	basicProgram.init();
	basicProgram.addShader(vShader);
	basicProgram.addShader(fShader);
	basicProgram.link();
	if(!basicProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << basicProgram.log() << endl << endl;
	}
	basicProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}


//Function to send Uniforms before draw the mesh
void Scene::SendUniforms(glm::mat4& proj, glm::mat4& modelview, glm::vec3 color)
{
	glm::mat3 normalMatrix;

	basicProgram.use();
	basicProgram.setUniformMatrix4f("projection", proj);
	basicProgram.setUniformMatrix4f("modelview", modelview);
	normalMatrix = glm::inverseTranspose(modelview);
	basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
	basicProgram.setUniform1i("bLighting", 1);

	basicProgram.setUniform4f("color", color.x, color.y, color.z, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



//Function to change the mode:
void Scene::changeModelMode()
{
	if (modelMode)
		modelMode = false;
	else
		modelMode = true;
}


Particle Scene::InitializeParticle(int mode)
{
	Particle p(0.0, 0.0, 0.0);
	float speed = 15.0f;
	float alpha, beta;
	glm::vec3 vel;

	switch (mode)
	{
	case 0: //Fountain
		p.setPosition(1.0f * (rand01() - 0.5), 1.0f * (rand01()), 1.0f * (rand01() - 0.5));
		vel = glm::vec3(8.0f * (rand01() - 0.5), 6.0f, 8.0f * (rand01() - 0.5));
		p.setVelocity(vel);
		if (solverUsed == 2) p.setPreviousPosition(p.getCurrentPosition() - dt * vel);
		break;
	case 1: //Waterfall
		//p.setPosition(8.0f * (rand01() - 0.5), 3.0f * (rand01()), 8.0f * (rand01() - 0.5));
		p.setPosition(8.0f * (rand01() - 0.5), 3.0f * (rand01()), 8.0f * (rand01() - 0.5));
		//vel = glm::vec3(1.0f * (rand01() - 0.5), 0.0, 1.0f * (rand01() - 0.5));
		vel = glm::vec3(30.f , 0.0, 2.f * (rand01() - 0.5));
		//vel = glm::vec3(rand01() - 0.2, 0.0, (rand01() - 0.5));
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

	default:
		p.setPosition(0.0, 0.0, 0.0);
	}

	p.setLifetime(5.0f * rand01());
	p.setBouncing(0.3f);
	//p.addForce(0.0f, -9.8f, 0.0f);

	return p;
}

std::vector<Particle> Scene::GenerateParticles(int numberOfParticles, int mode)
{
	std::vector<Particle> particles;

	float maxPos = 8.0f;
	float maxVel = 15.0f;

	for (unsigned int i = 0; i < numberOfParticles; i++)
	{

		//glm::vec3 position(2.0f * (maxPos * rand01() - (maxPos / 2.0f)), 0.0f, 2.0f * (maxPos * rand01() - (maxPos / 2.0f)));
		//glm::vec3 velocity(2.0f * (maxVel * rand01() - (maxVel / 2.0f)), 0.0f, 2.0f * (maxVel * rand01() - (maxVel / 2.0f)));
		particles.push_back(InitializeParticle(mode));

	}

	return particles;
}

std::vector<Plane> Scene::InitializePlanes(float dist)
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

void Scene::computeSimulation()
{

	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		physics.ComputeFluidForce(particles[i], particles, viscFluid, radiusAction, i);

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

		
		
		//std::vector<Particle*> nn = particles[i].getNeighbours();
		//for (unsigned int i = 0; i < nn.size(); i++)
		//{
		//	Sphere sphere(nn[i]->getCurrentPosition(), particleSize);
		//	physics.GetSpehereCollision(particles[i], sphere, frictionCoef, solverUsed);
		//}

		physics.GetPlaneCollisions(particles[i], planes, oldDistances[i], newDistances[i], frictionCoef, solverUsed);
		particles[i].setLifetime(particles[i].getLifetime() - dt);
	}
}






