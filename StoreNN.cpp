#include<fstream>
#include<iostream>
#include<vector>
#include "NN/NeuralNetwork.hpp"
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
    int writeData(NN::NeuralNetwork parent1, bool overwrite){
        try{            
            if(overwrite){
                file.open(fname, std::ios_base::out|std::ios_base::trunc);
            }else{
                file.open(fname, std::ios_base::out|std::ios_base::app);    
            }
            file.write((char*)&parent1, sizeof(parent1));
            file.close();
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
        }
    }
    std::vector<NN::NeuralNetwork> readData(){
        try{
            file.open(fname, std::ios_base::in);
            NN::NeuralNetwork *network;
            std::vector<NN::NeuralNetwork> vec;
            while(!file.eof()){
                file.read((char*)network, sizeof(*network));
                vec.emplace_back(*network);
            }
            delete network;
            return vec;
    
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
            return;
        }
        
    }
};
