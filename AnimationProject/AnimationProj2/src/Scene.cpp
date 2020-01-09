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

	//Particles:
	numberOfParticles = 10;
	dt = 0.01f;
	solverUsed = 1;
	colRadius = 0.4f * 0.50f;
	distanceToPlane = 10.0f;
	frictionCoef = 0.3f; //0.6f
	elasticCoef = 30.0f;
	dampingCoef = 0.5f;

	currentTime = 0.0f;

	wall = new TriangleMesh();
	wall->buildWall();
	wall->sendToOpenGL(basicProgram);

	PLYReader reader;

	sphere = new TriangleMesh();
	bool Success = reader.readMesh("Res/Models/sphere.ply", *sphere);
	if (Success) sphere->sendToOpenGL(basicProgram);

	floortile = new FloorTile();

	srand(time(NULL));
	
	bPolygonFill = true;

	//camera.init(glm::vec3(3.0f, 0.5f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	camera.init(glm::vec3(10.0f, 10.0f, 25.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));	   	 

	// Select which font you want to use
	if(!text.init("Res/fonts/OpenSans-Regular.ttf"))
		cout << "Could not load font!!!" << endl;

	//Load Tilemap
	std::cout << "Loading Tilemap..." << std::endl;
	Tilemap tilemap;
	tilemap.init("Res/Tilemap/Tilemap.txt");
	tilemapUsed = 0;
	tilemapvec = tilemap.getTilemap();
	tilemapRows = tilemap.getTilemapRows();
	tilemapCols = tilemap.getTilemapCols();

	cellsPositions = tilemap.getPositions();
	cellCounter = tilemap.getCellCounter();


	for (int i = 1; i < tilemapCols-1; i++)
	{
		for (unsigned int j = 1; j < tilemapRows-1; j++)
		{
			if (tilemapvec[j][i] == 1)
			{
				GRectangle rect_temp(glm::vec2(j, i), 1.0f, 1.0f);
				rectangles.push_back(rect_temp);
				//std::cout << "x " << -distanceToPlane + j << " z " << distanceToPlane - i << std::endl;
			}
		}
	}

	for (int i = 0; i < tilemapCols; i++)
	{
		for (unsigned int j = 0; j < tilemapRows; j++)
		{
			if (tilemapvec[j][i] == 1)
			{
				GRectangle rect_temp(glm::vec2(j, i), 1.0f, 1.0f);
				rectangles2.push_back(rect_temp);
			}
		}
	}

	//Init particles
	particles = GenerateParticles(numberOfParticles);

	//Pathfinding
	pathfinding = new Pathfinding();
	int maxPos = 18;
	int minPos = 1;
	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		int x = std::floor(particles[i].getCurrentPosition().x);
		int z = std::floor(particles[i].getCurrentPosition().z);

		int posX = minPos + rand() % ((maxPos + 1) - minPos);
		int posZ = minPos + rand() % ((maxPos + 1) - minPos);

		while (tilemapvec[posX][abs(posZ)] == 1)
		{
			posX = minPos + rand() % ((maxPos + 1) - minPos);
			posZ = minPos + rand() % ((maxPos + 1) - minPos);
		}

		pathfinding->init(tilemapvec, x, z, posX, posZ);
		pathfinding->computePathFinding();
		std::vector<node> path1 = pathfinding->getPath();
		particlesPath.push_back(path1);
		currentPathPos.push_back(0);
		FollowPath(i, 1.0f);
	}

	//Planes
	int newDist = (int)(distanceToPlane - 1);
	planes = InitializePlanes(newDist);

	oldDistances.resize(numberOfParticles * sizeof(float));
	newDistances.resize(numberOfParticles * sizeof(float));

	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		oldDistances[i].resize(6 * sizeof(float));
		newDistances[i].resize(6 * sizeof(float));


		for (unsigned int j = 0; j < planes.size(); j++)
		{
			newDistances[i][j] = planes[j].distPoint2Plane(particles[i].getCurrentPosition());
		}

		particlesColor.push_back(glm::vec3(rand01(), rand01(), rand01()));
	}

	// load 'cally' model
	std::cout << "Loading 'cally' model ..." << std::endl;

	//pModel = new Model();
	//m_strCal3D_Datapath = "Src/vendor";
	//m_strDatapath = "Res/data/";

	//if (m_strCal3D_Datapath != "")
	//	pModel->setPath("Res/data/cally/");
	///*pModel->setPath(m_strCal3D_Datapath + "/" + "cally/");*/

	//if (!pModel->onInit(m_strDatapath + "cally.cfg"))
	//{
	//	delete pModel;
	//	std::cerr << "Model initialization failed! (cally)" << std::endl;
	//	//return false;
	//}

	//m_vectorModel.push_back(pModel);

	//m_vectorModel[0]->executeAction(0);
	//m_vectorModel[0]->onUpdate(0.1);

	//m_vectorModel[0]->initModelRendering();


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
	computeSimulation();  //Particle Simulation
	
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 proj = camera.getProjectionMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	
	glm::mat4 modelview = view * model;

	for (int i = 0; i < tilemapCols; i++)
	{
		for (unsigned int j = 0; j < tilemapRows; j++)
		{
			model = glm::mat4(1.0f);

			modelview = view * model;
			
			if (tilemapvec[j][i] == 1)
			{
				modelview = glm::translate(modelview, glm::vec3(j, 0.0, i));
				SendUniforms(proj, modelview, glm::vec3(0.9f, 0.9f, 0.95f));
				wall->render();
			}
			else if (tilemapvec[j][i] == 0)
			{
				modelview = glm::translate(modelview, glm::vec3(j, 0.0, i));
				floortile->setUniforms(proj, modelview);
				floortile->DrawFloorTile();
			}
		}
	}

	//Path
	//for (unsigned int j = 0; j < particlesPath[0].size(); j++)
	//{
	//	model = glm::mat4(1.0f);
	//    modelview = view * model;

	//	modelview = glm::translate(modelview, glm::vec3(particlesPath[0][j].row, 0.0, particlesPath[0][j].col));
	//	SendUniforms(proj, modelview, glm::vec3(0.9f, 0.1f, 0.1f));
	//	wall->render();
	//}

	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		model = glm::mat4(1.0f);
		modelview = view * model;

		modelview = glm::translate(modelview, particles[i].getCurrentPosition()-glm::vec3(0.5, 0.0, 0.5));
		//modelview = glm::scale(modelview, glm::vec3(0.4f, 1.0f, 0.4f));
		//SendUniforms(proj, modelview, particlesColor[i]);
		//wall->render();

		modelview = glm::translate(modelview, glm::vec3(0.0f, 0.3f, 0.0f));
		modelview = glm::scale(modelview, glm::vec3(0.4f, 2.0f, 0.4f));
		SendUniforms(proj, modelview, particlesColor[i]);
		sphere->render();
		//m_vectorModel[0]->onRender();
		
	}

	text.render(std::to_string(fps), glm::vec2(20, 20), 16, glm::vec4(0, 0, 0, 1));
	//text.render("Mode: Mean Vertex ", glm::vec2(20, 40), 16, glm::vec4(0, 0, 0, 1));

	switch (tilemapUsed)
	{
	case 0:
		text.render("Mode: Tilemap 1 ", glm::vec2(20, 40), 16, glm::vec4(0, 0, 0, 1));
		break;

	case 1:
		text.render("Mode: Tilemap 2 ", glm::vec2(20, 40), 16, glm::vec4(0, 0, 0, 1));
		break;

	case 2:
		text.render("Mode: Tilemap 3 ", glm::vec2(20, 40), 16, glm::vec4(0, 0, 0, 1));
		break;

	default:
		text.render("Mode: Tilemap 1 ", glm::vec2(20, 40), 16, glm::vec4(0, 0, 0, 1));
		break;
	}
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

void Scene::reloadScene(int tilemapToLoad)
{
	Tilemap tilemap;

	switch (tilemapToLoad)
	{
	case 0:
		tilemap.init("Res/Tilemap/Tilemap.txt");
		tilemapUsed = 0;
		break;

	case 1:
		tilemap.init("Res/Tilemap/Tilemap2.txt");
		tilemapUsed = 1;
		break;

	case 2:
		tilemap.init("Res/Tilemap/Tilemap3.txt");
		tilemapUsed = 2;
		break;

	default:
		tilemap.init("Res/Tilemap/Tilemap.txt");
		tilemapUsed = 0;
		break;
	}


	tilemapvec = tilemap.getTilemap();
	tilemapRows = tilemap.getTilemapRows();
	tilemapCols = tilemap.getTilemapCols();

	cellsPositions = tilemap.getPositions();
	cellCounter = tilemap.getCellCounter();

	//Particles
	particles = GenerateParticles(numberOfParticles);

	//Pathfinding
	pathfinding = new Pathfinding();
	particlesPath.clear();
	currentPathPos.clear();
	int maxPos = 18;
	int minPos = 1;
	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		int x = std::floor(particles[i].getCurrentPosition().x);
		int z = std::floor(particles[i].getCurrentPosition().z);

		int posX = minPos + rand() % ((maxPos + 1) - minPos);
		int posZ = minPos + rand() % ((maxPos + 1) - minPos);

		while (tilemapvec[posX][abs(posZ)] == 1)
		{
			posX = minPos + rand() % ((maxPos + 1) - minPos);
			posZ = minPos + rand() % ((maxPos + 1) - minPos);
		}

		//std::cout << "Init" << x << " " << z << std::endl;
		//std::cout << "End" << posX << " " << posZ << std::endl;

		pathfinding->init(tilemapvec, x, z, posX, posZ);
		pathfinding->computePathFinding();
		std::vector<node> path1 = pathfinding->getPath();
		particlesPath.push_back(path1);
		currentPathPos.push_back(0);
		FollowPath(i, 1.0f);
	}

	rectangles.clear();
	for (int i = 1; i < tilemapCols - 1; i++)
	{
		for (unsigned int j = 1; j < tilemapRows - 1; j++)
		{
			if (tilemapvec[j][i] == 1)
			{
				GRectangle rect_temp(glm::vec2(j, i), 1.0f, 1.0f);
				rectangles.push_back(rect_temp);
			}
		}
	}

	rectangles2.clear();
	for (int i = 0; i < tilemapCols; i++)
	{
		for (unsigned int j = 0; j < tilemapRows; j++)
		{
			if (tilemapvec[j][i] == 1)
			{
				GRectangle rect_temp(glm::vec2(j, i), 1.0f, 1.0f);
				rectangles2.push_back(rect_temp);
			}
		}
	}

	oldDistances.resize(numberOfParticles * sizeof(float));
	newDistances.resize(numberOfParticles * sizeof(float));

	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		oldDistances[i].resize(6 * sizeof(float));
		newDistances[i].resize(6 * sizeof(float));


		for (unsigned int j = 0; j < planes.size(); j++)
		{
			newDistances[i][j] = planes[j].distPoint2Plane(particles[i].getCurrentPosition());
		}

		particlesColor.push_back(glm::vec3(rand01(), rand01(), rand01()));
	}


}


Particle Scene::InitializeParticle(glm::vec3 pos, glm::vec3 vel)
{
	Particle p(0.0, 0.0, 0.0);

	p.setPosition(pos.x, pos.y, pos.z);
	p.setVelocity(vel.x, vel.y, vel.z);

	p.setLifetime(5.0f * rand01());
	p.setBouncing(0.8f); //0.5f
	//p.addForce(0.0f, -9.8f, 0.0f);

	return p;
}

std::vector<Particle> Scene::GenerateParticles(int numberOfParticles)
{
	std::vector<Particle> particles;

	int minPos = 1;
	int maxPos = 18;

	float maxVel = 5.0f;

	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		int posX = minPos + rand() % ((maxPos + 1) - minPos);
		int posZ = minPos + rand() % ((maxPos + 1) - minPos);

		while (tilemapvec[posX][abs(posZ)] == 1)
		{
			posX = minPos + rand() % ((maxPos + 1) - minPos);
			posZ = minPos + rand() % ((maxPos + 1) - minPos);
		}

		glm::vec3 position(posX+0.5f, 0.0f, posZ+0.5f);
		glm::vec3 velocity(2.0f * (maxVel * rand01() - (maxVel / 2.0f)), 0.0f, 2.0f * (maxVel * rand01() - (maxVel / 2.0f)));
		particles.push_back(InitializeParticle(position, velocity));

	}

	//Check same cell
	for (unsigned int i = 0; i < numberOfParticles; i++)
	{
		for (unsigned int j = 0; j < numberOfParticles; j++)
		{
			if (i != j)
			{
				if (particles[i].getCurrentPosition().x == particles[j].getCurrentPosition().x &&
					particles[i].getCurrentPosition().z == particles[j].getCurrentPosition().z)
				{
					int posX = minPos + rand() % ((maxPos + 1) - minPos);
					int posZ = minPos + rand() % ((maxPos + 1) - minPos);

					while (tilemapvec[posX][abs(posZ)] == 1)
					{
						posX = minPos + rand() % ((maxPos + 1) - minPos);
						posZ = minPos + rand() % ((maxPos + 1) - minPos);
					}

					particles[i].setPosition(posX + 0.5f, 0.0f, posZ + 0.5f);
				}
			}
		}
	}

	return particles;
}

std::vector<Plane> Scene::InitializePlanes(float dist)
{
	std::vector<Plane> planes_temp;

	planes_temp.push_back(Plane(glm::vec3(-dist+1), glm::vec3(0, 1, 0)));
	planes_temp.push_back(Plane(glm::vec3(dist), glm::vec3(0, -1, 0)));
	planes_temp.push_back(Plane(glm::vec3(18.5), glm::vec3(-1, 0, 0)));
	planes_temp.push_back(Plane(glm::vec3(1.5), glm::vec3(1, 0, 0)));
	planes_temp.push_back(Plane(glm::vec3(18.5), glm::vec3(0, 0, -1)));
	planes_temp.push_back(Plane(glm::vec3(1.5), glm::vec3(0, 0, 1)));

	return planes_temp;
}

void Scene::FollowPath(int index, float partVelocity)
{
	//Last Cell
	if(glm::distance(particles[index].getCurrentPosition(), pathfinding->getCellWaypoint(particlesPath[index][particlesPath[index].size() - 1])) <= 0.1f)
	{
		//std::cout << pathfinding->getCellWaypoint(particlesPath[index][particlesPath[index].size() - 1]).x << std::endl;

		if(particles[index].getVelocity() != glm::vec3(0.0f)) particles[index].setVelocity(glm::vec3(0.0f));

		int x = std::floor(particles[index].getCurrentPosition().x);
		int z = std::floor(particles[index].getCurrentPosition().z);

		float xf = 16.0 * rand01() + 2.0f;
		float zf = 16.0 * rand01() + 2.0f;
		int xf_int = std::floor(xf);
		int zf_int = std::floor(zf);

		while (tilemapvec[xf_int][abs(zf_int)] == 1)
		{
			xf = 16.0f * rand01() + 2.0f;
			zf = 16.0f * rand01() + 2.0f;

			xf_int = std::floor(xf);
			zf_int = std::floor(xf);
		}
		//std::cout << " x " << x << " z " << z << std::endl;
		//std::cout << " xf " << xf_int << " zf " << zf_int << std::endl;

		pathfinding->init(tilemapvec, x, z, xf_int, zf_int);
		pathfinding->computePathFinding();
		std::vector<node> path1 = pathfinding->getPath();
		particlesPath[index] = path1;
		currentPathPos[index] = 0;
		return;
	}
		
	//Next Cell
	float delta = 0.01f;
	if (glm::distance(particles[index].getCurrentPosition(), pathfinding->getCellWaypoint(particlesPath[index][currentPathPos[index]])) <= 0.05f)
	{		
		currentPathPos[index] += 1;
		//std::cout << "Next Cell"<< currentPathPos[index] << std::endl;
	}

	//Compute velocity
	glm::vec3 nextPos = pathfinding->getCellWaypoint(particlesPath[index][currentPathPos[index]]);
	glm::vec3 velocity = nextPos - particles[index].getCurrentPosition();
	velocity /= glm::length(velocity);


	float velocityFactor = 2.0f;//rand01() + 1.5f;
	//velocity *= partVelocity;
	velocity *= velocityFactor;

	particles[index].setVelocity(velocity);
}

void Scene::computeSimulation()
{
	for (unsigned int i = 0; i < numberOfParticles; i++)
	{		
		FollowPath(i, 2.0f);	

		physics.ComputeObsAvoidance(particles, particles[i], rectangles2, i, colRadius, sqrt(0.51f), 1.0f, 1.0f);
		//physics.ComputeObsAvoidanceUnaligned(particles, particles[i], rectangles2, i, colRadius, sqrt(0.51f), 1.0f, 1.0f);

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

		//Particle Collision
		for (unsigned int j = 0; j < particles.size(); j++)
		{
			if (i != j)
			{
				Sphere sphere(particles[j].getCurrentPosition(), 2.0f * colRadius);
				physics.GetSpehereCollision(particles[i], sphere, frictionCoef, solverUsed);
				Sphere sphere2(particles[i].getCurrentPosition(), 2.0f * colRadius);
				physics.GetSpehereCollision(particles[j], sphere2, frictionCoef, solverUsed);
				//physics.GetSphereSphereCollision(particles[i], particles[j], sphere, frictionCoef, solverUsed);
			}
		}

		//Exterior Walls Collision
		physics.GetPlaneCollisions(particles[i], planes, oldDistances[i], newDistances[i], frictionCoef, solverUsed);

		//Interior Walls Collision
		for (unsigned int j = 0; j < rectangles.size(); j++)
		{
			physics.GetRectangleCircleColision(particles[i], rectangles[j], colRadius, frictionCoef, solverUsed);
		}
		
		particles[i].setLifetime(particles[i].getLifetime() - dt);
	}
}






