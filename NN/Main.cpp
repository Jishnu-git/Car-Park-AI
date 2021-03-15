#include "NeuralNetwork.hpp"

using namespace std;

int main(void) {
    vector<int> dims{3,3,1};
    NN::NeuralNetwork brain(dims, NN::identity);
}