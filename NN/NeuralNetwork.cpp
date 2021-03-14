#include "NeuralNetwork.hpp"

using namespace NN;
#define except(message) NNException(__FILE__, __LINE__, message)
#define MIN -10
#define MAX 10

float random(float min, float max) {
    return min + static_cast<float>(rng()) / static_cast<float>(rng.max() / (max - min));
}

float random() {
    return random(MIN, MAX);
}

/*=========================================================================
===============================NeuralNetwork===============================
===========================================================================*/

NeuralNetwork::NeuralNetwork(std::vector<int> dims, std::function<float (float)> activation = identity) : activation(activation) {
    if (dims.size() < 3) {
        throw except("Dimension of Neural Network is less than 3.");
    }

    for (size_t i = 0; i < dims[0]; i++) {
        allNodes.emplace_back(1);
        inputNodes.push_back(&allNodes[i]);
    }
    
    std::vector<int> prevLayer(dims[0]);
    std::iota(prevLayer.begin(), prevLayer.end(), 0);
    for (size_t i = 1; i < dims.size() - 1; i++) {
        for (size_t j = 0; j < dims[i]; j++) {
            allNodes.emplace_back(prevLayer.size(), MIN, MAX);
            hiddenNodes.push_back(&(allNodes.back()));
            mapping[allNodes.size() - 1] = std::vector(prevLayer);
        }
        int start = prevLayer.back() + 1;
        prevLayer.resize(dims[i]);
        std::iota(prevLayer.begin(), prevLayer.end(), start);
    }

    for (size_t i = 0; i < dims.back(); i++) {
        allNodes.emplace_back(prevLayer.size(), MIN, MAX);
        outputNodes.push_back(&(allNodes.back()));
        mapping[allNodes.size() - 1] = std::vector(prevLayer);
    }
}

NeuralNetwork::~NeuralNetwork() {
    allNodes.clear();
    inputNodes.clear();
    hiddenNodes.clear();
    outputNodes.clear();
    mapping.clear();
}

void NeuralNetwork::mutate(float rate) {
    for (size_t i = inputNodes.size(); i < allNodes.size(); i++) {
        allNodes[i].mutate(rate);
    }
}


std::vector<float> NeuralNetwork::computeOutputs() {
    for (size_t i = 0; i < inputNodes.size(); i++) {
        allNodes[i].output = allNodes[i].inputs[0];
    }

    for (size_t i = 0; i < hiddenNodes.size(); i++) {
        hiddenNodes[i] -> computeOutput();
    }

    for (size_t i = 0; i < outputNodes.size(); i++) {
        outputNodes[i] -> output = activation(outputNodes[i] -> computeOutput());
    }
}



const std::vector<Node>& NeuralNetwork::getAllNodes() {
    return allNodes;
}

const std::vector<Node*>& NeuralNetwork:: getInputNodes() {
    return inputNodes;
}

const std::vector<Node*>& NeuralNetwork:: getHiddenNodes() {
    return hiddenNodes;
}

const std::vector<Node*>& NeuralNetwork:: getOutputNodes() {
    return outputNodes;
}


/*=========================================================================
====================================Node===================================
===========================================================================*/

Node::Node(int size) : inputs(size, 0), weights(size, 1) {}

Node::Node(Node& newNode) {
    inputs = newNode.inputs;
    weights = newNode.weights;
    output = newNode.output;
}

Node::Node(std::vector<float> inputs, std::vector<float> weights) : inputs(inputs), weights(weights) {}

Node::Node(std::vector<float> inputs, float min, float max) : inputs(inputs) {
    weights.clear();
    for (size_t i = 0; i < inputs.size(); i++) {
        weights.push_back(random(min, max));
    }
}

Node::Node(int size, float min, float max) : inputs(size) {
    weights.clear();
    for (size_t i = 0; i < inputs.size(); i++) {
        weights.push_back(random(min, max));
    }
}

Node::~Node() {
    inputs.clear();
    weights.clear();
}

void Node::setInputs(std::vector<float> newInputs) {
    inputs = newInputs;
}

const std::vector<float>& Node::getInputs() {
    return inputs;
}

float Node::computeOutput() {
    float out = 0;
    for (size_t i = 0; i < inputs.size(); i++) {
        out += weights[i] * inputs[i];
    }
    output = out;
    return out;
}

void Node::mutate(float rate) {
    for (size_t i = 0; i < weights.size(); i++) {
        if (random(0, 1) < rate) {
            weights[i] *= random(-2, 2);
        }
    }
}
