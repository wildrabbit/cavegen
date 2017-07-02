#include "drunkardWalkGenerator.h"
#include "imgui.h"
#include <random>
//----------------------------

DrunkardWalkGenerator::DrunkardWalkGenerator()
	:r()
	, engine(r())
	, lastStepCount(0)
	, lastEmptyRatio(0.f)
	, numTilesCarved(0)
	, mapSize(0)
{
}
void DrunkardWalkGenerator::init(const DrunkardWalkConfig& _config)
{
	config = _config;
}
void DrunkardWalkGenerator::renderGUI(Game* game)
{
	ImGui::InputFloat("Expected empty ratio (0..1)", &config.expectedRatio);
	ImGui::Checkbox("Stop when ratio is met", &stopWhenRatioIsMet);
	renderInfoGUI(game);
}

void DrunkardWalkGenerator::renderInfoGUI(Game* game)
{
	char fillRatio[64];
	snprintf(fillRatio, 64, "Last empty ratio: %.03f, expected: %.02f", lastEmptyRatio, config.expectedRatio);
	ImGui::LabelText("", fillRatio);

	char stepCountBuf[64];
	snprintf(stepCountBuf, 64, "Last step count: %d", lastStepCount);
	ImGui::LabelText("", stepCountBuf);
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

	lastStepCount = 0;
	lastEmptyRatio = 0.f;
	numTilesCarved = 0;
	mapSize = map->numCells();

	std::uniform_int_distribution<int> rowDist(0, map->rows - 1);
	std::uniform_int_distribution<int> colDist(0, map->cols - 1);

	row = rowDist(engine);
	col = colDist(engine);
	(*map)(row, col) = CellType::Empty;
	numTilesCarved++;
	lastEmptyRatio = numTilesCarved / (float)mapSize;
}

bool DrunkardWalkGenerator::filledRatio(Map* map) const
{
	return lastEmptyRatio >= config.expectedRatio;
}

void DrunkardWalkGenerator::step(Map* map)
{
	static const int rowIndexes[] = { 0, 0, 1, -1 };
	static const int colIndexes[] = { 1, -1, 0, 0 };

	lastStepCount++; 

	std::uniform_int_distribution<int> direction(0, 3);
	int dir = -1;
	do
	{
		dir = direction(engine);
	} while (dir < 0 || !map->validCoords(row + rowIndexes[dir], col + colIndexes[dir]));

	row += rowIndexes[dir];
	col += colIndexes[dir];
	CellType& cell = (*map)(row, col);
	
	if (cell != CellType::Empty)
	{
		numTilesCarved++;
		lastEmptyRatio = numTilesCarved / (float)mapSize;
	}
	(*map)(row, col) = CellType::Empty;
}
void DrunkardWalkGenerator::generate(Map* map)
{

	lastEmptyRatio = numTilesCarved / (float)mapSize;

	while (lastEmptyRatio < config.expectedRatio)
	{
		step(map);
	}
}

