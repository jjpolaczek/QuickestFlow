#pragma once

#include "Generator.h"
#include "QFSolver.h"
#include <ctime>
#include <vector>

//!  Performance Monitor Class
/*!
	Class for measuring the Quickest flow algorithm performance
*/
class PerformanceMonitor
{
public:
	PerformanceMonitor()
	{}
	//!  Measure
	/*!
		Measure time needed for solving a sampleCount  quickest flow problems in a flow network 
		with graphSize ammount of nodes and average the time needed
	*/
	double Measure(int graphSize, int sampleCount);
};