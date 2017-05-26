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
		struct Arc_
		{
			int vi;
			int vj;
			int capacity;
			int delay;
		};
		std::vector<Arc_> tmpArc;
		//First line contains graph global parameters//
		int source, target, F, node_count, arc_count;
		std::string::size_type sz;
		if (!getline(file, line))
			throw std::out_of_range("File containing graph empty");

		sourceID_ = source = std::stoi(line, &sz);
		line = line.substr(sz);
		targetID_ = target = std::stoi(line, &sz);
		line = line.substr(sz);
		F = std::stoi(line, &sz);
		line = line.substr(sz);
		arc_count = std::stoi(line, &sz);
		line = line.substr(sz);
		node_count = std::stoi(line, &sz);
		std::cout << "Loading graph, source: " << source << ", sink: " << target << ", flow: " << F << "\r\n";
		std::cout << "Nodes: " << node_count << ", Arcs: " << arc_count << "\r\n\r\n";
		while (getline(file, line))
		{
			
			Arc_ arc;// vi, vj, capacity, delay;
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
		for (Arc_ a : tmpArc)
		{
			if (a.vi >= node_count || a.vj >= node_count)
				throw std::out_of_range("Invalid arc node parameters");
			if(a.capacity < 0 || a.delay < 0)
				throw std::out_of_range("Invalid arc parameters");
		}
		//File is loaded we can now init memory//
		baseGraph_ = new ListDigraph();
		//Create graph stucture//
		for (int i = 0; i < node_count; ++i)
			baseGraph_->addNode();

		for (Arc_ a : tmpArc)
		{
			baseGraph_->addArc(baseGraph_->nodeFromId(a.vi), baseGraph_->nodeFromId(a.vj));
		}
		//Add maps for appropriate arcs//
		speedMap_ = new ListDigraph::ArcMap<int>(*baseGraph_,0);
		capacityMap_ = new ListDigraph::ArcMap<int>(*baseGraph_,0);
		for (int i = 0; i < tmpArc.size(); ++i)
		{
			(*speedMap_)[baseGraph_->arcFromId(i)] = tmpArc.at(i).delay;
			(*capacityMap_)[baseGraph_->arcFromId(i)] = tmpArc.at(i).capacity;
		}
		initialized_ = true;
	}
	else
	{
		throw std::invalid_argument("Invalid file path");
	}
}
void QFSolver::ExpandGraph(unsigned int horizon)
{
	if (horizon < 1)
		throw std::out_of_range("Invalid horizon");
	//Delete existing graph and capacity map//
	if (teGraph_ != nullptr)
	{
		delete teGraph_;
		teGraph_ = nullptr;
	}
		
	if (teMap_ != nullptr)
	{
		delete teMap_;
		teMap_ = nullptr;
	}
	teGraph_ = new ListDigraph();
	teMap_ = new ListDigraph::ArcMap<int>(*teGraph_);
	//The new graph has horizon * n nodes - format for visibility
	int nodeCount = baseGraph_->maxNodeId() + 1;
	for (int i = 0; i <= horizon; ++i)
	{
		for (int j = 0; j < nodeCount; ++j)
		{
			teGraph_->addNode();
		}
	}
	//Create map//

	//We want a time expanded graph - vertical (same nodes are connected with infinite capacities 
	//in time slices, this is first n * (horizon -1) arcs//
	for (int i = 0; i <= horizon - 1; ++i)
	{
		for (int j = 0; j < nodeCount; ++j)
		{
			ListDigraph::Arc arc = teGraph_->addArc(baseGraph_->nodeFromId(j + i * nodeCount), baseGraph_->nodeFromId(j + (i + 1) * nodeCount));
			(*teMap_)[arc] = INT32_MAX;//Very large value of capacity//
		}
	}
	//And respective nodes are connected via several time slices (taking speed into account)//
	int arcCount = baseGraph_->maxArcId() + 1;
	for (int i = 0; i < horizon; ++i)
	{
		for (int j = 0; j < arcCount; ++j)
		{
			int vi, vj, speed, capacity;
			ListDigraph::Arc arc = baseGraph_->arcFromId(j);

			vi = baseGraph_->id(baseGraph_->source(arc));
			vj = baseGraph_->id(baseGraph_->target(arc));
			speed = (*speedMap_)[arc];
			capacity = (*capacityMap_)[arc];
			if (i + speed <= horizon)
			{
				ListDigraph::Arc arc = 
					teGraph_->addArc(baseGraph_->nodeFromId(vi + i * nodeCount), baseGraph_->nodeFromId(vj + (i + speed) * nodeCount));
				(*teMap_)[arc] = capacity;//Assign capacity to the arc map//
			}
		}
	}
	horizon_ = horizon;
}
int QFSolver::MaxFlow()
{
	//Sanity checks//
	if (teGraph_ == nullptr || teMap_ == nullptr)
		throw std::exception("No time expanded network");
	//Calculate source and target nodes//
	int source = sourceID_;
	int target = targetID_ + (horizon_ - 1) * (baseGraph_->maxNodeId() + 1); //Trough time expanded network//

	ListDigraph::Node sNode = teGraph_->nodeFromId(source);
	ListDigraph::Node tNode = teGraph_->nodeFromId(target);
	EdmondsKarp<ListDigraph, ListDigraph::ArcMap<int>> mfAlg((*teGraph_),(*teMap_),sNode, tNode);
	mfAlg.init();
	mfAlg.run();
	return mfAlg.flowValue();
	//EdmondsKarp<ListDigraph, ListDigraph::ArcMap<int>>::FlowMap flowMap = mfAlg.flowMap;

}
void QFSolver::Solve()
{
	for (int i = 1; i < 15; ++i)
	{
		ExpandGraph(i);
		int flowValue = MaxFlow();
		std::cout << "Flow returned: " << flowValue << "\r\n";
	}
	//Try one iteration of karps algo//
	
}