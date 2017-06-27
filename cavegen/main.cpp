#include <iostream>
#include <cstdlib>

#include "map.h"
#include "game.h"

int main()
{

	WindowConfig windowCfg;
	MapConfig mapCfg;
	SimulationConfig simulationCfg;
	Game g;
	if (g.init(windowCfg, mapCfg, simulationCfg))
	{
		g.run();
		g.cleanup();
	}
	
	return 0;
}