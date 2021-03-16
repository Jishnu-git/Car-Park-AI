#include "NN/NeuralNetwork.hpp"
#include "TCP_IP/CPP/NNSocket.hxx"
#include "json.hpp"

class Details {
public:
    float yPos;
    float pipeX;
    float pipeY;
    bool dead;

    Details() : yPos(0), pipeX(0), pipeY(0), dead(-1) {}
    Details(float yPos, float pipeX, float pipeY, bool dead) : yPos(yPos), pipeX(pipeX), pipeY(pipeY), dead(dead) {}
    Details(const Details &other) {
        yPos = other.yPos;
        pipeX = other.pipeX;
        pipeY = other.pipeY;
        dead = other.dead;
    }
    Details(std::string json) {
        try {
            auto args = nlohmann::json::parse(json);
            yPos = args["yPos"];
            pipeX = args["pipeX"];
            pipeY = args["pipeY"];
            dead = args["dead"];
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
        } catch (NNException e) {
            throw e;
        } catch ( ... ) {
            throw except("Error building details array from JSON");
        }
        
    }
};

class Bird {
    NN::NeuralNetwork brain;
    Details details;

public:
    Bird() : brain(std::vector<int>{3, 5, 1}, NN::fastSigmoid) {}
    Bird(std::function<float (float)> activation) : brain(std::vector<int>{3, 5, 1}, activation) {}
    Bird(std::vector<int> dims, std::function<float (float)> activation) : brain(dims, activation) {}

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

    bool computeOutput() {
        brain.setInputs(details.formatToInputs());
        return (brain.computeOutputs()[0] >= 0);
    }


};

//g++ Main.cpp -w -lws2_32 -lmswsock -ladvapi32
int main(void) {
    NNSocket socket;
    std::vector<Bird> birds(3);
    try {
        std::cout << "Initializing server...";
        socket.initServer();
        std::cout << "Done!\nWaiting for message...\n\n";
        while(true) {
            std::string detailsJSON{socket.recvMessage()};
            std::vector<Details> details = Details::fromJSON(detailsJSON);
            if (birds.size() != details.size()) {
                throw except("Number of entities and input are not the same");
            }

            nlohmann::json response;
            for (size_t i = 0; i < birds.size(); i++) {
                birds[i].setDetails(details[i]);
                response.push_back(birds[i].computeOutput());
            }
            std::cout << response << std::endl;

            socket.sendMessage(response.dump());
        }
    } catch (NNException exception) {
        std::cerr << exception.describe();
        socket.shutdownServer();
        return -1;
    } catch (std::exception e) {
        std::cerr << "Something went wrong!" << std::endl << e.what();
        socket.shutdownServer();
        return -1;
    } catch ( ... ) {
        std::cerr << "Unknown exception!";
        socket.shutdownServer();
        return -1;
    }
    std::cout << "\nShutting Down...";
    socket.shutdownServer();
    return 0;
}