#pragma once

#include <string>
#include <vector>
#include <queue>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct node
{
	int row, col;
	float fCost, gCost, hCost;
	int idx;
	int operator<(const node& other) const { return fCost > other.fCost; };

	bool operator !=(const node& other) const {
		return row != other.row || col != other.col;
	}

	bool operator ==(const node& other) const {
		return row == other.row && col == other.col;
	}
};

class Pathfinding
{
public:
	Pathfinding() {};
	~Pathfinding() {};

	void init(const std::vector<std::vector<int>>& tilemap, const int& rowStart, const int& colStart, const int& rowEnd, const int& colEnd);
	void computePathFinding();
	std::vector<node> getPath();
	glm::vec3 getCellWaypoint(const node& node);

private:
	std::vector<node> getNeighbours(const node& node);
	float computeHCost(const node& node);
	float computeGCost(const node& node);
	float computeMovCost(const node& node1, const node& node2);
	void createPath(const node& current);
	

	//Var:
	std::vector<std::vector<int>> _tilemap;
	std::priority_queue<node> *openQ;
	std::vector<int> closedQ, openVQ;
	std::map<int, node>* cameFrom;
	std::vector<node> path;
	node startNode, endNode;
};