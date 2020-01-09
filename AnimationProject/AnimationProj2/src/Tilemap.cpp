#include "Tilemap.h"

#include <iostream>
#include <fstream>
#include <sstream>

/*Tilemap file reader.

The structure to read the tilemap is prepared to read only 1 number and if it finds an space character it transform it to 9.

Using the second map prepared this will not affect because there is no spaces between characters.
*/
Tilemap::Tilemap()
{
}

Tilemap::~Tilemap()
{
}

void Tilemap::init(const std::string& filepath)
{
	cols = 0;
	rows = 0;

	std::ifstream stream(filepath);
	std::string line;

	while (std::getline(stream, line))
	{
		std::vector<int> tempv;
		std::string dummy;
		for (int i = 0; i < line.size(); i++)
		{
			//If the character is not an space transform it to integer and push it back to the vector
			if (line[i] != ' ')
			{
				dummy = line[i];
				tempv.push_back(std::atoi(dummy.c_str()));
			}
		}

		tilemap.push_back(tempv);
	}

	rows = tilemap.size();
	cols = tilemap[0].size();

	int count = 0;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	unsigned int countCell = 0;
	std::vector<int> vec_temp;

	for (const std::vector<int>& v : tilemap)
	{
		count = 0;
		vec_temp = {};
		for (int x : v)
		{
			if (x != 0)
			{
				numObstacles += 1; //Num of models
				positions.push_back(position); //Position of each cell
			}

			vec_temp.push_back(countCell); //Numvber of cell
			countCell++;

			position += glm::vec3(0.0, 0.0, -1.0);
			count += 1;
		}
		position += glm::vec3(1.0, 0.0, 1.0 * count);
		cellCounter.push_back(vec_temp);
	}
}

std::vector<std::vector<int>> Tilemap::getTilemap()
{
	return tilemap;
}

unsigned int& Tilemap::getObstacles()
{
	return numObstacles;
}

unsigned int& Tilemap::getTilemapRows()
{
	return rows;
}

unsigned int& Tilemap::getTilemapCols()
{
	return cols;
}


std::vector<glm::vec3>& Tilemap::getPositions()
{
	return positions;
}

std::vector<std::vector<int>>& Tilemap::getCellCounter()
{
	return cellCounter;
}
