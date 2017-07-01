#include "generators.h"
#include "game.h"
#include "imgui.h"
#include "imgui-sfml/imgui-sfml.h"
#include <algorithm>

#include <iostream>

#define ARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*_ARR)))

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
	const char* options[] = { "Basic", "Extended" };
	if (ImGui::Combo("Algorithm", &config.functionIndex, options, ARRAYSIZE(options)))
	{
		config.checkFunction = (config.functionIndex == 0) 
			? std::bind(&CellAutomataGenerator::basicEvaluation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
			: std::bind(&CellAutomataGenerator::basicEvaluationClosed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	}
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

bool CellAutomataGenerator::basicEvaluationClosed(int i, int j, const Map* map, const Map::CellArray& oldCells)
{
	bool basic = basicEvaluation(i, j, map, oldCells);
	int numWalls = countNeighbourhood(i, j, map, oldCells, 2, config.includeSelf);
	return basic || numWalls <= 1;
}

int CellAutomataGenerator::countNeighbourhood(int i, int j, const Map* map, const Map::CellArray& oldCells, int distance, bool countSelf)
{
	int numWalls = 0;
	for (int deltaRow = -distance; deltaRow <= distance; ++deltaRow)
	{
		for (int deltaCol = -distance; deltaCol <= distance; ++deltaCol)
		{
			if (distance == 2 && (abs(i - deltaRow) == 2 && abs(j - deltaCol) == 2)) continue;
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

//----------------------------

DrunkardWalkGenerator::DrunkardWalkGenerator()
:r()
,engine(r())
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

//----------------------------

BSPTree::BSPTree()
	:left(nullptr)
	,right(nullptr)
	,area({ 0,0,0,0 })
{

}

BSPTree::~BSPTree()
{
	if (left) delete left;
	if (right) delete right;
	if (room) delete room;
	left = right = nullptr;
	room = nullptr;
}

void BSPTree::getLeaves(std::vector<BSPTree*>& leaves)
{
	if (right == nullptr && left == nullptr)
	{
		leaves.emplace_back(this);
	}
	else
	{
		left->getLeaves(leaves);
		right->getLeaves(leaves);
	}
}

bool BSPTree::split()
{
	int splitValue;

	std::uniform_real_distribution<float> realDist(0.f, 1.f);
	bool isHSplit = realDist(*splitEngine) < config->horizSplitProbability;
	float ratio = area.w / (float)area.h;
	if (ratio >= (1.f + config->splitRatio))
	{
		isHSplit = false;
	}
	else if ((area.h / (float)area.w) > 1.f + config->splitRatio)
	{
		isHSplit = true;
	}

	int maxSize = 0;
	int minSize = 0;
	if (isHSplit)
	{
		maxSize = area.h - config->minHeight;
		minSize = config->minHeight;
	}
	else
	{
		maxSize = area.w - config->minWidth;
		minSize = config->minWidth;
	}
	if (maxSize <= minSize)
	{
		return false;
	}

	std::uniform_int_distribution<int> splitDist(minSize, maxSize);
	splitValue = splitDist(*splitEngine);
		
	left = new BSPTree();
	right = new BSPTree();
	left->splitEngine = right->splitEngine = splitEngine;
	left->config = right->config = config;
	if (isHSplit)
	{
		left->area = {area.x, area.y, area.w, splitValue};
		right->area = { area.x, area.y + splitValue, area.w, area.h - splitValue };
	}
	else
	{
		left->area = {area.x, area.y, splitValue, area.h};
		right->area = { area.x + splitValue, area.y, area.w - splitValue, area.h };
	}
	left->split();
	right->split();
	return true;
}

BSPGenerator::BSPGenerator()
{
}

BSPGenerator::~BSPGenerator()
{
	if (generatedTree) { delete generatedTree; generatedTree = nullptr; }
}
void BSPGenerator::init(const BSPConfig& _config)
{
	config = _config;
	splitEngine.seed(r());
}
void BSPGenerator::renderGUI(Game* game)
{
	
}
void BSPGenerator::start(Map* map)
{
	for (int i = 0; i < map->rows; ++i)
	{
		for (int j = 0; j < map->cols; ++j)
		{
			(*map)(i, j) = CellType::Wall;
		}
	}

	std::uniform_int_distribution<int> rowDist(config.minHeight, config.maxHeight);
	std::uniform_int_distribution<int> colDist(config.minWidth, config.maxWidth);


	generatedTree = new BSPTree();
	generatedTree->left = generatedTree->right = nullptr;
	generatedTree->area = { 0, 0, map->cols, map->rows };
	
	generatedTree->splitEngine = &splitEngine;
	generatedTree->config = &config;
}

void BSPGenerator::step(Map* map)
{
}
void BSPGenerator::generate(Map* map)
{
	std::vector<BSPTree*> leaves;
	generatedTree->getLeaves(leaves);
	
	std::uniform_real_distribution<float> skipDist(0.f, 1.f);
	std::uniform_int_distribution<int> wDist(config.minWidth, config.maxWidth);
	

	// 1: split map
	generatedTree->split();
	/*do
	{
		splits = 0;
		for (BSPTree* leaf: leaves)
		{
			if (leaf->split())
			{
				splits++;
			}
		}
	} while (splits != 0);*/

	// 2: populate
	generatedTree->getLeaves(leaves);
	for (BSPTree* leaf : leaves)
	{
		bool willSkip = skipDist(splitEngine) < config.horizSplitProbability;
		//if (willSkip) continue;

		std::uniform_int_distribution<int> roomDist(config.minHeight, std::min(config.maxHeight, leaf->area.h));
		int h = roomDist(splitEngine);
		roomDist = std::uniform_int_distribution<int>(config.minWidth, std::min(config.maxWidth, leaf->area.w));
		int w = roomDist(splitEngine);

		roomDist = std::uniform_int_distribution<int>(0, leaf->area.x + leaf->area.w - w);
		int x = leaf->area.x + roomDist(splitEngine);

		roomDist = std::uniform_int_distribution<int>(0, leaf->area.y + leaf->area.h - h);
		int y = leaf->area.y + roomDist(splitEngine);
		leaf->room = new BSPRect();
		leaf->room->x = x;
		leaf->room->y = y; 
		leaf->room->w = w; 
		leaf->room->h = h;

		for (int posY = y; posY <= y + h; ++posY)
		{
			for (int posX = x; posX <= x + w; ++posX)
			{
				(*map)(posY, posX) = CellType::Empty;
			}
		}
	}
	
	// 3: connect

}