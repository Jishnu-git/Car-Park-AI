#pragma once
#include<fstream>
#include<iostream>
#include<sstream>
#include<iterator>
#include<vector>
#include "NeuralNetwork.hpp"

/*****************************************************************************Information****************************************************************
 * 
 * Write Data Generates 2 files -> filename_node and filename_map, _node stores the node weights and dimension and _map stores the mapping info
 * If moving the saved models place both the files in the same folder.
 * 
 * ******************************************************************************************************************************************************/

namespace NN {
class StoreData{
    
    std::string fname;
    std::fstream mapping_file;
    std::fstream node_file;

    public:
    StoreData(std::string filename){
        fname = filename;
    }
    ~StoreData(){
        if(mapping_file.is_open())
            mapping_file.close();

        if(node_file.is_open())
            node_file.close();
    }

    int writeData(NN::NeuralNetwork parent, bool overwrite = false) {
        try {
            if(overwrite) {
                node_file.open(fname+"_node", std::ios_base::out|std::ios_base::trunc);
                mapping_file.open(fname+"_map", std::ios_base::out|std::ios_base::trunc);
            } else {
                node_file.open(fname+"_node", std::ios_base::out|std::ios_base::app);
                mapping_file.open(fname+"_map", std::ios_base::out|std::ios_base::app);
            }

            //Store Mapping
            for(auto key : parent.getMapping()){
                mapping_file<<key.first<< " "; 
                for(auto val: key.second) {
                    mapping_file<<val<<" ";
                }
                mapping_file<<std::endl;
            }
            mapping_file.close();
            
            //Store Weights and Dimensions
            for(auto dim:parent.getDims()) {
                node_file<<dim<<" ";
            }
            node_file<<std::endl;
            mapping_file.close();
            
            for(auto Node: parent.getAllNodes()){
                node_file<<getActivationName(Node)<<" ";
                if(getActivationName(Node)  ==""){
                    std::cout<<"Error in Activation Function Name retr"<<std::endl;
                }
                for(auto weight: Node.getWeights()) {
                    node_file << weight<< " ";
                }
                node_file<<std::endl;
            }
            node_file.close();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
    NN::NeuralNetwork readData(){
        try {
            node_file.open(fname+"_node", std::ios_base::in);
            std::vector<NN::Node>nodes; //Node Vector
            std::vector<std::function<float (float)>> activations; //Activation Function Vector
            std::vector<std::vector<float>> NodeWeights; //All Nodes Weights Vector

            std::string tp;
            getline(node_file, tp);
            std::istringstream stri(tp);

            std::vector<int> dim((std::istream_iterator<int>(stri)), (std::istream_iterator<int>())); //Dimension Vector
            int i=0;
            while(i <= dim.size()){
                getline(node_file, tp);
                std::stringstream st(tp);          
                int j;
                if( i != 0)
                    j = dim[i-1];
                else
                    j = 1;
                std::vector<float>inputs(j,0); //Inputs Vector
                std::vector<float>weights; //Weights Vector
                std::string activation;
                st>>activation;
                std::cout<<activation<<" ";
                float weight;
                while(st >>weight) {
                    weights.push_back(weight);
                    std::cout<<weight<<" ";
                }
                std::cout<<std::endl;
                activations.push_back(returnActivation(activation));
                NodeWeights.push_back(weights);
                //nodes.push_back(NN::Node(inputs, weights, returnActivation(activation)));
                i++;
            }
            std::cout<<std::endl;
            NN::NeuralNetwork network (dim, activations);
            std::cout<<"Loaded Nodes and Dimensions\n";
            return network;
        }
        catch(const std::exception& e)
        {
            std::cerr <<"Error while reading Node File: "<<e.what() << '\n';
        }

        try{
            mapping_file.open(fname+"_map", std::ios_base::in);
            std::unordered_map<int, std::vector<int>> map;
            std::string line;
            
            while(!mapping_file.eof()) { 
                getline(mapping_file, line);
                std::stringstream sline(line);
                int key, value;
                std::vector<int> values;
                sline >> key;
                while(sline>>value){
                    values.push_back(value);
                }
                map.insert({key, values});
            }
        }
        catch(const std::exception& e)
        {
            std::cerr <<"Error while reading Map File"<<e.what() << '\n';
        }
        
        
    }
    std::string getActivationName(NN::Node node) {

        float(*const* ptr)(float) = node.getActivation().target<float(*)(float)>(); //Get Address of Function

        if(ptr && *ptr == NN::identity) {
            return "Identity";
        }
        if(ptr && *ptr == NN::fastSigmoid) {
            return "FastSigmoid";
        }
        if(ptr && *ptr == NN::LeakyReLU) {
            return "LeakyReLu";
        }
        if(ptr && *ptr == NN::ReLU) {
            return "ReLu";
        }
        if(ptr && *ptr == NN::tanInv) {
            return "TanInv";
        }
        if(ptr && *ptr == NN::sigmoid) {
            return "Sigmoid";
        }
        return "";
    }

    std::function<float (float)> returnActivation(std::string functionName){

        std::function <float (float)> activation = nullptr;
        if( functionName == "Identity") {
            activation = NN::identity;
        } else if (functionName == "FastSigmoid") {
            activation = NN::fastSigmoid;
        }else if (functionName == "LeakyReLu") {
            activation = NN::LeakyReLU;
        }else if (functionName == "ReLu") {
            activation = NN::ReLU;
        }else if (functionName == "TanInv") {
            activation = NN::tanInv;
        }else if (functionName == "Sigmoid") {
            activation = NN::sigmoid;
        }
        return activation;
    }
};
}
