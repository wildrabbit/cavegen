#include "drunkardWalkGenerator.h"
#include "imgui.h"
#include <random>
//----------------------------

DrunkardWalkGenerator::DrunkardWalkGenerator()
	:r()
	, engine(r())
{
}
void DrunkardWalkGenerator::init(const DrunkardWalkConfig& _config)
{
	config = _config;
}
void DrunkardWalkGenerator::renderGUI(Game* game)
{
	ImGui::InputFloat("Expected empty ratio (0..1)", &config.expectedRatio);
}
void DrunkardWalkGenerator::start(Map* map)
{
	for (int i = 0; i < map->rows; ++i)
	{
		for (int j = 0; j < map->cols; ++j)
		{
			(*map)(i, j) = CellType::Wall;
		}
	}

	std::uniform_int_distribution<int> rowDist(0, map->rows - 1);
	std::uniform_int_distribution<int> colDist(0, map->cols - 1);

	row = rowDist(engine);
	col = colDist(engine);
	(*map)(row, col) = CellType::Empty;

}

void DrunkardWalkGenerator::step(Map* map)
{
	static const int rowIndexes[] = { 0, 0, 1, -1 };
	static const int colIndexes[] = { 1, -1, 0, 0 };

	std::uniform_int_distribution<int> direction(0, 3);
	int dir = -1;
	do
	{
		dir = direction(engine);
	} while (dir < 0 || !map->validCoords(row + rowIndexes[dir], col + colIndexes[dir]));

	row += rowIndexes[dir];
	col += colIndexes[dir];
	(*map)(row, col) = CellType::Empty;
}
void DrunkardWalkGenerator::generate(Map* map)
{

	int countFloor = std::count(map->cells.begin(), map->cells.end(), CellType::Empty);
	float ratio = countFloor / (float)map->numCells();

	while (ratio < config.expectedRatio)
	{
		step(map);

		countFloor = std::count(map->cells.begin(), map->cells.end(), CellType::Empty);
		ratio = countFloor / (float)map->numCells();
	}
}

