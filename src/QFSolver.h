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
	QFSolver() :
		baseGraph_(nullptr),
		speedMap_(nullptr),
		capacityMap_(nullptr),
		teGraph_(nullptr),
		teMap_(nullptr),
		initialized_(false),
		horizon_(0),
		sourceID_(-1),
		targetID_(-1)
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
	void ExpandGraph(unsigned int horizon);
	int MaxFlow();
	lemon::ListDigraph *baseGraph_;
	lemon::ListDigraph::ArcMap<int> *capacityMap_;
	lemon::ListDigraph::ArcMap<int> *speedMap_;
	lemon::ListDigraph *teGraph_;
	lemon::ListDigraph::ArcMap<int> *teMap_;
	bool initialized_;
	int horizon_;
	int sourceID_;
	int targetID_;
};