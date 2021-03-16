#include <exception>
#include <string>

class NNException : public std::exception {
    std::string file;
    int line;
    std::string reason;

public:
    NNException(std::string file, int line, std::string reason) : file(file), line(line), reason(reason) {}
    NNException(char *file, int line, std::string reason) : file(file), line(line), reason(reason) {}
    NNException(char *file, int line, char *reason) : file(file), line(line), reason(reason) {}
    std::string what() {
        return reason;
    }
    std::string where() {
        return "\nFile: " + file + "\nLine: " + std::to_string(line);
    }    
    std::string describe() {
        return "\nReason: " + what() + "\n" + where(); 
    }
};