#include "QFSolver.h"

using std::ifstream;
using std::ofstream;
using namespace lemon;
void QFSolver::LoadGraph(std::string filename)
{
	ifstream file;
	file.open(filename, std::ios::in);
	if (file.is_open())
	{
		maxCapacity_ = 0;
		maxDelay_ = 0;
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
		flowValue_ = F = std::stoi(line, &sz);
		line = line.substr(sz);
		arc_count = std::stoi(line, &sz);
		line = line.substr(sz);
		node_count = std::stoi(line, &sz);
		//std::cout << "Loading graph, source: " << source << ", sink: " << target << ", flow: " << F << "\r\n";
		//std::cout << "Nodes: " << node_count << ", Arcs: " << arc_count << "\r\n\r\n";
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
			//std::cout << "Arc parameters: " << arc.vi << "-" << arc.vj << ", capacity: " << arc.capacity << ", delay: " << arc.delay << "\r\n";
			tmpArc.push_back(arc);
			//Max values//
			if (arc.capacity > maxCapacity_)
				maxCapacity_ = arc.capacity;
			if (arc.delay > maxDelay_)
				maxDelay_ = arc.delay;
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
void QFSolver::MaxFlowMap(unsigned int horizon)
{

	ExpandGraph(horizon);
	//Sanity checks//
	if (teGraph_ == nullptr || teMap_ == nullptr)
		throw std::exception("No time expanded network");
	//Calculate source and target nodes//
	int source = sourceID_;
	int target = targetID_ + (horizon_ - 1) * (baseGraph_->maxNodeId() + 1); //Through time expanded network//

	ListDigraph::Node sNode = teGraph_->nodeFromId(source);
	ListDigraph::Node tNode = teGraph_->nodeFromId(target);
	EdmondsKarp<ListDigraph, ListDigraph::ArcMap<int>> mfAlg((*teGraph_),(*teMap_),sNode, tNode);
	mfAlg.init();
	mfAlg.run();
	//This can be moved somwhere else//
	const EdmondsKarp<ListDigraph, ListDigraph::ArcMap<int>>::FlowMap &flowMap = mfAlg.flowMap();
	if (flowMap_ != nullptr)
	{
		delete flowMap_;
		flowMap_ = nullptr;
	}
	//Copy calculated flowmap from the algorithm//
	flowMap_ = new ListDigraph::ArcMap<int>(*teGraph_);
	int arcCount = teGraph_->maxArcId() + 1;
	for (int i = 0; i < arcCount; ++i)
	{
		ListDigraph::Arc arc = teGraph_->arcFromId(i);
		(*flowMap_)[arc] = flowMap[arc];
	}
}
int QFSolver::MaxFlow(unsigned int horizon)
{

	ExpandGraph(horizon);
	//Sanity checks//
	if (teGraph_ == nullptr || teMap_ == nullptr)
		throw std::exception("No time expanded network");
	//Calculate source and target nodes//
	int source = sourceID_;
	int target = targetID_ + (horizon_ - 1) * (baseGraph_->maxNodeId() + 1); //Through time expanded network//

	ListDigraph::Node sNode = teGraph_->nodeFromId(source);
	ListDigraph::Node tNode = teGraph_->nodeFromId(target);
	EdmondsKarp<ListDigraph, ListDigraph::ArcMap<int>> mfAlg((*teGraph_), (*teMap_), sNode, tNode);
	mfAlg.init();
	mfAlg.run();

	return mfAlg.flowValue();

}
void QFSolver::Solve()
{
	//Binary search//
	int lh = 0, uh = maxDelay_ + maxCapacity_ / flowValue_;
	//std::cout << "Lower bound:  "<< lh << ", Upper bound: " << uh << "\r\n";
	//Expand upper bound if needed//
	while (MaxFlow(uh) < flowValue_)
	{
		lh = uh;
		uh *= 2;

	//	std::cout << "Lower bound:  " << lh << ", Upper bound: " << uh << "\r\n";
	}
	//Search until 1 epsilion//
	int flow;
	while(lh != uh && lh != uh -1)
	{
		int middle = (uh + lh) / 2;
		flow = MaxFlow(middle);
		if (flow < flowValue_)
			lh = middle;
		else
			uh = middle;
	//	std::cout << "Flow returned: " << flow << "\r\n";
	//	std::cout << "Lower bound:  " << lh << ", Upper bound: " << uh << "\r\n";
	}
	//std::cout << "Quickest flow time horizon returned: " << uh << "\r\n";
	MaxFlowMap(uh);
	unitsSent_ = flow;
	timeUsed_ = uh;
}
void QFSolver::SaveResults(std::string filename)
{
	ofstream file;
	file.open(filename, std::ios::out | std::ios::trunc);
	if (file.is_open())
	{
		file << "Flow from node id: " << sourceID_ << " to node id: " << targetID_ << "\r\n";
		file << "Units sent: " << unitsSent_ << " Time used: " << timeUsed_ << "\r\n";
		file << "czas\tvi\tvj\tF" << "\r\n";
		if (teGraph_ == nullptr || flowMap_ == nullptr)
			throw std::exception("Invalid Operaton - optimization not performed");
		int arcCount = teGraph_->maxArcId() + 1;
		int sliceCount = baseGraph_->maxNodeId() + 1;
		for (int i = timeUsed_ * sliceCount; i < arcCount; ++i)
		{
			ListDigraph::Arc arc = teGraph_->arcFromId(i);
			int value = (*flowMap_)[arc];
			int source = teGraph_->id(teGraph_->source(arc));
			int target = teGraph_->id(teGraph_->target(arc));
			if(value != 0/* && source % sliceCount != target % sliceCount*/)
				file << (i- timeUsed_ * sliceCount) / sliceCount << "\t" << source % sliceCount << "\t" << target% sliceCount << "\t" << value<<"\r\n";
		}
		file.close();
	}
	else
	{
		throw std::invalid_argument("Invalid file path");
	}
}