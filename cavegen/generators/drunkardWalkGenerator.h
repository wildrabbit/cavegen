#pragma once
#include "../generators.h"


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
	int lastStepCount;
	int numTilesCarved;
	int mapSize;
	float lastEmptyRatio; 
	std::random_device r;
	std::default_random_engine engine;

public:
	bool stopWhenRatioIsMet = true;

	DrunkardWalkGenerator();
	~DrunkardWalkGenerator() {}

	GeneratorType getType() const override
	{
		return GeneratorType::DrunkardWalk;
	}

	void init(const DrunkardWalkConfig& _config);

	void renderGUI(Game* game) override;
	void renderInfoGUI(Game* game);
	void start(Map* map) override;
	bool filledRatio(Map* map) const;
	void generate(Map* map) override;
	void step(Map* map) override;
};