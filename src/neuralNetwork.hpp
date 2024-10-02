

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
		for (int i = 0; i < numNodesIn; i++)
		{
			std::vector<double> weight;
			for (int i = 0; i < numNodesOut; i++)
			{
				weight.push_back(0);
			}
			weights.push_back(weight);
		}
		for (int i = 0; i < numNodesOut; i++)
		{
			biases.push_back(0);
		}
	}
	std::vector<double> calcOutputs(std::vector<double> inputs)
	{
		std::vector<double> activations;

		for (int nodeOut = 0; nodeOut < numNodesOut; nodeOut++)
		{
			double weightedInput = biases[nodeOut];
			for (int nodeIn = 0; nodeIn < numNodesIn; nodeIn++)
			{
				weightedInput += inputs[nodeIn] * weights[nodeIn][nodeOut];
			}
			activations.push_back(ActivationFunction(weightedInput));
		}

		return activations;
	}
	
};

class NeuralNetwork
{
private:
	std::vector<Layer> layers;

public:
	std::vector<std::vector<sf::CircleShape>> nodes;

	NeuralNetwork(std::vector<int> layerSizes) 
	{
		for (int i = 0; i < layerSizes.size() - 1; i++)
		{
			layers.push_back(Layer(layerSizes[i], layerSizes[i + 1]));
			nodes.push_back(std::vector<sf::CircleShape>{});

			for (int j = 0; j < layerSizes[i]; j++)
			{
				nodes[i].push_back(sf::CircleShape(2.5f, 10));
				nodes[i][j].setPosition(sf::Vector2f(i * 10, j * 10));
			}

		}

		nodes.push_back(std::vector<sf::CircleShape>{});
		for (int j = 0; j < layerSizes[4]; j++)
		{
			nodes[4].push_back(sf::CircleShape(2.5f, 10));
			nodes[4][j].setPosition(sf::Vector2f(4 * 10, j * 10));
		}

		Randomize();
	}
	
	std::vector<double> CalcOutputs(std::vector<double> inputs)
	{
		for (int i = 0; i < layers.size()-1; i++)
		{
			for (int j = 0; j < inputs.size()-1; j++)
			{
				if (inputs[j] > 0)
				{
					nodes[i][j].setFillColor(sf::Color::Green);
				}
				else
				{
					nodes[i][j].setFillColor(sf::Color::Red);
				}
			}
			inputs = layers[i].calcOutputs(inputs);
		}
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
					temp1.push_back(0);
				}
				temp.push_back(temp1);
			}
			layer.weights = temp;
			std::vector<double> temp1;
			for each (double bias in layer.biases)
			{
				temp1.push_back(1000);
			}
			layer.biases = temp1;

		}
	}

};


