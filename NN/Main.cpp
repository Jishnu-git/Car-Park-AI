#include "NeuralNetwork.hpp"
#include "StoreNN.hpp"

using namespace std;


void displayNodeData(vector <NN::Node> nodes){
    std::cout<<"\nDisplaying Node Data\n";
    for (size_t i = 0; i < nodes.size(); i++) {
        std::cout << "Node " << i + 1 << ": ";
        cout<<"Weights"<<": ";
        for (auto weight: nodes[i].getWeights()) {
            std::cout << weight << " ";
        }
    }
}

void displayMappingData(unordered_map<int, vector<int>> map) {
    for(auto key: map){
        cout<< key.first <<": ";
        for(auto inp: key.second) {
            cout<< inp << " ";
        }
        cout<<endl;
    }
}


int main(void) {
    try {
        vector<int> dims{3,3,1};
        NN::NeuralNetwork brain({1, 2, 1}, {NN::identity, NN::identity});
        NN::StoreData st("test");
        st.writeData(brain, true);
        NN::NeuralNetwork newNetwork =  st.readData();
        auto nodes = brain.getAllNodes();
        // displayNodeData(nodes);
        // auto newNodes = newNetwork.getAllNodes();
        // displayNodeData(newNodes);
        // auto map = brain.getMapping();
        // // displayMappingData(map);
        brain.setInputs({1});
        brain.computeOutputs();
        nodes = brain.getAllNodes();
        for (size_t i = 0; i < nodes.size(); i++) {
            std::cout << "Node " << i + 1 << ": " << nodes[i].computeOutput() << std::endl;
        }
        std::cout << "\nFinal Output: " << brain.computeOutputs()[0];        
        std::cout<<"New Network"<<std::endl;
        newNetwork.setInputs({1});
        newNetwork.computeOutputs();
        nodes = newNetwork.getAllNodes();
        for (size_t i = 0; i < nodes.size(); i++) {
            std::cout << "Node " << i + 1 << ": " << nodes[i].computeOutput() << std::endl;
        }
        std::cout << "\nFinal Output: " << newNetwork.computeOutputs()[0];
    } catch (NN::NNException e) {
        std::cerr << e.describe();
    }
}