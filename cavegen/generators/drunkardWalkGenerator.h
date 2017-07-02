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