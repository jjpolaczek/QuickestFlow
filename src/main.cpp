#include <iostream>
#include "QFSolver.h"
#include "Generator.h"
using namespace lemon;
int main()
{
	GraphGenerator gen;
	gen.Generate("graph.txt", 1000);
	QFSolver solver;
	solver.LoadGraph("graph.txt");
	solver.Solve();
	solver.SaveResults("results.txt");
	system("PAUSE");

}