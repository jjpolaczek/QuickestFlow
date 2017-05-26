#include <iostream>
#include "QFSolver.h"
using namespace lemon;
int main()
{
	QFSolver solver;
	solver.LoadGraph("sample.txt");
	solver.Solve();
	
	system("PAUSE");

}