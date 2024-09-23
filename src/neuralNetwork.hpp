

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <array>
#include <list>
#include <vector>
#include <random>
#include <time.h>
#include <stdlib.h> 
#include "baseCharacter.hpp"

#pragma once

class Layer
{
private:
	int numNodesIn;
	int numNodesOut;
	double ActivationFunction(double Input)
	{
		return 1 / (1 + exp(-Input));
	}

public:
	std::vector<std::vector<double>> weights;
	std::vector<double> biases;
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
	std::vector<double> calcOutputs(std::vector<double> inputs)
	{
		std::vector<double> activations(numNodesIn);

		for (int nodeOut = 0; nodeOut < numNodesOut; nodeOut++)
		{
			double weightedInput = biases[nodeOut];
			for (int nodeIn = 0; nodeIn < numNodesIn; nodeIn++)
			{
				weightedInput += inputs[nodeIn] * weights[nodeIn][nodeOut];
			}
			activations[nodeOut] = ActivationFunction(weightedInput);
		}

		return activations;
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
		Randomize();
	}
	
	std::vector<double> CalcOutputs(std::vector<double> inputs)
	{
		for each (Layer layer in layers)
		{
			inputs = layer.calcOutputs(inputs);
		}
		return inputs;
	}

	void Randomize()
	{
		for each (Layer layer in layers)
		{
			std::vector<std::vector<double>> temp;
			for each (std::vector<double> weights in layer.weights)
			{
				std::vector<double> temp1;
				for each (double weight in weights)
				{
					temp1.push_back(rand() % 10 - 5);
				}
				temp.push_back(temp1);
			}
			layer.weights = temp;
			std::vector<double> temp1;
			for each (double bias in layer.biases)
			{
				temp1.push_back(rand() % 10 -5);
			}
			layer.biases = temp1;

		}
	}

};


