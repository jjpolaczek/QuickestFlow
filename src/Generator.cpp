#include "Generator.h"


using namespace lemon;
using std::ofstream;
void GraphGenerator::Generate(std::string filename, const int nodeCount)
{
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> idist(0, nodeCount - 1);
	std::uniform_int_distribution<std::mt19937::result_type> iddist(1, 100);
	std::uniform_int_distribution<std::mt19937::result_type> icdist(1, 100);
	std::uniform_int_distribution<std::mt19937::result_type> ifdist(100, 1000);
	std::uniform_real_distribution<double> rdist(0.0, 1.0);

	const double thresh = 0.15;
	if (nodeCount < 2)
		throw std::invalid_argument("Invalid node count < 2");
	//Create a strongly connected graph with speed and velcity maps and save it to a file//
	ListDigraph graph;
	//ListDigraph::ArcMap<int> delayMap(graph, 0);
	//ListDigraph::ArcMap<int> speedMap(graph, 0);

	//Graph creation until strongly connected//
	bool **conTrack;
	conTrack = new bool*[nodeCount];
	for (int i = 0; i < nodeCount; ++i)
	{
		conTrack[i] = new bool[nodeCount];
	}
	for (int i = 0; i < nodeCount; ++i)
	{
		graph.addNode();
		for (int j = 0; j < nodeCount; ++j)
			conTrack[i][j] = false;
	}
	for (int i = 0; i < nodeCount; ++i)
		for (int j = 0; j < nodeCount; ++j)
		{
			if (rdist(rng) < thresh)
			{
				//Connect net//
				conTrack[i][j] = true;
				graph.addArc(graph.nodeFromId(i), graph.nodeFromId(j));
			}
			
		}
	//Ensure full connectivity//
	while (!stronglyConnected(graph))
	{
		//Find random edge that is not yet used//
		while (true)
		{
			int vi = idist(rng);
			int vj = idist(rng);
			if (vi != vj && conTrack[vi][vj] == false)
			{
				conTrack[vi][vj] = true;
				graph.addArc(graph.nodeFromId(vi), graph.nodeFromId(vj));
				break;
			}
		}
	}

	//Save file containing the graph//
	ofstream file;
	file.open(filename, std::ios::out | std::ios::trunc);
	if (file.is_open())
	{
		int source = 0, target = 0;
		while (source == target)
		{
			source = idist(rng);
			target = idist(rng);
		}

		//Header//
		file << source << "\t" << target << "\t" << ifdist(rng) << "\t" << graph.maxArcId() + 1 << "\t" << graph.maxNodeId() + 1 << "\r\n";
		for (int i = 0; i < graph.maxArcId() + 1; ++i)
		{
			ListDigraph::Arc arc = graph.arcFromId(i);
			file << graph.id(graph.source(arc)) << "\t" << graph.id(graph.target(arc)) << "\t" << icdist(rng) << "\t" << iddist(rng) << "\r\n";
		}
		file.close();
	}
	else
	{
		throw std::invalid_argument("Invalid file path");
	}
	//Cleanup//
	for (int i = 0; i < nodeCount; ++i)
	{
		delete[] conTrack[i];
	}
	delete[] conTrack;
} 