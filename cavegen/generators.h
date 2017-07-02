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
	AgentWalk = 3	// TODO
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
