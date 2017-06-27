#include "generators.h"
#include "game.h"
#include "imgui.h"
#include "imgui-sfml/imgui-sfml.h"

void CellAutomataGenerator::init(const CellAutomataConfig& _config)
{
	config = _config;
	config.checkFunction = std::bind(&CellAutomataGenerator::basicEvaluation, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
}

void CellAutomataGenerator::start(Map* map)
{
	noise(map);
}

void CellAutomataGenerator::generate(Map* map)
{
	step(map);
}

void CellAutomataGenerator::renderGUI(Game* game)
{
	const float SPINNER_DELTA = 0.05f;
	const float FAST_SPINNER_DELTA = 0.1f;

	ImGui::InputFloat("Initial wall probability [0..1]", &config.wallProbability, SPINNER_DELTA, FAST_SPINNER_DELTA);
	ImGui::InputInt("Neighbouring walls required for next iteration", &config.minSurroundingWallsForNextIter);
	ImGui::Checkbox("Count current cell?", &config.includeSelf);
	ImGui::Checkbox("Simulate borders?", &config.useCorners);
}

void CellAutomataGenerator::noise(Map* map)
{
	int numCells = map->numCells();
	std::random_device r;
	std::default_random_engine engine(r());
	std::uniform_real_distribution<float> dist(0, 1);

	for (int i = 0; i < map->rows; ++i)
	{
		for (int j = 0; j < map->cols; ++j)
		{
			bool corner = j == 0 || j == map->cols - 1 || i == 0 || i == map->rows - 1;
			if (!config.useCorners && corner)
			{
				(*map)(i, j) = CellType::Wall;
			}
			else
			{
				(*map)(i, j) = dist(engine) <= config.wallProbability ? CellType::Wall : CellType::Empty;
			}			
		}
	}
	
}

void CellAutomataGenerator::step(Map* map)
{
	std::vector<CellType> copy(map->cells);
	int startRow = config.useCorners ? 0 : 1;
	int endRow = config.useCorners ? map->rows : map->rows - 1;
	int startCol = config.useCorners ? 0 : 1;
	int endCol = config.useCorners ? map->cols : map->cols - 1;
	
	for (int i = startRow; i < endRow; ++i)
	{
		for (int j = startCol; j < endCol; ++j)
		{
			(*map)(i, j) = config.checkFunction(i, j, map, copy) ? CellType::Wall : CellType::Empty;
		}
	}
}

bool CellAutomataGenerator::basicEvaluation(int i, int j, const Map* map, const Map::CellArray& oldMapCells)
{
	int numWalls = countNeighbourhood(i, j, map, oldMapCells, 1, config.includeSelf);
	return numWalls >= config.minSurroundingWallsForNextIter;
}

int CellAutomataGenerator::countNeighbourhood(int i, int j, const Map* map, const Map::CellArray& oldCells, int distance, bool countSelf)
{
	int numWalls = 0;
	for (int deltaRow = -1; deltaRow <= 1; ++deltaRow)
	{
		for (int deltaCol = -1; deltaCol <= 1; ++deltaCol)
		{
			if (!map->validCoords(i + deltaRow, j + deltaCol)) continue;
			if (deltaRow == 0 && deltaCol == 0 && !countSelf) continue;
			if (oldCells[map->asIndex(i + deltaRow, j + deltaCol)] == CellType::Wall)
			{
				numWalls++;
			}
		}
	}
	return numWalls;
}

