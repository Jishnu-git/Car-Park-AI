#include "NN/NeuralNetwork.hpp"
#include "TCP_IP/CPP/NNSocket.hxx"
#include "json.hpp"

static float goalX;
static float goalZ;

class Details {
public:
    float carX;
    float carZ;
    float leftObs;
    float rightObs;
    float frontObs;
    float backObs;;
    float speed;
    float rotation;
    bool dead;
    int score;

    Details() : score(-1), dead(false) {}
    Details(float carX, float carZ, float leftObs, float rightObs, float frontObs, float backObs, float speed, float rotation, bool dead, int score) : 
        carX(carX), carZ(carZ), 
        leftObs(leftObs),
        rightObs(rightObs),
        frontObs(frontObs),
        backObs(backObs),
        speed(speed),
        rotation(rotation),
        dead(dead), score(score) {}
        
    Details(const Details &other) {
        carX = other.carX;
        carZ = other.carZ;
        leftObs = other.leftObs;
        rightObs = other.rightObs;
        frontObs = other.frontObs;
        backObs = other.backObs;
        speed = other.speed;
        rotation = other.rotation;
        dead = other.dead;
        score = other.score;
    }
    Details(std::string json) {
        try {
            auto args = nlohmann::json::parse(json);
            carX = args["cX"];
            carZ = args["cZ"];
            leftObs = args["lO"];
            rightObs = args["rO"];
            frontObs = args["fO"];
            backObs = args["bO"];
            speed = args["s"];
            rotation = args["r"];
            dead = args["d"];
            score = args["sc"];
        } catch ( ... ) {
            throw except("Error building the details from json");
        }
    }

    Details(nlohmann::json args) {
        try {
            carX = args["cX"];
            carZ = args["cZ"];
            leftObs = args["lO"];
            rightObs = args["rO"];
            frontObs = args["fO"];
            backObs = args["bO"];
            speed = args["s"];
            rotation = args["r"];
            dead = args["d"];
            score = args["sc"];
        } catch ( ... ) {
            throw except("Error building the details from JSON");
        }
    }

    std::vector<float> formatToInputs() {
        return std::vector<float> {
            carX, carZ, 
            goalX, goalZ, 
            leftObs, 
            rightObs,
            frontObs, 
            backObs, 
            speed
            };
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

    static void setGoal(std::string json) {
        try {
            auto goal = nlohmann::json::parse(json);
            goalX = goal["goalX"];
            goalZ = goal["goalZ"];
        } catch ( ... ) {
            std::cout << json << std::endl;
            throw except("Error parsing goal position from JSON string");
        }
        
    }
};

class Car {
    NN::NeuralNetwork driver;
    Details details;

public:
    Car() : driver(std::vector<int>{9, 10, 10, 10, 5, 5}, std::vector<std::function<float (float)>>{NN::identity, NN::LeakyReLU, NN::LeakyReLU, NN::ReLU, NN::sigmoid}) {}
    Car(NN::NeuralNetwork driver) : driver(driver) {}
    Car(std::vector<std::function<float (float)>> activation) : driver(std::vector<int>{9, 10, 10, 10, 5, 5}, activation) {}
    Car(std::vector<int> dims, std::vector<std::function<float (float)>> activation) : driver(dims, activation) {}

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
        return driver;
    }

    int computeOutput() {
        driver.setInputs(details.formatToInputs());
        auto outputs = driver.computeOutputs();
        int output = 0;
        int inc = 1;
        for (auto rawOutput : outputs) {
            if (rawOutput >= 0.5) {
                output += inc;
            }
            inc *= 2;
        }
        return output;
    }
};

//g++ Car.cpp -w -lws2_32 -lmswsock -ladvapi32
int main(void) {
    NNSocket socket;
    int numCars = 50, gen = 1, prevScore = -1;
    std::vector<Car> Cars(numCars);
    Car prevParent;
    try {
        std::cout << "Initializing server...";
        socket.initServer();
        std::cout << "Done!\nWaiting for message...\n\n";
        Details::setGoal(std::string(socket.recvMessage()));

        while(true) {
            std::string detailsJSON{socket.recvMessage()};
            if (detailsJSON == "close") {
                break;
            }
            std::vector<Details> details = Details::fromJSON(detailsJSON);
            if (Cars.size() != details.size()) {
                std::cout << Cars.size() << " " << details.size() << std::endl;
                throw except("Number of entities and input are not the same");
            }

            nlohmann::json response;
            int deadCars = 0;
            int maxInd = 0;
            for (size_t i = 0; i < Cars.size(); i++) {
                Cars[i].setDetails(details[i]);
                if (Cars[i].getDetails().dead) {
                    deadCars++;
                    if (Cars[i].getDetails().score >= Cars[maxInd].getDetails().score) {
                        maxInd = i;
                    }
                    response.push_back(0);
                } else {
                    response.push_back(Cars[i].computeOutput());
                }    
            }
            if (deadCars == numCars) {
                gen++;
                Car parentA = Cars[maxInd];
                Car parentB = prevParent; 
                if (prevScore > parentA.getDetails().score) {
                    parentA = prevParent;
                    parentB = Cars[maxInd];
                }

                std::vector<NN::NeuralNetwork> newdriversA = NN::NeuralNetwork::mutateFromNN(parentA.getNeuralNetwork(), ceil(numCars * 0.75), 0.15);
                std::vector<NN::NeuralNetwork> newdriversB = NN::NeuralNetwork::mutateFromNN(parentB.getNeuralNetwork(), floor(numCars * 0.25), 0.20);

                Cars.clear();
                for (int i = 0; i < newdriversA.size(); i++) {
                    Cars.push_back(Car(newdriversA[i]));
                }
                for (int i = 0; i < newdriversB.size(); i++) {
                    Cars.push_back(Car(newdriversB[i]));
                }
                std::cout << "mutating, generation#: " << gen << " scores: " << parentA.getDetails().score << ", " << parentB.getDetails().score << " OP: " << parentA.computeOutput() << std::endl;
                prevScore = parentA.getDetails().score;
                prevParent = parentA;   
            }

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