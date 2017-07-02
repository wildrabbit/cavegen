#include "game.h"

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-sfml/imgui-sfml.h"


#include "generators/bspRoomsGenerator.h"
#include "generators/cellAutomataGenerator.h"
#include "generators/drunkardWalkGenerator.h"

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

std::string getGeneratorName(IGenerator* generator)
{
	if (generator == nullptr) return "NONE";
	switch (generator->getType())
	{
		case GeneratorType::BSP:
		{
			return "BSP";
		}
		case GeneratorType::CellAutomata:
		{
			return "CELL AUTOMATA";
		}
		case GeneratorType::DrunkardWalk:
		{
			return "DRUNKARD WALK";
		}
	}
	return "NONE";
}

void Game::drawGUI()
{
	std::string name = getGeneratorName(generator);
	
	char buf[128];
	if (!playing)
	{
		snprintf(buf, 128, "Config generator: %s ###window", name.c_str());
	}
	else
	{
		snprintf(buf, 128, "Iter %d/%d###window", (simulationConfig.numIters - itersLeft), simulationConfig.numIters);
	}
	ImGui::Begin(buf); // begin window

	char generatorName[64];
	snprintf(generatorName, 64, "Current generator: %s", name.c_str());
	ImGui::LabelText("", generatorName);
	
	if (!playing)
	{
		ImGui::LabelText("", "Map data");
		// Background color edit
		ImGui::InputInt2("Rows, cols", mapConfig.size);
		// Clamp values
		mapConfig.size[MAP_ROWS_IDX] = std::min(mapConfig.size[MAP_ROWS_IDX], getMaxRows());
		mapConfig.size[MAP_COLS_IDX] = std::min(mapConfig.size[MAP_COLS_IDX], getMaxCols());

		if (ImGui::Button("Reset map size"))
		{
			map->init(mapConfig.size[MAP_ROWS_IDX], mapConfig.size[MAP_COLS_IDX], CellType::Wall);
		}
		ImGui::Separator();

		ImGui::LabelText("", "Generator data");
		const char* items[] = { "Cell automata", "Drunkard Walk", "BSP" };
		if (ImGui::Combo("Generator type", &simulationConfig.generatorIdx, items, 3))
		{
			GeneratorType nextType = static_cast<GeneratorType>(simulationConfig.generatorIdx);
			if (generator == nullptr || generator->getType() != nextType)
			{
				delete generator;
				generator = nullptr;
				if (nextType == GeneratorType::CellAutomata)
				{
					simulationConfig.numIters = 10;
					CellAutomataConfig config;
					generator = new CellAutomataGenerator();
					((CellAutomataGenerator*)generator)->init(config);
				}
				else if (nextType == GeneratorType::DrunkardWalk)
				{
					simulationConfig.numIters = 10000;
					DrunkardWalkConfig config;
					generator = new DrunkardWalkGenerator();
					((DrunkardWalkGenerator*)generator)->init(config);
				}
				else
				{
					BSPConfig config;
					generator = new BSPGenerator();
					((BSPGenerator*)generator)->init(config);
				}
			}
		}

		if (generator != nullptr)
		{
			generator->renderGUI(this);
		}
		ImGui::Separator();
		bool isBSP = generator->getType() == GeneratorType::BSP;
		if (!isBSP)
		{
			ImGui::LabelText("", "Simulation data");
			ImGui::Checkbox("Auto-step?", &simulationConfig.autoStep);
			ImGui::InputInt("Iteration #", &simulationConfig.numIters);
			ImGui::InputFloat("Step delay", &simulationConfig.stepDelay);
		}
		else
		{
			simulationConfig.autoStep = false;
		}
		

		bool isAutomataGenerator = generator->getType() == GeneratorType::CellAutomata;
		char startLabel[32];
		if (simulationConfig.autoStep)
		{
			snprintf(startLabel, 32, "Play!");
		}
		else
		{
			snprintf(startLabel, 32, "Start generation");
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
			else
			{
				generator->generate(map);
			}
		}
		if (!simulationConfig.autoStep && isAutomataGenerator && ImGui::Button("Next Step"))
		{
			generator->generate(map);
		}
	}
	else
	{
		if (generator->getType() == GeneratorType::DrunkardWalk)
		{
			((DrunkardWalkGenerator*)generator)->renderInfoGUI(this);
		}
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
				return;
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

		DrunkardWalkGenerator* drunkardGen = dynamic_cast<DrunkardWalkGenerator*>(generator);

		if (itersLeft == 0 || (drunkardGen != nullptr && drunkardGen->stopWhenRatioIsMet && drunkardGen->filledRatio(map)))
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
	const sf::Color COLOR_EMPTY(0xA, 0xf, 0xd);
	const sf::Color COLOR_WALL(0xc4, 0xcb, 0xca);

	float cellWidth = (float)mapConfig.cellSize[CELL_WIDTH_IDX];
	float cellHeight = (float)mapConfig.cellSize[CELL_HEIGHT_IDX];

	sf::RectangleShape emptyShape({ cellWidth , cellHeight });
	emptyShape.setFillColor(COLOR_EMPTY);
	sf::RectangleShape wallShape({ cellWidth, cellHeight });
	wallShape.setFillColor(COLOR_WALL);

	sf::RectangleShape hallShape({ cellWidth, cellHeight });
	hallShape.setFillColor(sf::Color::Red);

	sf::RectangleShape* shape = nullptr;
	for (int y = 0; y < map->rows; ++y)
	{
		for (int x = 0; x < map->cols; ++x)
		{
			CellType curCell = (*map)(y, x);
			shape = nullptr;
			switch (curCell)
			{
			case CellType::Empty:
			{
				shape = &emptyShape; break;
			}
			case CellType::Wall:
			{
				shape = &wallShape; break;
			}
			case CellType::Corridor:
			{
				shape = &hallShape; break;
			}
			}
			if (shape != nullptr)
			{
				shape->setPosition({ x *  cellWidth, y *  cellHeight });				
				window->draw(*shape);
			}
		}
	}
}