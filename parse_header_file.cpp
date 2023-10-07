#include "interpreter.h"

//parsing header file
bool Interpreter::parse_header_file(std::vector<std::string>& tokens, int& eip, bool& header_file_exists) {
    // checking existence of <iostream> header file
    if (eip == 0 && tokens[0] == "#include" && tokens[1] == "<iostream>") { 
        ++eip;
        header_file_exists = true;
    } else if (eip == 0 && tokens[0] != "#include" && tokens[1] != "<iostream>") {
        throw std::runtime_error("There is no header file included. Please input the header file");
    } 
    return header_file_exists;
}