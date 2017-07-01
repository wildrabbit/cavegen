#pragma once

#include <vector>
#include <functional>
#include <random>

enum class CellType : int
{
	Empty = 0,
	Wall,
	Corridor
};

struct Map
{
	static CellType NOCELL;

	typedef std::vector<CellType> CellArray;
	typedef std::function<bool(int, int, const CellArray&)> CellEvaluationFunction;

	CellArray cells;
	int rows = 0;
	int cols = 0;
	CellEvaluationFunction checkFunction;

	int numCells() const 
	{
		return rows * cols;
	}

	Map(){}

	void init(int _rows, int _cols, CellType _defaultValue = CellType::Empty);

	CellType operator[] (int idx) const;
	CellType& operator[] (int idx);
	
	bool validCoords(int row, int col) const;
	bool validIdx(int idx) const;
	int asIndex(int row, int col) const;
	void asCoords(int idx, int& row, int& col) const;
	
	CellType operator() (int row, int col) const;
	CellType& operator() (int row, int col);
};
