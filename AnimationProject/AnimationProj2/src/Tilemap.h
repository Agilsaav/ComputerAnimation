#include <string>
#include <vector>

#include <glm/glm.hpp>

class Tilemap
{
public:
	Tilemap();
	~Tilemap();

	void init(const std::string& filepath);

	std::vector<std::vector<int>> getTilemap();
	unsigned int& getObstacles();
	unsigned int& getTilemapRows();
	unsigned int& getTilemapCols();
	std::vector<glm::vec3>& getPositions();
	std::vector<std::vector<int>>& getCellCounter();

private:
	std::vector<std::vector<int>> tilemap;

	unsigned int numObstacles;
	unsigned int rows, cols;
	std::vector<glm::vec3> positions;
	std::vector<std::vector<int>> cellCounter;
};