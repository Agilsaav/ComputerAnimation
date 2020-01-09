#include "LOD.h"

bool CellPos::operator<(const CellPos& cellPos) const
{
	return x < cellPos.x || (x ==cellPos.x && y < cellPos.y) || (x == cellPos.x && y == cellPos.y && z<cellPos.z);
}

LOD::LOD()
{
}

//Function to initialize the LOD. It calculates the LOD and returns the new vertices and triangles:
void LOD::init(const int N, TriangleMesh& mesh, bool bQEM)
{
	//Initialize empty variables:
	newVertices = {};
	newTriangles = {};
	Positions = {};
	Cells = {};
	vertexMatrices = {};
	Qmatrices = {};
	grid.clear();

	//Select LOD:
	switch (N)
	{
		case 1:
			grid_x = grid_y = grid_z = 256;
			break;
		case 2:
			grid_x = grid_y = grid_z = 128;
			break;
		case 3:
			grid_x = grid_y = grid_z = 64;
			break;
		case 4:
			grid_x = grid_y = grid_z = 32;
			break;
	}

	//Get vertices and Triangles
	vertices = mesh.getVertices();
	triangles = mesh.getTriangles();

	//Boundig Box -> Cube (Triangle Mesh)
	bmin = glm::vec3(-1.0, -1.0, -1.0);
	bmax = glm::vec3(1.0, 1.0, 1.0);

	//LOD without QEM:
	if (!bQEM)
	{
		//Create LOD
		getverticesPositions();
		createCells();
		computeNewVertices();
		computeNewTriangles();

		//Send LOD
		mesh.applyLOD(newVertices, newTriangles);
	}
	//LOD with QEM:
	else
	{
		getverticesPositions();
		createCells();
		computeVertexMatrices();
		computeNewVerticesQEM();
		computeNewTriangles();

		//Send LOD
		mesh.applyLOD(newVertices, newTriangles);
	}

}

//Function to create the Cells once we know the position of each vertex
void LOD::createCells()
{
	count = 0;
	Cell cell_temp;

	Cells.resize(vertices.size());

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		if (grid.find(Positions[i]) != grid.end()) //Find if positions exists in the map
		{
			//If Positions[i] is found:

			cell_temp = grid[Positions[i]]; // Get Positions ID
			cell_temp.numpoints += 1; // Increase number of points
			cell_temp.postot += vertices[i]; // Increase total position
			cell_temp.VertexInitalPos.push_back(i); //Initial position of the vertex

			grid[Positions[i]] = cell_temp; //redefine
		}
		else
		{
			//If Positions[i] does not exist:

			Cells[i].numpoints = 1; //Initialize number of points
			Cells[i].postot = vertices[i];	//Sum vertices positions
			Cells[i].VertexInitalPos.push_back(i); //Initial position of the vertex
			grid.insert(std::pair<CellPos, Cell>(Positions[i], Cells[i])); //Insert to map

			count += 1;

		}
	}

	Cells.resize(count);
}

//Function to compute the new vertices of the LOD (Mean vertex):
void LOD::computeNewVertices()
{
	newVertices.resize(count); //Resize new vertices
	int i = 0;

	//Iterate for each key
	map<CellPos, Cell>::iterator it = grid.begin();
	while (it != grid.end())
	{
		if (it->second.numpoints > 1)
		{
			//Calculate mean vertex
			it->second.vertex = glm::vec3((it->second.postot.x / it->second.numpoints),
				(it->second.postot.y / it->second.numpoints),
				(it->second.postot.z / it->second.numpoints));

			newVertices[i] = (it->second.vertex);	//Add new vertex
			it->second.vOrder = i;
		}
		else
		{
			newVertices[i] = (it->second.postot);	//Add new vertex
			it->second.vOrder = i;
		}

		it++;
		i++;
	}

	cout << "New Vertices: " << newVertices.size() << endl;
}

//Function to compute the new triangles using the new vertices created
void LOD::computeNewTriangles()
{
	Cell cell_temp;
	int a,b,c;
	bool deg;

	for (unsigned int i = 0; i < triangles.size(); i+= 3)
	{
		cell_temp = grid[Positions[triangles[i]]];		//Cell for the First vertex
		a = cell_temp.vOrder;
		cell_temp = grid[Positions[triangles[i+1]]];	//Cell for the Second vertex
		b = cell_temp.vOrder;
		cell_temp = grid[Positions[triangles[i+2]]];	//Cell for the Third vertex
		c = cell_temp.vOrder;

		deg = degenerateTriangle(a, b, c);	//Check is the triangles is degenerate
		if (deg == false)
		{
			//If triangle non-degenerate -> Push into newTriangles 
			newTriangles.push_back(a);
			newTriangles.push_back(b);
			newTriangles.push_back(c);

		}

	}
	cout << "New Triangles: " << newTriangles.size() << endl;
	cout << "New Faces: " << newTriangles.size() / 3.0 << endl;
}

//Degenerate triangle function:
bool LOD::degenerateTriangle(int a, int b, int c)
{
	bool degenerate;

	if (a == b or b == c or a == c)
		degenerate = true;
	else
		degenerate = false;

	return degenerate;
}

//Function to create the Q Matrix given three vertices
glm::dmat4 LOD::createQMatrix(glm::vec3& vertex1, glm::vec3& vertex2, glm::vec3& vertex3)
{
	PlaneLOD plane;

	//Plane creation:
	glm::vec3 vec1 = vertex2 - vertex1;
	glm::vec3 vec2 = vertex3 - vertex1;

	glm::vec3 normal = glm::normalize(glm::cross(vec1, vec2));
	plane.a = (double)normal.x;
	plane.b = (double)normal.y;
	plane.c = (double)normal.z;

	plane.d = (double)(-plane.a * vertex1.x - plane.b * vertex1.y - plane.c * vertex1.z);

	//Matrix creation:
	glm::dmat4 Q;

	Q[0][0] = plane.a * plane.a;
	Q[1][1] = plane.b * plane.b;
	Q[2][2] = plane.c * plane.c;
	Q[3][3] = plane.d * plane.d;
	Q[1][0] = Q[0][1] = plane.a * plane.b;
	Q[2][0] = Q[0][2] = plane.a * plane.c;
	Q[3][0] = Q[0][3] = plane.a * plane.d;
	Q[1][2] = Q[2][1] = plane.b * plane.c;
	Q[1][3] = Q[3][1] = plane.b * plane.d;
	Q[2][3] = Q[3][2] = plane.c * plane.d;

	return Q;
}

/*Function to compute the Q matrix for each vertex:

First compute the Q matrix for each triangle and finally it sums all the matrices for each vertex
*/
void LOD::computeVertexMatrices()
{
	vertexMatrices.resize(vertices.size());
	glm::dmat4 mat_temp;

	//Compute matrices for all the triangles
	for (unsigned int i = 0; i < triangles.size(); i += 3)
	{
		mat_temp = createQMatrix(vertices[triangles[i]], vertices[triangles[i + 1]], vertices[triangles[i + 2]]);

		vertexMatrices[triangles[i]].push_back(mat_temp);
		vertexMatrices[triangles[i+1]].push_back(mat_temp);
		vertexMatrices[triangles[i+2]].push_back(mat_temp);
	}

	Qmatrices.resize(vertexMatrices.size());

	//Sum the Matrices for each vertex
	for (unsigned int i = 0; i < vertexMatrices.size(); i++)
	{
		if (vertexMatrices[i].size() > 1)
		{
			for (unsigned int j = 0; j < vertexMatrices[i].size(); j++)
			{
				Qmatrices[i] += vertexMatrices[i][j];
			}
		}
		else
			Qmatrices[i] = vertexMatrices[i][0];

	}
}

/*Function to create the new vertices with QEM:

If the Determinant is close to 0 it uses the mean vertex otherwise it uses QEM
*/
void LOD::computeNewVerticesQEM()
{
	newVertices.resize(count); //Resize new vertices
	int i = 0;

	//Iterate for each key
	map<CellPos, Cell>::iterator it = grid.begin();
	while (it != grid.end())
	{
		if (it->second.numpoints > 1)
		{
			glm::dmat4 Q_temp;

			//Sum all the matrices of the cell
			for (int i = 0; i < it->second.numpoints; i++)
			{
				if (i == 0)
					Q_temp = Qmatrices[it->second.VertexInitalPos[i]];
				else
					Q_temp += Qmatrices[it->second.VertexInitalPos[i]];
			}

			Q_temp[0][3] = Q_temp[1][3] = Q_temp[2][3] = 0.0;
			Q_temp[3][3] = 1.0;

			//Caclulate new Vertex
			if (abs(glm::determinant(Q_temp)) > 0.01)
			{
				it->second.vertex = glm::inverse(Q_temp) * glm::vec4(0.0, 0.0, 0.0, 1.0);
			}
			else
			{
				//Mean Vertex
				it->second.vertex = glm::vec3((it->second.postot.x / it->second.numpoints),
					(it->second.postot.y / it->second.numpoints),
					(it->second.postot.z / it->second.numpoints));
			}

			newVertices[i] = (it->second.vertex);	//Add new vertex
			it->second.vOrder = i;
		}
		else
		{
			newVertices[i] = (it->second.postot);	//Add new vertex
			it->second.vOrder = i;
		}

		it++;
		i++;
	}

	cout << "New Vertices: " << newVertices.size() << endl;
}

//Get the position of the vertices in the Grid:
void LOD::getverticesPositions()
{
	Positions.resize(vertices.size());

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		Positions[i].x = floor((vertices[i].x - bmin.x) / (bmax.x - bmin.x) * grid_x);
		Positions[i].y = floor((vertices[i].y - bmin.y) / (bmax.y - bmin.y) * grid_y);
		Positions[i].z = floor((vertices[i].z - bmin.z) / (bmax.z - bmin.z) * grid_z);
	}
}


//Recalculate Bounding Box in case is not the max and min defined (Not Used in our case)
void LOD::createNewBB()
{
	glm::vec3 size = bmax - bmin;
	glm::vec3 center = glm::vec3((bmax.x - bmin.x) / 2.0,
		(bmax.y - bmin.y) / 2.0,
		(bmax.z - bmin.z) / 2.0);

	float S = max(size.x, max(size.y, size.z));

	bmin = center - glm::vec3(S / 2.0, S / 2.0, S / 2.0);
	bmax = center + glm::vec3(S / 2.0, S / 2.0, S / 2.0);
}

/*
void LOD::computePairs()
{
	//vertexNeighbors.resize(vertices.size());
	//for (unsigned int i = 0; i < triangles.size(); i += 3)
	//{
	//	//Vertex 1 Neighbors
	//	vertexNeighbors[triangles[i]].push_back(triangles[i+1]);
	//	vertexNeighbors[triangles[i]].push_back(triangles[i+2]);

	//	//Vertex 2 Neighbors
	//	vertexNeighbors[triangles[i+1]].push_back(triangles[i]);
	//	vertexNeighbors[triangles[i+1]].push_back(triangles[i + 2]);

	//	//Vertex 3 Neighbors
	//	vertexNeighbors[triangles[i+2]].push_back(triangles[i]);
	//	vertexNeighbors[triangles[i+2]].push_back(triangles[i + 1]);

	//	//Vertex Triangle Position
	//	vertexTriPos[triangles[i]].push_back(i);
	//	vertexTriPos[triangles[i+1]].push_back(i+1);
	//	vertexTriPos[triangles[i+2]].push_back(i+2);
	//}
}*/


