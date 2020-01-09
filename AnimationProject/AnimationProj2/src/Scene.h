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
#include "Model.h"
#include "Pathfinding.h"

#include <glm/glm.hpp>
#include "cal3d/cal3d.h"

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
	void reloadScene(int tilemapToLoad);

  Camera &getCamera();
  
  void switchPolygonMode();

private:
	void initShaders(const std::string &vpath, const std::string &fpath);
	void SendUniforms(glm::mat4& proj, glm::mat4& modelview, glm::vec3 color);

	//Functions
	std::vector<Particle> GenerateParticles(int numberOfParticles);
	Particle InitializeParticle(glm::vec3 pos, glm::vec3 vel);
	std::vector<Plane> InitializePlanes(float dist);
	void FollowPath(int index, float partVelocity);

	void computeSimulation();


private:
	Camera camera;
	TriangleMesh *mesh;
	TriangleMesh* wall;
	TriangleMesh* sphere;
	ShaderProgram basicProgram;
	float currentTime;
	
	bool bPolygonFill;

	Pathfinding *pathfinding;
	std::vector<node> path1;
	std::vector< std::vector<node>> particlesPath;
	std::vector<int> currentPathPos;
	
	Text text;
	bool modelMode;
	
	vector<MeshProp> meshesProp;

	unsigned int tilemapUsed;
	std::vector<std::vector<int>> tilemapvec;
	unsigned int tilemapRows, tilemapCols;
	std::vector<glm::vec3> cellsPositions;
	std::vector<std::vector<int>> cellCounter;

	std::vector<GRectangle> rectangles;
	std::vector<GRectangle> rectangles2;

	FloorTile *floortile;

	//Particles
	std::vector<Plane> planes;
	std::vector<Particle> particles;
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

	Model* pModel;
	std::string m_strDatapath;
	std::string m_strCal3D_Datapath;
	std::vector<Model*> m_vectorModel;
};


#endif // _SCENE_INCLUDE

