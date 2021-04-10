#include "NeuralNetwork.hpp"

using namespace std;

int main(void) {
    try {
        vector<int> dims{3,3,1};
        NN::NeuralNetwork brain({1, 2, 1}, {NN::identity, NN::identity});
        auto nodes = brain.getAllNodes();
        for (size_t i = 0; i < nodes.size(); i++) {
            std::cout << "Node " << i + 1 << ": ";
            for (auto weight: nodes[i].getWeights()) {
                std::cout << weight << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "\n\n";
        brain.setInputs({1});
        brain.computeOutputs();
        nodes = brain.getAllNodes();
        for (size_t i = 0; i < nodes.size(); i++) {
            std::cout << "Node " << i + 1 << ": " << nodes[i].computeOutput() << std::endl;
        }
        std::cout << "\nFinal Output: " << brain.computeOutputs()[0];
    } catch (NN::NNException e) {
        std::cerr << e.describe();
    }
}