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

	simulationConfig.generatorIdx = 0;
	CellAutomataConfig config;
	generator = new CellAutomataGenerator();
	((CellAutomataGenerator*)generator)->init(config);

	window = new sf::RenderWindow(sf::VideoMode(windowConfig.width, windowConfig.height), windowConfig.title, windowConfig.fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
	window->setVerticalSyncEnabled(true);

	map = new Map();
	map->init(mapConfig.size[MAP_ROWS_IDX], mapConfig.size[MAP_COLS_IDX], CellType::Wall);


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
	char buf[128];
	if (!playing)
	{
		snprintf(buf, 128, "Config generator###window");
	}
	else
	{
		snprintf(buf, 128, "Iter %d/%d###window", (simulationConfig.numIters - itersLeft), simulationConfig.numIters);
	}
	ImGui::Begin(buf); // begin window

	char generatorName[64];
	if (generator == nullptr)
	{
		snprintf(generatorName, 64, "NONE");
	}
	else if (generator->getType() == GeneratorType::CellAutomata)
	{
		snprintf(generatorName, 64, "CELL AUTOMATA");
	}
	else
	{
		snprintf(generatorName, 64, "DRUNKARD WALK");
	}
	ImGui::LabelText("Current generator: ", generatorName);
	
	if (!playing)
	{
		// Background color edit
		ImGui::InputInt2("Rows, cols", mapConfig.size);
		// Clamp values
		mapConfig.size[MAP_ROWS_IDX] = std::min(mapConfig.size[MAP_ROWS_IDX], getMaxRows());
		mapConfig.size[MAP_COLS_IDX] = std::min(mapConfig.size[MAP_COLS_IDX], getMaxCols());

		if (ImGui::Button("Clear map"))
		{
			map->init(mapConfig.size[MAP_ROWS_IDX], mapConfig.size[MAP_COLS_IDX], CellType::Wall);
		}

		const char* items[] = { "Cell automata", "Drunkard Walk" };
		if (ImGui::Combo("Generator type", &simulationConfig.generatorIdx, items, 2))
		{
			GeneratorType nextType = static_cast<GeneratorType>(simulationConfig.generatorIdx);
			if (generator == nullptr || generator->getType() != nextType)
			{
				delete generator;
				generator = nullptr;
				if (nextType == GeneratorType::CellAutomata)
				{
					CellAutomataConfig config;
					generator = new CellAutomataGenerator();
					((CellAutomataGenerator*)generator)->init(config);
				}
				else
				{
					DrunkardWalkConfig config;
					generator = new DrunkardWalkGenerator();
					((DrunkardWalkGenerator*)generator)->init(config);
				}
			}
		}

		if (generator != nullptr)
		{
			generator->renderGUI(this);
		}

		// TODO: Simulation window
		ImGui::Checkbox("Auto-step?", &simulationConfig.autoStep);
		ImGui::InputInt("Iteration #", &simulationConfig.numIters);
		ImGui::InputFloat("Step delay", &simulationConfig.stepDelay);


		bool isAutomataGenerator = generator->getType() == GeneratorType::CellAutomata;
		char startLabel[32];
		if (simulationConfig.autoStep)
		{
			snprintf(startLabel, 32, "Play!");
		}
		else
		{
			snprintf(startLabel, 32, "Init generation");
		}
		if (ImGui::Button(startLabel))
		{
			generator->start(map);
			if (simulationConfig.autoStep)
			{
				playing = true;
				itersLeft = simulationConfig.numIters;
				nextIterTime = simulationConfig.stepDelay;
				ImGui::SetWindowCollapsed(true);
			}
		}
		if (!simulationConfig.autoStep && ImGui::Button("Generate!"))
		{
			generator->generate(map);
		}
	}
	else
	{
		if (ImGui::Button("Stop"))
		{
			playing = false;
			nextIterTime = 0;
			itersLeft = 0;
		}
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
		generator->step(map);
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

	if (generator)
	{
		delete generator;
		generator = nullptr;
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