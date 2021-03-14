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

    //NN
    class NeuralNetwork {
        std::vector<Node> allNodes;
        std::vector<Node*> inputNodes;
        std::vector<Node*> outputNodes;
        std::vector<Node*> hiddenNodes;
        std::unordered_map<int, std::vector<int>> mapping;
        std::function<float (float)> activation; 

    public:
        NeuralNetwork(std::vector<int> dims, std::function<float (float)> activation = identity);
        NeuralNetwork(NeuralNetwork&);
        ~NeuralNetwork();

        const std::vector<Node>& getAllNodes();
        const std::vector<Node*>& getInputNodes();
        const std::vector<Node*>& getHiddenNodes();
        const std::vector<Node*>& getOutputNodes();
        std::vector<float> computeOutputs();
        void mutate(float rate);
    };

    class Node {
        std::vector<float> inputs;
        float output;
        std::vector<float> weights;

        friend class NeuralNetwork;
    public:
        Node(int size);
        Node(std::vector<float> inputs, std::vector<float> weights);
        Node(std::vector<float> inputs, float min, float max);
        Node(int size, float min, float max);
        Node(Node&);
        ~Node();

        void setInputs(std::vector<float> newInputs);
        const std::vector<float>& getInputs(); 
        float computeOutput();
        void mutate(float rate);
    };
}
