#pragma once

#include <lemon\concepts\graph.h>
#include <lemon\concepts\maps.h>
#include <lemon\list_graph.h>
#include <lemon\edmonds_karp.h>
#include <iostream>
#include <fstream>
//!  Quickest Flow Solver class
/*!
	Class for solving the quickest flow problem. Typical workflow consists of calling LoadGraph() on a graph file
	followed by Solve() and SaveResults()
*/
class QFSolver
{
public:
	QFSolver() :
		baseGraph_(nullptr),
		speedMap_(nullptr),
		capacityMap_(nullptr),
		teGraph_(nullptr),
		teMap_(nullptr),
		flowMap_(nullptr),
		initialized_(false),
		horizon_(0),
		sourceID_(-1),
		targetID_(-1),
		flowValue_(0),
		maxCapacity_(0),
		maxDelay_(0)
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
		if (flowMap_ != nullptr)
			delete flowMap_;
	}
	//!  LoadGraph
	/*!
		Method loads flow network into class structure from file containing flow network definition named filepath
	*/
	void LoadGraph(std::string filepath);
	//!  Solve
	/*!
		Method solves Quickest flow problem for previously loaded graph. Throws exception if called without geometry
	*/
	void Solve();
	//!  SaveResults
	/*!
		Method saves calculation results in a file specified with filename.
	*/
	void SaveResults(std::string filename);
private:
	//!  ExpandGrapg
	/*!
		Create a time expanded graph for max flow calculation
	*/
	void ExpandGraph(unsigned int horizon);
	//!  MaxFlow
	/*!
		Calculate Max flow on base flow network loaded from file, Calls ExpandGraph internally
	*/
	int MaxFlow(unsigned int horizon);
	//!  MaxFlowMap
	/*!
		Variation of MaxFlow, saves the flow map into class structure ( it is dedstroyed in the alternative)
	*/
	void MaxFlowMap(unsigned int horizon);
	lemon::ListDigraph *baseGraph_;
	lemon::ListDigraph::ArcMap<int> *capacityMap_;
	lemon::ListDigraph::ArcMap<int> *speedMap_;
	lemon::ListDigraph *teGraph_;
	lemon::ListDigraph::ArcMap<int> *teMap_;
	lemon::ListDigraph::ArcMap<int> *flowMap_;
	bool initialized_;
	int horizon_;
	int sourceID_;
	int targetID_;
	int flowValue_;
	int maxCapacity_;
	int maxDelay_;
	int unitsSent_;
	int timeUsed_;
};