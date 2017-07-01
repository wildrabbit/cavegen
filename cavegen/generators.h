#pragma once
#include <functional>
#include <random>
#include "map.h"
class Game;

enum class GeneratorType : int
{
	CellAutomata = 0,
	DrunkardWalk = 1,
	BSP = 2,
	AgentWalk = 3
};

class IGenerator
{
public:
	virtual void start(Map* map) = 0;
	virtual void generate(Map* map) = 0;
	virtual void step(Map* map) = 0;
	virtual void renderGUI(Game* game) = 0;
	virtual GeneratorType getType() const = 0;
};

typedef std::function<bool(int, int, const Map*, const Map::CellArray&)> CellEvaluationFunction;


struct CellAutomataConfig
{
	bool useSeed = false;
	unsigned int seed = 0;
	bool useCorners = false;
	float wallProbability = 0.45f;
	int minSurroundingWallsForNextIter = 5;
	bool includeSelf = true;
	int functionIndex = 0;
	CellEvaluationFunction checkFunction = nullptr;
};

class CellAutomataGenerator: public IGenerator
{
private:
	CellAutomataConfig config;

private:
	void noise(Map* map);
	bool basicEvaluation(int i, int j, const Map* map, const Map::CellArray& oldCells);
	bool basicEvaluationClosed(int i, int j, const Map* map, const Map::CellArray& oldCells);
	int countNeighbourhood(int i, int j, const Map* map, const Map::CellArray& oldCells, int distance, bool countSelf);
public:
	CellAutomataGenerator() {}
	~CellAutomataGenerator() {}
	
	GeneratorType getType() const override
	{
		return GeneratorType::CellAutomata;
	}

	void init(const CellAutomataConfig& _config);

	void renderGUI(Game* game) override;
	void start(Map* map) override;
	void generate(Map* map) override;
	void step(Map* map) override;

};

//----------- DRUNKARD WALK ----------------//
struct DrunkardWalkConfig
{
	float expectedRatio = 0.55f;
};

class DrunkardWalkGenerator : public IGenerator
{
private:
	DrunkardWalkConfig config;
	int row;
	int col;

	std::random_device r;
	std::default_random_engine engine;

public:
	DrunkardWalkGenerator();
	~DrunkardWalkGenerator() {}

	GeneratorType getType() const override
	{
		return GeneratorType::DrunkardWalk;
	}

	void init(const DrunkardWalkConfig& _config);

	void renderGUI(Game* game) override;
	void start(Map* map) override;
	void generate(Map* map) override;
	void step(Map* map) override;
};

//----------- BSP ----------------//
struct BSPConfig
{
	int maxDivisions = -1;

	float splitRatio = 0.25f;

	float horizSplitProbability = 0.45f;
	float emptyRoomProbability = 0.05f;
	int minWidth = 15;
	int maxWidth = 40;

	int minHeight = 15;
	int maxHeight = 40;
};

struct BSPRect
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
};

struct BSPTree
{
	BSPRect area;
	BSPRect* room = nullptr;
	BSPTree* right = nullptr;
	BSPTree* left = nullptr;

	BSPConfig* config;

	std::default_random_engine* splitEngine;
	
	BSPTree();
	~BSPTree();
	
	void getLeaves(std::vector<BSPTree*>& leaves);
	bool split();
};

class BSPGenerator : public IGenerator
{
private:
	BSPConfig config;
	BSPTree* generatedTree;

	std::random_device r;
	std::default_random_engine splitEngine;
public:
	BSPGenerator();
	~BSPGenerator();

	GeneratorType getType() const override
	{
		return GeneratorType::BSP;
	}

	void init(const BSPConfig& _config);
	void renderGUI(Game* game) override;
	void start(Map* map) override;
	void generate(Map* map) override;
	void step(Map* map) override;
};

//----------- AGENT ----------------//