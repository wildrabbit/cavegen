#pragma once
#include "../generators.h"


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

class CellAutomataGenerator : public IGenerator
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