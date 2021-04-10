#include "NN/NeuralNetwork.hpp"
#include "TCP_IP/CPP/NNSocket.hxx"
#include "json.hpp"

class Details {
public:
    float yPos;
    float pipeX;
    float pipeY;
    bool dead;
    int score;

    Details() : yPos(0), pipeX(0), pipeY(0), dead(-1), score(0) {}
    Details(float yPos, float pipeX, float pipeY, bool dead, int score) : yPos(yPos), pipeX(pipeX), pipeY(pipeY), dead(dead), score(score) {}
    Details(const Details &other) {
        yPos = other.yPos;
        pipeX = other.pipeX;
        pipeY = other.pipeY;
        dead = other.dead;
        score = other.score;
    }
    Details(std::string json) {
        try {
            auto args = nlohmann::json::parse(json);
            yPos = args["yPos"];
            pipeX = args["pipeX"];
            pipeY = args["pipeY"];
            dead = args["dead"];
            score = args["score"];
        } catch ( ... ) {
            throw except("Error building the details from json");
        }
    }

    Details(nlohmann::json args) {
        try {
            yPos = args["yPos"];
            pipeX = args["pipeX"];
            pipeY = args["pipeY"];
            dead = args["dead"];
            score = args["score"];
        } catch ( ... ) {
            throw except("Error building the details from json");
        }
    }

    std::vector<float> formatToInputs() {
        return std::vector<float>{yPos, pipeX, pipeY};
    } 

    static std::vector<Details> fromJSON(std::string json) {
        try {
            std::vector<Details> out;
            auto args = nlohmann::json::parse(json);
            for (auto arg : args) {
                out.emplace_back(arg);
            }
            return out;
        } catch (NN::NNException e) {
            throw e;
        } catch ( ... ) {
            std::cout << json << std::endl;
            throw except("Error building details array from JSON");
        }
        
    }
};

class Bird {
    NN::NeuralNetwork brain;
    Details details;

public:
    Bird() : brain(std::vector<int>{3, 10, 10, 5, 1}, std::vector<std::function<float (float)>>{NN::identity, NN::ReLU, NN::ReLU, NN::sigmoid}) {}
    Bird(NN::NeuralNetwork brain) : brain(brain) {}
    Bird(std::vector<std::function<float (float)>> activation) : brain(std::vector<int>{3, 10, 10, 5, 1}, activation) {}
    Bird(std::vector<int> dims, std::vector<std::function<float (float)>> activation) : brain(dims, activation) {}

    void setDetails(Details det) {
        details = det;
    }

    void setDetails(nlohmann::json json) {
        details = Details(json);
    }

    void setDetails(std::string json) {
        details = Details(json);
    }

    const Details& getDetails() {
        return details;
    }

    const NN::NeuralNetwork& getNeuralNetwork() {
        return brain;
    }

    bool computeOutput() {
        brain.setInputs(details.formatToInputs());
        return (brain.computeOutputs()[0] >= 0.5);
    }


};

//g++ Flappy.cpp -w -lws2_32 -lmswsock -ladvapi32
int main(void) {
    NNSocket socket;
    int numBirds = 100, gen = 1, prevScore = -1;
    std::vector<Bird> birds(numBirds);
    Bird prevParent;
    try {
        std::cout << "Initializing server...";
        socket.initServer();
        std::cout << "Done!\nWaiting for message...\n\n";

        while(true) {
            std::string detailsJSON{socket.recvMessage()};
            if (detailsJSON == "close") {
                break;
            }
            std::vector<Details> details = Details::fromJSON(detailsJSON);
            if (birds.size() != details.size()) {
                std::cout << birds.size() << " " << details.size() << std::endl;
                throw except("Number of entities and input are not the same");
            }

            nlohmann::json response;
            int deadBirds = 0;
            int maxInd = 0;
            for (size_t i = 0; i < birds.size(); i++) {
                birds[i].setDetails(details[i]);
                if (birds[i].getDetails().dead) {
                    deadBirds++;
                    if (birds[i].getDetails().score >= birds[maxInd].getDetails().score) {
                        maxInd = i;
                    }
                    response.push_back(false);
                } else {
                    response.push_back(birds[i].computeOutput());
                }    
            }
            if (deadBirds == numBirds) {
                gen++;
                Bird parentA = birds[maxInd];
                Bird parentB = prevParent; 
                if (prevScore > parentA.getDetails().score) {
                    parentA = prevParent;
                    parentB = birds[maxInd];
                }
                //*(std::max_element(birds.begin(), birds.end(), [](Bird bird1, Bird bird2) {
                //     if (bird1.getDetails().score == bird2.getDetails().score) {
                //         return 0;
                //     } else if (bird1.getDetails().score < bird2.getDetails().score) {
                //         return -1;
                //     } else {
                //         return 1;
                //     }
                // }));
                std::vector<NN::NeuralNetwork> newBrainsA = NN::NeuralNetwork::mutateFromNN(parentA.getNeuralNetwork(), round(numBirds * 0.75), 0.15);
                std::vector<NN::NeuralNetwork> newBrainsB = NN::NeuralNetwork::mutateFromNN(parentB.getNeuralNetwork(), round(numBirds * 0.25), 0.20);

                birds.clear();
                for (int i = 0; i < newBrainsA.size(); i++) {
                    birds.push_back(Bird(newBrainsA[i]));
                }
                for (int i = 0; i < newBrainsB.size(); i++) {
                    birds.push_back(Bird(newBrainsB[i]));
                }
                std::cout << "mutating, generation#: " << gen << " scores: " << parentA.getDetails().score << ", " << parentB.getDetails().score << std::endl;
                prevScore = parentA.getDetails().score;
                prevParent = parentA;   
            }
            // std::cout << response << " " << deadBirds << " " << numBirds << std::endl;

            socket.sendMessage(response.dump());
        }

    } catch (NN::NNException e) {
        std::cerr << e.describe();
        socket.shutdownServer();
        return -1;

    }

    std::cout << "\nShutting Down...";
    socket.shutdownServer();
    return 0;
}