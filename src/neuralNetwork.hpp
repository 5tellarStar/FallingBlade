#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <array>
#include <list>
#include <vector>
#include "baseCharacter.hpp"



class Layer
{
private:
	int numNodesIn;
	int numNodesOut;
	double weights[2];
	double biases[];


public:
	Layer(int nodesIn, int nodesOut) 
	{
		numNodesIn = nodesIn;
		numNodesOut = nodesOut;

		weights = new double[numNodesIn, numNodesOut];
		biases = new double[numNodesOut];
	}
	std::list<double> calcWeightedOutputs(std::list<double> inputs)
	{
		std::list<double> weightedInputs(numNodesOut);


	}
	
};

