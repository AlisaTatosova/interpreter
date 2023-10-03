#include "interpreter.h"

bool Interpreter::parse_main(std::vector<std::string>& tokens, int& eip, bool& main_exists) {
    // checking existence of int main
    if (eip == 1 && tokens[0] == "int") {
        std::string concat = concatenate_and_remove_spaces(tokens);
        if (concat == "intmain(){") {
            main_exists = true;
            rows[eip] = tokens;
            ++eip;
        } else if (concat == "intmain()") {
            throw std::runtime_error("There is no opening scope '{' for main function");
        } else if (concat != "intmain()" && tokens[1] != "intmain") {
            throw std::runtime_error("There is no main function to start program");
        }
    } 
    return main_exists;
}