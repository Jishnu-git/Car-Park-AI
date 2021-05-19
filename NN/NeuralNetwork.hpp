#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <functional>
#include "Exceptions.hpp"

namespace NN {
    //RNG for mutation
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);

    //Activation functions
    float identity(float x) {
        return x;
    }

    float sigmoid(float x) {
        return (1 / (1 + exp(-x)));
    }

    float fastSigmoid(float x) {
        return x / (1 + abs(x));
    }

    float tanInv(float x) {
        return tanh(x);
    }

    float ReLU(float x) {
        return std::max(static_cast<float>(0), x);
    }

    float LeakyReLU(float x) {
        return std::max(static_cast<float>(0.01) * x, x);
    }

    

    //NN
    class Node {
        std::vector<float> inputs;
        float output;
        std::vector<float> weights;
        std::function<float (float)> activation;

        friend class NeuralNetwork;
    public:
        Node(int size);
        Node(std::vector<float> inputs, std::vector<float> weights);
        Node(std::vector<float> inputs, float min, float max);
        Node(int size, float min, float max);
        Node(std::vector<float> inputs, std::vector<float> weights, std::function<float (float)> activation);
        Node(std::vector<float> inputs, float min, float max, std::function<float (float)> activation);
        Node(int size, float min, float max, std::function<float (float)> activation);
        Node(const Node&);
        ~Node();

        void setInputs(std::vector<float> newInputs);
        std::vector<float>& getWeights();
        const std::vector<float>& getInputs(); 
        const std::function<float (float)>& getActivation();
        float computeOutput();
        void mutate(float rate);
    };

    class NeuralNetwork {
        std::vector<Node> allNodes;
        std::vector<int> dims;
        std::unordered_map<int, std::vector<int>> mapping;
    public:
        NeuralNetwork(std::vector<int> dims, std::vector<std::function<float (float)>> activation);
        //NeuralNetwork(const NeuralNetwork&);
        ~NeuralNetwork();

        const std::vector<Node>& getAllNodes();
        const std::vector<Node>& getInputNodes();
        const std::vector<Node>& getHiddenNodes();
        const std::vector<Node>& getOutputNodes();
        const std::vector<int>& getDims();
        const std::unordered_map<int, std::vector<int>>& getMapping();
        std::vector<float> computeOutputs();
        void setInputs(std::vector<float> newInputs);
        void mutate(float rate);
        static std::vector<int> getRandomDims(int input, int output, int min, int max);
        static std::vector<NeuralNetwork> mutateFromNN(const NeuralNetwork& nn, int population, float rate);
    };
}

namespace NN {
#define MIN -1
#define MAX 1

float random(float min, float max) {
    return min + static_cast<float>(rng()) / static_cast<float>(rng.max() / (max - min));
}

float random() {
    return random(MIN, MAX);
}

/*=========================================================================
===============================NeuralNetwork===============================
===========================================================================*/
bool fun (bool (*arg)(char *)) {
    return true;
}

NeuralNetwork::NeuralNetwork(std::vector<int> dims, std::vector<std::function<float (float)>> activation) : dims(dims) {
    if (dims.size() < 3) {
        throw except("Dimension of Neural Network is less than 3.");
    }

    for (size_t i = 0; i < dims[0]; i++) {
        allNodes.emplace_back(1);
    }
    std::vector<int> prevLayer(dims[0]);
    std::iota(prevLayer.begin(), prevLayer.end(), 0);
    for (size_t i = 1; i < dims.size() - 1; i++) {
        for (size_t j = 0; j < dims[i]; j++) {
            allNodes.emplace_back(prevLayer.size(), MIN, MAX, activation[i - 1]);
            mapping[allNodes.size() - 1] = std::vector<int>(prevLayer);
        }
        int start = prevLayer.back() + 1;
        prevLayer.resize(dims[i]);
        std::iota(prevLayer.begin(), prevLayer.end(), start);
    }

    for (size_t i = 0; i < dims.back(); i++) {
        allNodes.emplace_back(prevLayer.size(), MIN, MAX, activation.back());
        mapping[allNodes.size() - 1] = std::vector<int>(prevLayer);
    }
}

// NeuralNetwork::NeuralNetwork(const NeuralNetwork& nn) {
//     dims = nn.dims;
//     allNodes = nn.allNodes;
//     mapping = nn.mapping;
// }

NeuralNetwork::~NeuralNetwork() {
    allNodes.clear();
    mapping.clear();
}

void NeuralNetwork::mutate(float rate) {
    for (size_t i = dims[0]; i < allNodes.size(); i++) {
        allNodes[i].mutate(rate);
    }
}

std::vector<float> NeuralNetwork::computeOutputs() {
    for (size_t i = 0; i < dims[0]; i++) {
        allNodes[i].output = allNodes[i].inputs[0];
    }

    int offset = dims[0];
    for (size_t i = 1; i < dims.size() - 1; i++) {
        for (size_t j = 0; j < dims[i]; j++) {
            std::vector<float> inputs;
            for (auto k : mapping[j + offset]) {
                inputs.push_back(allNodes[k].output);
            }
            allNodes[j + offset].setInputs(inputs);
            allNodes[j + offset].computeOutput();
        }
        offset += dims[i];
    }
    

    std::vector<float> output;
    for (size_t i = 0; i < dims.back(); i++) {
        std::vector<float> inputs;
        for (auto j : mapping[i + offset]) {
            inputs.push_back(allNodes[j].output);
        }
        allNodes[i + offset].setInputs(inputs);
        allNodes[i + offset].computeOutput();
        output.push_back(allNodes[i + offset].output);
    }

    return output;
}

void NeuralNetwork::setInputs(std::vector<float> newInputs) {
    if (dims[0] != newInputs.size()) {
        throw except("Input vector dimension does not match the dimensions of neural network");
        return;
    }
    for (size_t i = 0; i < dims[0]; i++) {
        allNodes[i].setInputs(std::vector<float>{newInputs[i]});
    }
}

std::vector<int> NeuralNetwork::getRandomDims(int input, int output, int min = 3, int max = 5) {
    int size = static_cast<int>(round(NN::random(3, 5)));
    std::vector<int> dims(size);
    dims[0] = 3;
    dims[size - 1] = 1;
    for (size_t i = 1; i < size - 1; i++) {
        dims[i] = static_cast<int>(round(NN::random(3, 7)));
    }
    return dims;
}

std::vector<NeuralNetwork> NeuralNetwork::mutateFromNN(const NeuralNetwork& nn, int population, float rate = 0.1) {
    std::vector<NeuralNetwork> nextGen;
    nextGen.push_back(nn);
    for (int _ = 1; _ < population; _++) {
        NeuralNetwork newNN(nn);
        newNN.mutate(rate);
        nextGen.push_back(newNN);
    }
    return nextGen;
}


const std::vector<int>& NeuralNetwork::getDims() {
    return dims;
}

const std::vector<Node>& NeuralNetwork::getAllNodes() {
    return allNodes;
}

const std::vector<Node>& NeuralNetwork::getInputNodes() {
    return std::vector<Node>(allNodes.begin(), allNodes.begin() + dims[0]);
}

const std::vector<Node>& NeuralNetwork::getHiddenNodes() {
    return std::vector<Node>(allNodes.begin() + dims[0], allNodes.begin() + dims[1]);
}

const std::vector<Node>& NeuralNetwork::getOutputNodes() {
    return std::vector<Node>(allNodes.begin() + dims[1], allNodes.begin() + dims[2]);
}

const std::unordered_map<int, std::vector<int>>& NeuralNetwork::getMapping() {
    return mapping;
}


/*=========================================================================
====================================Node===================================
===========================================================================*/

Node::Node(int size) : inputs(size, 0), weights(size, 1), activation(identity) {}

Node::Node(const Node& newNode) {
    inputs = newNode.inputs;
    weights = newNode.weights;
    output = newNode.output;
    activation = newNode.activation;
}

Node::Node(std::vector<float> inputs, std::vector<float> weights) : inputs(inputs), weights(weights), activation(identity) {}

Node::Node(std::vector<float> inputs, std::vector<float> weights, std::function<float (float)> activation) : inputs(inputs), weights(weights), activation(activation) {}

Node::Node(std::vector<float> inputs, float min, float max) : Node(inputs, min, max, identity) {}

Node::Node(std::vector<float> inputs, float min, float max, std::function<float (float)> activation) : inputs(inputs), activation(activation) {
    weights.clear();
    for (size_t i = 0; i < inputs.size(); i++) {
        weights.push_back(random(min, max));
    }
}

Node::Node(int size, float min, float max) : Node(size, min, max, identity) {}

Node::Node(int size, float min, float max, std::function<float (float)> activation) : inputs(size), activation(activation) {
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

std::vector<float>& Node::getWeights(){
    return weights;
}

const std::vector<float>& Node::getInputs() {
    return inputs;
}

const std::function<float (float)>& Node::getActivation(){
    return activation;
}

float Node::computeOutput() {
    float out = 0;
    for (size_t i = 0; i < inputs.size(); i++) {
        out += weights[i] * inputs[i];
    }
    out = activation(out);
    output = out;
    return out;
}

void Node::mutate(float rate) {
    for (size_t i = 0; i < weights.size(); i++) {
        if (random(0, 1) < rate) {
            weights[i] += random(-1, 1);
        }
    }
}
}
