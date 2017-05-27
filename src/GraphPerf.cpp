#include "GraphPerf.h"
using namespace std;
double PerformanceMonitor::Measure(int graphSize, int sampleCount) 
{
	std::vector<double> times;
	GraphGenerator generator;
	QFSolver solver;
	for (int i = 0; i < sampleCount; ++i)
	{
		generator.Generate("graph.txt", graphSize);
		solver.LoadGraph("graph.txt");
		clock_t begin = clock();
		solver.Solve();
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		times.push_back(elapsed_secs);
	}
	double average = times.back();
	times.pop_back();
	for (double time : times)
	{
		average += time;
	}
	return average / sampleCount;
}