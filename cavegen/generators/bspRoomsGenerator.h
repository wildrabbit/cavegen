#pragma once
#include "../generators.h"

//----------- BSP ----------------//
struct BSPConfig
{
	int maxDivisions = -1;

	float splitHRatio = 0.3f;
	float splitVRatio = 0.5f;

	float horizSplitProbability = 0.5f;
	float emptyRoomProbability = 0.04f;
	int minWidth = 40;
	int minRoomWidth = 18;
	int maxRoomWidth = minWidth - 2;

	int minHeight = 40;
	int minRoomHeight = 20;
	int maxRoomHeight = minHeight - 2;
};

struct BSPRect
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;

	void set(int _x, int _y, int _w, int _h)
	{
		x = _x;
		y = _y;
		w = _w;
		h = _h;
	}
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
	BSPTree* getRoom();
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
