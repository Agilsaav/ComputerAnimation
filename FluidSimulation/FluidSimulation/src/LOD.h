#pragma once

#include <map>
#include <algorithm>
#include <iostream>

#include "TriangleMesh.h"
#include <glm/glm.hpp>

struct CellPos
{
	int x;
	int y;
	int z;

	bool operator<(const CellPos &cellPos) const;
};

struct Cell
{
	glm::vec3 postot; //sum positions
	int numpoints; //num points

	glm::vec3 vertex; //Final vertex
	int vOrder;  //Vertex Order

	vector<int> VertexInitalPos; //Initial position of the vertex
};

struct PlaneLOD
{
	double a;
	double b;
	double c;
	double d;
};

class LOD
{
public:
	LOD();

	void init(const int N, TriangleMesh& mesh, bool bQEM);

private:
	std::map<CellPos, Cell> grid;
	vector<CellPos> Positions;
	vector<Cell> Cells;

	glm::vec3 bmin;
	glm::vec3 bmax;

	int grid_x, grid_y, grid_z;
	int count;

	vector<glm::vec3> vertices;
	vector<int> triangles;

	vector<glm::vec3> newVertices;
	vector<int> newTriangles;

	void getverticesPositions();
	void createNewBB();
	void createCells();
	void computeNewVertices();
	void computeNewTriangles();
	bool degenerateTriangle(int a, int b, int c);

	//QEM:
	vector<vector<glm::dmat4>> vertexMatrices;
	vector<glm::dmat4> Qmatrices;

	glm::dmat4 createQMatrix(glm::vec3& vertex1, glm::vec3& vertex2, glm::vec3& vertex3);
	void computeVertexMatrices();
	void computeNewVerticesQEM();
};