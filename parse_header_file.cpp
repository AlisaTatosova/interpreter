#include "interpreter.h"

bool Interpreter::parse_header_file(std::vector<std::string>& tokens, int& eip, bool& header_file_exists) {
    // checking existence of <iostream> header file
    if (eip == 0 && tokens[0] == "#include" && tokens[1] == "<iostream>") { 
        ++eip;
        header_file_exists = true;
    } else if (eip == 0 && tokens[0] != "#include" && tokens[1] != "<iostream>") {
        std::cout <<  "There is no header file included. Please input the header file" << std::endl;
    } 
    return header_file_exists;
}