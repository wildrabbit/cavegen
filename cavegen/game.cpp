#include "game.h"

#include "map.h"
#include "generators.h"
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-sfml/imgui-sfml.h"

static const int MAP_ROWS_IDX = 0;
static const int MAP_COLS_IDX = 1;

static const int CELL_WIDTH_IDX = 0;
static const int CELL_HEIGHT_IDX = 1;

bool Game::init(const WindowConfig& _windowConfig, const MapConfig& _mapConfig, const SimulationConfig& _simulationConfig)
{
	windowConfig = _windowConfig;
	mapConfig = _mapConfig;
	simulationConfig = _simulationConfig;

	window = new sf::RenderWindow(sf::VideoMode(windowConfig.width, windowConfig.height), windowConfig.title, windowConfig.fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
	window->setVerticalSyncEnabled(true);

	map = new Map();
	map->init(mapConfig.size[MAP_ROWS_IDX], mapConfig.size[MAP_COLS_IDX], CellType::Wall);

	CellAutomataConfig config;
	generator = new CellAutomataGenerator();
	((CellAutomataGenerator*)generator)->init(config);


	ImGui::SFML::Init(*window);

	return true;
}

int Game::getMaxRows() const
{
	return windowConfig.height / mapConfig.cellSize[CELL_HEIGHT_IDX];
}

int Game::getMaxCols() const
{
	return windowConfig.width / mapConfig.cellSize[CELL_WIDTH_IDX];
}

void Game::drawGUI()
{
	ImGui::Begin("Map starting params"); // begin window
										 // Background color edit
	ImGui::InputInt2("Rows, cols", mapConfig.size);
	// Clamp values
	mapConfig.size[MAP_ROWS_IDX] = std::min(mapConfig.size[MAP_ROWS_IDX], getMaxRows());
	mapConfig.size[MAP_COLS_IDX] = std::min(mapConfig.size[MAP_COLS_IDX], getMaxCols());

	if (ImGui::Button("Clear map"))
	{
		map->init(mapConfig.size[MAP_ROWS_IDX], mapConfig.size[MAP_COLS_IDX], CellType::Wall);
	}

	if (generator != nullptr)
	{
		generator->renderGUI(this);
	}

	// TODO: Simulation window
	ImGui::Checkbox("Auto-step?", &simulationConfig.autoStep);
	ImGui::InputInt("Iteration #", &simulationConfig.numIters);
	ImGui::InputFloat("Step delay", &simulationConfig.stepDelay);


	if (ImGui::Button("Start generation"))
	{
		generator->start(map);
		if (simulationConfig.autoStep)
		{
			playing = true;
			itersLeft = simulationConfig.numIters;
			nextIterTime = simulationConfig.stepDelay;
		}
	}
	if (!simulationConfig.autoStep && ImGui::Button("Simulation step"))
	{
		generator->generate(map);
	}
	ImGui::End(); // end window
	ImGui::Render();
}

void Game::run()
{
	sf::Clock deltaClock;
	while (window->isOpen())
	{
		// Read input!
		sf::Event evt;
		while (window->pollEvent(evt))
		{
			ImGui::SFML::ProcessEvent(evt);
			if (evt.type == sf::Event::Closed || evt.type == sf::Event::KeyReleased && evt.key.code == sf::Keyboard::Key::Escape)
			{
				window->close();
			}
		}
		sf::Time elapsed = deltaClock.restart();
		ImGui::SFML::Update(*window, elapsed);
		update(elapsed.asSeconds());

		draw();
	}
}
void Game::update(float deltaTime)
{
	if (!playing) return;
	if (itersLeft <= 0)
	{
		itersLeft = 0;
		return;
	}
	nextIterTime -= deltaTime;
	if (nextIterTime <= 0.f)
	{
		generator->generate(map);
		itersLeft--;

		if (itersLeft == 0)
		{
			nextIterTime = 0.f;
			playing = false;
		}
		else
		{
			nextIterTime = simulationConfig.stepDelay;
		}
	}

}
void Game::draw()
{
	window->clear();
	if (map != nullptr)
	{
		drawMap();
	}

	drawGUI();


	window->display();
}

void Game::cleanup()
{
	ImGui::SFML::Shutdown();
	window->close();
	delete window;
	window = nullptr;

	if (map)
	{
		delete map;
		map = nullptr;
	}
}

void Game::drawMap()
{
	const sf::Color COLOR_EMPTY(0x1A, 0x1E, 0x1B);
	const sf::Color COLOR_WALL(0x20, 0x42, 0x26);

	float cellWidth = (float)mapConfig.cellSize[CELL_WIDTH_IDX];
	float cellHeight = (float)mapConfig.cellSize[CELL_HEIGHT_IDX];

	sf::RectangleShape emptyShape({ cellWidth , cellHeight });
	emptyShape.setFillColor(COLOR_EMPTY);
	sf::RectangleShape wallShape({ cellWidth, cellHeight });
	wallShape.setFillColor(COLOR_WALL);

	sf::RectangleShape shape;
	for (int y = 0; y < map->rows; ++y)
	{
		for (int x = 0; x < map->cols; ++x)
		{
			sf::Shape* shape = ((*map)(y, x) == CellType::Empty) ? &emptyShape : &wallShape;
			if (shape != nullptr)
			{
				shape->setPosition({ x *  cellWidth, y *  cellHeight });
				window->draw(*shape);
			}
		}
	}
}