#pragma once
#include<fstream>
#include<iostream>
#include<vector>
#include "NeuralNetwork.hpp"

namespace NN {
class StoreData{
    
    std::string fname;
    std::fstream file;

    public:
    StoreData(std::string filename){
        fname = filename;
    }
    ~StoreData(){
        if(file.is_open())
            file.close();
    }
    int writeData(NN::NeuralNetwork parent, bool overwrite = false){
        try{            
            if(overwrite){
                file.open(fname, std::ios_base::out|std::ios_base::trunc);
            }else{
                file.open(fname, std::ios_base::out|std::ios_base::app);    
            }
            file.write((char*)&parent, sizeof(parent));
            file.close();
        }
        catch(const std::exception& e){
            std::cerr << "Error writing data: " << e.what() << '\n';
        }
    }

    std::vector<NN::NeuralNetwork> readData(){
        try{
            file.open(fname, std::ios_base::in);
            //std::cout << "1";
            NN::NeuralNetwork network(std::vector<int>{9, 10, 10, 10, 5, 5}, std::vector<std::function<float (float)>>{NN::identity, NN::LeakyReLU, NN::LeakyReLU, NN::ReLU, NN::sigmoid});
            std::vector<NN::NeuralNetwork> vec;
            //std::cout << "2";
            while(!file.eof()){
                //std::cout << "\n2.1";
                file.read((char*)&network, sizeof(network));
                //std::cout << "\n2.2";
                for (auto dim : network.getDims()) {
                    std::cout << dim << std::endl;
                }
                //vec.push_back(network);
               // std::cout << "\n2.3";
            }
            //std::cout << "3";
            file.close();
            return vec;
    
        }
        catch(const std::exception& e){
            std::cerr << "Error reading: " << e.what() << '\n' << "File: " << fname << "\n";
            exit(-1);
        } 
    }
};
}
