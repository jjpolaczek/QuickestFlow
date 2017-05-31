#include <iostream>
#include "QFSolver.h"
#include "Generator.h"
#include "GraphPerf.h"

//#define PERFORMANCETEST
using std::ofstream;
int main()
{

	QFSolver solver;
	solver.LoadGraph("sample.txt");
	solver.Solve();
	solver.SaveResults("results.txt");
	return 0;
#ifdef PERFORMANCETEST
	QFSolver solver;
	solver.LoadGraph("sample.txt");
	solver.Solve();
	solver.SaveResults("results.txt");
	return 1;
	//system("PAUSE");
	PerformanceMonitor monitor;
	ofstream file;

	int attempts = 50;
	int skip = 2;
	int min = 2;
	int max = 50;
	char filestr[32];
	sprintf_s(filestr, sizeof(filestr), "%d-%dresults.txt", min, max);
	std::string filename(filestr);
	file.open(filename, std::ios::out | std::ios::trunc);
	if (file.is_open())
	{
		file << "edges\tattempts\tseconds\r\n";
		for (int i = min; i < max; i += skip)
		{
			double time = monitor.Measure(i, attempts);
			file << i << "\t" << attempts << "\t" << time << "\r\n";
			std::cout << i << "edges\t" << attempts << "attempts\t" << time << "seconds\r\n";
		}

		file.close();
	}
	else
	{
		throw std::invalid_argument("Invalid file path");
	}
#endif
}