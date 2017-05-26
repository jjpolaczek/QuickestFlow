#pragma once

#include <lemon\concepts\graph.h>
#include <lemon\concepts\maps.h>
#include <lemon\list_graph.h>
#include <lemon\edmonds_karp.h>

#include <iostream>
#include <fstream>

class QFSolver
{
public:
	QFSolver():
		baseGraph_(nullptr),
		speedMap_(nullptr),
		capacityMap_(nullptr),
		teGraph_(nullptr),
		teMap_(nullptr),
		initialized_(false)
	{

	}
	~QFSolver()
	{
		if (baseGraph_ != nullptr)
			delete baseGraph_;
		if (speedMap_ != nullptr)
			delete speedMap_;
		if (capacityMap_ != nullptr)
			delete capacityMap_;
		if (teGraph_ != nullptr)
			delete teGraph_;
		if (teMap_ != nullptr)
			delete teMap_;
	}
	void LoadGraph(std::string filepath);
	void Solve();
private:
	lemon::ListDigraph *baseGraph_;
	lemon::ListDigraph::ArcMap<unsigned int> *capacityMap_;
	lemon::ListDigraph::ArcMap<unsigned int> *speedMap_;
	lemon::ListDigraph *teGraph_;
	lemon::ListDigraph::ArcMap<unsigned int> *teMap_;
	bool initialized_;
};