#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE



#include "Camera.h"
#include "ShaderProgram.h"
#include "TriangleMesh.h"
#include "Text.h"
#include "FloorTile.h"
#include "Particle.h"
#include "Geometry.h"
#include "Physics.h"

#include <glm/glm.hpp>


// Scene contains all the entities of our game.
// It is responsible for updating and render them.

struct MeshProp
{
	unsigned int ID; 
	float distance;
	unsigned int LOD;
	float Cost;
	float Benefit;
	float Value;

};


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	bool loadMesh(const char *filename);
	void update(int deltaTime);
	void render(double fps);

	void changeModelMode();

  Camera &getCamera();
  
  void switchPolygonMode();

private:
	void initShaders(const std::string &vpath, const std::string &fpath);
	void SendUniforms(glm::mat4& proj, glm::mat4& modelview, glm::vec3 color);

	//Functions
	std::vector<Particle> GenerateParticles(int numberOfParticles, int mode);
	Particle InitializeParticle(int mode);
	std::vector<Plane> InitializePlanes(float dist);

	void computeSimulation();


private:
	Camera camera;
	TriangleMesh *mesh;
	TriangleMesh* cube;
	TriangleMesh* wall;
	TriangleMesh* particleMesh;
	ShaderProgram basicProgram;
	float currentTime;
	
	bool bPolygonFill;
	
	Text text;
	bool modelMode;
	
	vector<MeshProp> meshesProp;

	FloorTile *floortile;

	//Particles
	std::vector<Plane> planes;
	std::vector<Particle> particles;
	//std::vector<Sphere> partiSpheres;
	std::vector<glm::vec3> particlesColor;
	std::vector< std::vector<float>> oldDistances;
	std::vector< std::vector<float>> newDistances;

	Physics physics;
	float dt;

	int numberOfParticles;
	int solverUsed;
	float colRadius;
	float distanceToPlane;
	float frictionCoef;
	float elasticCoef;
	float dampingCoef;

	float radiusAction;
	float viscFluid;
	float fluidDen;
	float Vol;
	float k;
	float particleMass;
	float particleSize;
	float surfaceTension;
	float surfaceTensionTresh;

};


#endif // _SCENE_INCLUDE

