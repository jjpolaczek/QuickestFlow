#pragma once

#include "Generator.h"
#include "QFSolver.h"
#include <ctime>
#include <vector>

class PerformanceMonitor
{
public:
	PerformanceMonitor()
	{}
	double Measure(int graphSize, int sampleCount);
};