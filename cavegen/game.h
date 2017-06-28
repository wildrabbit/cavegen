#pragma once

#include <string>

namespace sf
{
	class RenderWindow;
}
struct Map;

struct MapConfig
{
	int size[2] = { 150,200};
	int cellSize[2] = {4, 4 };
};

struct SimulationConfig
{
	bool autoStep = false;
	float stepDelay = 0.2f;
	int numIters = 5;
	int generatorIdx = 0;
};

struct WindowConfig
{
	std::string title = "Cavegen!";
	int width = 800;
	int height = 600;
	bool fullscreen = false;
};

class IGenerator;
class Game
{
private:
	sf::RenderWindow* window = nullptr;
	Map* map = nullptr;
	
	MapConfig mapConfig;
	WindowConfig windowConfig;
	SimulationConfig simulationConfig;

	IGenerator* generator;

	bool playing = false;
	int itersLeft = 0;
	float nextIterTime = 0.f;

private:
	int getMaxRows() const;
	int getMaxCols() const;
public:
	Map* getMap() 
	{
		return map;
	}

	bool init(const WindowConfig& _windowConfig, const MapConfig& _mapConfig, const SimulationConfig& _simulationConfig);
	void run();
	void cleanup();

	void drawGUI();
	void drawMap();

	void draw();
	void update(float delta);
};