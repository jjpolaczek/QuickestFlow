#include "QFSolver.h"

using std::ifstream;
using namespace lemon;
void QFSolver::LoadGraph(std::string filename)
{
	ifstream file;
	file.open(filename, std::ios::in);
	if (file.is_open())
	{

		std::string line;
		struct Arc
		{
			int vi;
			int vj;
			int capacity;
			int delay;
		};
		std::vector<Arc> tmpArc;
		//First line contains graph global parameters//
		int source, target, F, node_count, arc_count;
		std::string::size_type sz;
		if (!getline(file, line))
			throw std::out_of_range("File containing graph empty");

		source = std::stoi(line, &sz);
		line = line.substr(sz);
		target = std::stoi(line, &sz);
		line = line.substr(sz);
		F = std::stoi(line, &sz);
		line = line.substr(sz);
		node_count = std::stoi(line, &sz);
		line = line.substr(sz);
		arc_count = std::stoi(line, &sz);
		std::cout << "Loading graph, source: " << source << ", sink: " << target << ", flow: " << F << "\r\n";
		std::cout << "Nodes: " << node_count << ", Arcs: " << arc_count << "\r\n\r\n";
		while (getline(file, line))
		{
			
			Arc arc;// vi, vj, capacity, delay;
			arc.vi = std::stoi(line, &sz);
			line = line.substr(sz);
			arc.vj = std::stoi(line, &sz);
			line = line.substr(sz);
			arc.capacity = std::stoi(line, &sz);
			line = line.substr(sz);
			arc.delay = std::stoi(line, &sz);
			std::cout << "Arc parameters: " << arc.vi << "-" << arc.vj << ", capacity: " << arc.capacity << ", delay: " << arc.delay << "\r\n";
			tmpArc.push_back(arc);
		}

		file.close();
		//Verify integrity of the data//
		if (tmpArc.size() != arc_count)
			throw std::out_of_range("Invalid nodes");
		for (Arc a : tmpArc)
		{
			if (a.vi > node_count || a.vj > node_count)
				throw std::out_of_range("Invalid arc node parameters");
			if(a.capacity < 0 || a.delay < 0)
				throw std::out_of_range("Invalid arc parameters");
		}
		//File is loaded we can now init memory//
		baseGraph_ = new ListDigraph();
		for (int i = 0; i < node_count; ++i)
			baseGraph_->addNode();
		initialized_ = true;
	}
	else
	{
		throw std::invalid_argument("Invalid file path");
	}
}