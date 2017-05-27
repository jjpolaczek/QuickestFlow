#pragma once

#include <lemon\concepts\graph.h>
#include <lemon\concepts\maps.h>
#include <lemon\list_graph.h>
#include <lemon\euler.h>
#include <iostream>
#include <fstream>
#include <random>

class GraphGenerator
{
public:
	GraphGenerator()
	{

	}
	void Generate(std::string filename, int NodeCount);
};