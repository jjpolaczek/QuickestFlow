#pragma once

#include <lemon\concepts\graph.h>
#include <lemon\concepts\maps.h>
#include <lemon\list_graph.h>
#include <lemon\euler.h>
#include <iostream>
#include <fstream>
#include <random>
//!  Flow network generator
/*!
	Class encapsulating flow network generation method 
*/
class GraphGenerator
{
public:
	GraphGenerator()
	{

	}
	//!  Generate
	/*!
		Crates and saves a file named filename containing random graph with nodeCount
		ammount of nodes
	*/
	void Generate(std::string filename, int NodeCount);
};