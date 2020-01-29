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

	startNode.fCost = computeGCost(startNode) + computeHCost(startNode);

	openQ = new std::priority_queue<node>();
	cameFrom = new std::map<int, node>();
	cameFrom->clear();

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
			if (neigh[i].fCost < current.fCost || openVQ[neigh[i].idx] == 0) //If cost is less or it is not in open
			{
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
	std::cout << "creating Path!!!!!!!!!!!!!!!!!" << std::endl;

	while (currentNode != startNode)
	{
		std::cout << currentNode.row << " " << currentNode.col << std::endl;
		path.push_back(currentNode);
		currentNode = cameFrom->find(currentNode.idx)->second;
	}

	std::cout << currentNode.row << " " << currentNode.col << std::endl;
	path.push_back(currentNode);
}

std::vector<node> Pathfinding::getNeighbours(const node& currNode)
{
	std::vector<node> neigh;

	node newNode;
	if (_tilemap[currNode.row + 1][currNode.col] == 0)
	{		
		newNode.row = currNode.row + 1;
		newNode.col = currNode.col;
		newNode.fCost = computeGCost(newNode) + computeHCost(newNode);
		newNode.idx = newNode.row * _tilemap[0].size() + newNode.col;

		neigh.push_back(newNode);
	}

	if (_tilemap[currNode.row - 1][currNode.col] == 0)
	{
		newNode.row = currNode.row - 1;
		newNode.col = currNode.col;
		newNode.fCost = computeGCost(newNode) + computeHCost(newNode);
		newNode.idx = newNode.row * _tilemap[0].size() + newNode.col;

		neigh.push_back(newNode);
	}
	
	if (_tilemap[currNode.row][currNode.col + 1] == 0)
	{
		newNode.row = currNode.row;
		newNode.col = currNode.col + 1;
		newNode.fCost = computeGCost(newNode) + computeHCost(newNode);
		newNode.idx = newNode.row * _tilemap[0].size() + newNode.col;

		neigh.push_back(newNode);
	}

	if (_tilemap[currNode.row][currNode.col - 1] == 0)
	{
		newNode.row = currNode.row;
		newNode.col = currNode.col - 1;
		newNode.fCost = computeGCost(newNode) + computeHCost(newNode);
		newNode.idx = newNode.row * _tilemap[0].size() + newNode.col;

		neigh.push_back(newNode);
	}


	return neigh;
}

float Pathfinding::computeHCost(const node& node)
{
	//Cost computed without diagonals
	float HCost = abs(endNode.row - node.row) + abs(endNode.col - node.col);
	//float HCost = sqrt(pow((_rowEnd - row),2) + pow((_colEnd - col),2));
	return HCost;
}

float Pathfinding::computeGCost(const node& node)
{
	//Cost computed without diagonals
	float GCost = abs(startNode.row - node.row) + abs(startNode.col - node.col);
	//float GCost = sqrt(pow((startNode.row - node.row), 2) + pow((startNode.col -node.col), 2));
	return GCost;
}

float Pathfinding::computeMovCost(const node& node1, const node& node2)
{
	float costMove = abs(node1.row - node2.row) + abs(node1.col - node2.col);
	return costMove;
}
