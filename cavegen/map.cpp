#include "map.h"

CellType Map::NOCELL = CellType::Empty;

void Map::init(int _rows, int _cols, CellType _defaultValue)
{
	rows = _rows;
	cols = _cols;
	cells.resize(rows * cols);
	std::fill(cells.begin(), cells.end(), _defaultValue);
}

CellType Map::operator[] (int idx) const
{
	return (idx >= 0 && idx < (int)cells.size()) ? cells[idx] : Map::NOCELL;
}
CellType& Map::operator[] (int idx)
{
	return (idx >= 0 && idx < (int)cells.size()) ? cells[idx] : Map::NOCELL;
}

bool Map::validCoords(int row, int col) const
{
	return row >= 0 && row < rows && col >= 0 && col < cols;
}

bool Map::validIdx(int idx) const
{
	return idx >= 0 && idx < (int)cells.size();
}

int Map::asIndex(int row, int col) const
{
	if (!validCoords(row, col)) return -1;
	return row * cols + col;
}

void Map::asCoords(int idx, int& row, int& col) const
{
	if (!validIdx(idx))
	{
		row = -1;
		col = -1;
	}
	else
	{
		row = idx / cols;
		col = idx % cols;
	}
}

CellType Map::operator() (int row, int col) const
{
	return (*this)[asIndex(row, col)];
}

CellType& Map::operator() (int row, int col)
{
	return (*this)[asIndex(row, col)];
}