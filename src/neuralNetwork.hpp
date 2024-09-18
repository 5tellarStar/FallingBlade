#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <array>
#include "baseCharacter.hpp"



class Layer
{
public:
	Layer(int nodesIn, int nodesOut) 
	{
		numNodesIn = nodesIn;
		numNodesOut = nodesOut;

		weights = new double[numNodesIn, numNodesOut];
		biases = new double[numNodesOut];
	}
	
private:
	int numNodesIn;
	int numNodesOut;
	double weights[,];
	double biases[];

};

