#include "Pathfinding.h"
#include <iostream>

void Pathfinding::init(const std::vector<std::vector<int>>& tilemap, const int& rowStart, const int& colStart, const int& rowEnd, const int& colEnd)
{
	_tilemap = tilemap;
	startNode.row = rowStart;
	startNode.col = colStart;
	startNode.idx = rowStart * tilemap[0].size() + colStart;
	endNode.row = rowEnd;
	endNode.col = colEnd;
	endNode.idx = rowEnd * tilemap[0].size() + colEnd;

	startNode.gCost = computeGCost(startNode);
	startNode.fCost = startNode.gCost + computeMovCost(startNode, endNode);

	openQ = new std::priority_queue<node>();
	cameFrom = new std::map<int, node>();
	cameFrom->clear();

	closedQ.clear();
	openVQ.clear();

	for (int i = 0; i < tilemap[0].size(); i++)
	{
		for (unsigned int j = 0; j < tilemap.size(); j++)
		{
			closedQ.push_back(0);
			openVQ.push_back(0);
		}

	}
}

void Pathfinding::computePathFinding()
{
	//std::cout << "Computing path...." << std::endl;
	openQ->push(startNode);
	node current;

	while (openQ->top() != endNode)
	{
		//Set current
		current = openQ->top();
		openQ->pop();
		closedQ[current.idx] == 1;		

		//Iterate through neighbours
		std::vector<node> neigh = getNeighbours(current);
		for (unsigned int i = 0; i < neigh.size(); i++)
		{
			if (closedQ[neigh[i].idx] == 1) continue; //If neigh in closed skip it	
			float newMov = current.gCost + computeMovCost(current, neigh[i]);
			if (newMov < neigh[i].gCost || openVQ[neigh[i].idx] == 0) //If cost is less or it is not in open
			{
				neigh[i].gCost = newMov;
				neigh[i].hCost = computeMovCost(neigh[i], endNode);
				//neigh[i].hCost = computeHCost(neigh[i]);
				neigh[i].fCost = neigh[i].gCost + neigh[i].hCost;

				cameFrom->insert(std::pair<int, node>(neigh[i].idx, current));
				if(openVQ[neigh[i].idx] == 0) openVQ[neigh[i].idx] = 1;
				openQ->push(neigh[i]);
			}
		}
	}
	//Recreate Path
	createPath(endNode);
}


void Pathfinding::createPath(const node& current)
{
	path.clear();
	node currentNode = current;
	//std::cout << "Creating Path...." << std::endl;

	while (currentNode != startNode)
	{
		//std::cout << currentNode.row << " " << currentNode.col << std::endl;
		path.push_back(currentNode);
		currentNode = cameFrom->find(currentNode.idx)->second;
	}

	//std::cout << currentNode.row << " " << currentNode.col << std::endl;
	path.push_back(currentNode);

	//reverse path
	//std::cout << "Reversing!" << std::endl;
	std::vector<node> path_temp;
	for (unsigned int i = 0; i < path.size(); i++)
	{
		path_temp.push_back(path[(path.size()-1) - i]);
		//std::cout << path[(path.size() - 1) - i].row << " " << path[(path.size() - 1) - i].col << std::endl;
	}
	path.clear();
	path = path_temp;
}

std::vector<node> Pathfinding::getPath()
{
	return path;
}

glm::vec3 Pathfinding::getCellWaypoint(const node& node)
{
	glm::vec3 waypoint = glm::vec3(node.row + 0.5f, 0.0f, node.col + 0.5f);

	return waypoint;
}

std::vector<node> Pathfinding::getNeighbours(const node& currNode)
{
	std::vector<node> neigh;

	neigh.clear();

	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (i == 0 && j == 0) continue;

			if (_tilemap[currNode.row + i][currNode.col + j] == 0)
			{
				node newNode;
				newNode.row = currNode.row + i;
				newNode.col = currNode.col + j;
				newNode.idx = newNode.row * _tilemap[0].size() + newNode.col;

				neigh.push_back(newNode);
			}
		}
	}	

	if (neigh.size() == 0) std::cout << "No neighbour for node: row " << currNode.row << " col " << currNode.col << std::endl;

	return neigh;
	/*if (_tilemap[currNode.row + 1][currNode.col] == 0)
	{		
		newNode.row = currNode.row + 1;
		newNode.col = currNode.col;
		newNode.gCost = computeGCost(newNode);
		newNode.hCost = computeHCost(newNode);
		newNode.fCost = newNode.gCost + newNode.hCost;
		newNode.idx = newNode.row * _tilemap[0].size() + newNode.col;

		neigh.push_back(newNode);
	}

	if (_tilemap[currNode.row - 1][currNode.col] == 0)
	{
		newNode.row = currNode.row - 1;
		newNode.col = currNode.col;
		newNode.gCost = computeGCost(newNode);
		newNode.hCost = computeHCost(newNode);
		newNode.fCost = newNode.gCost + newNode.hCost;
		newNode.idx = newNode.row * _tilemap[0].size() + newNode.col;

		neigh.push_back(newNode);
	}
	
	if (_tilemap[currNode.row][currNode.col + 1] == 0)
	{
		newNode.row = currNode.row;
		newNode.col = currNode.col + 1;
		newNode.gCost = computeGCost(newNode);
		newNode.hCost = computeHCost(newNode);
		newNode.fCost = newNode.gCost + newNode.hCost;
		newNode.idx = newNode.row * _tilemap[0].size() + newNode.col;

		neigh.push_back(newNode);
	}

	if (_tilemap[currNode.row][currNode.col - 1] == 0)
	{
		newNode.row = currNode.row;
		newNode.col = currNode.col - 1;
		newNode.gCost = computeGCost(newNode);
		newNode.hCost = computeHCost(newNode);
		newNode.fCost = newNode.gCost + newNode.hCost;
		newNode.idx = newNode.row * _tilemap[0].size() + newNode.col;

		neigh.push_back(newNode);
	}
*/
}

float Pathfinding::computeHCost(const node& node)
{
	//Cost computed without diagonals
	//float HCost = abs(endNode.row - node.row) + abs(endNode.col - node.col);
	float HCost = sqrt(pow((endNode.row - node.row),2) + pow((endNode.col - node.col),2));
	return HCost;
}

float Pathfinding::computeGCost(const node& node)
{
	//Cost computed without diagonals
	//float GCost = abs(startNode.row - node.row) + abs(startNode.col - node.col);
	float GCost = sqrt(pow((startNode.row - node.row), 2) + pow((startNode.col -node.col), 2));
	return GCost;
}

float Pathfinding::computeMovCost(const node& node1, const node& node2)
{
	//float costMove = abs(node1.row - node2.row) + abs(node1.col - node2.col);
	//float costMove = sqrt(pow((node2.row - node1.row), 2) + pow((node2.col - node1.col), 2));
	//return costMove;

	if (node2.row != node1.row && node2.col != node1.col) return sqrt(2.0f);
	else return 1.0f;

	//float distX = abs(node2.row - node1.row);
	//float distY = abs(node2.col - node1.col);

	//if (distX > distY) return 1.4f * distY + (distX - distY);
	//else 1.4f * distX + (distY - distX);
}

