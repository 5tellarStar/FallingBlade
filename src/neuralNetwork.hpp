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
	std::vector<std::vector<double>> weights;
	std::vector<double> biases;


public:
	Layer(int nodesIn, int nodesOut) 
	{
		numNodesIn = nodesIn;
		numNodesOut = nodesOut;

		weights.reserve(numNodesIn);
		for each ( std::vector<double> vect in weights)
		{
			vect.reserve(numNodesOut);
		}
		biases.reserve(numNodesOut);
	}
	std::vector<double> calcWeightedOutputs(std::vector<double> inputs)
	{
		std::vector<double> weightedInputs(numNodesIn);

		for (int nodeOut = 0; nodeOut < numNodesOut; nodeOut++)
		{
			double weightedInput = biases[nodeOut];
			for (int nodeIn = 0; nodeIn < numNodesIn; nodeIn++)
			{
				weightedInput += inputs[nodeIn] * weights[nodeIn][nodeOut];
			}
			weightedInputs[nodeOut] = weightedInput;
		}

		return weightedInputs;
	}
	
};

class NeuralNetwork
{
private:
	std::vector<Layer> layers;

public:
	NeuralNetwork(std::vector<int> layerSizes) 
	{
		layers.reserve(layerSizes.size() - 1);
		for (int i = 0; i < layers.size(); i++)
		{
			layers[i] = Layer(layerSizes[i], layerSizes[i + 1]);
		}
	}


};

