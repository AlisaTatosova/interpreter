#include "interpreter.h"

void Interpreter::parse_if_statement(std::vector<std::string>& tokens, bool& if_enter_flag) {
    if (find_brace(tokens).size() > 3) {
        std::cout << "extraneous brace" << std::endl;
        return;
    } if (find_brace(tokens).size() < 3) {
        std::cout << "missing brace" << std::endl;
        return;
    }

    // extracting ( ) s from if statement now we have for example if x != y {
    extract_paren(tokens[1], '('); 
    extract_paren(tokens[3], ')');

    //checking condition
    if (tokens[2] == "==") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3]);
        if (pair.first != pair.second) { // do not enter if
            if_enter_flag = true;
        } 
    } else if (tokens[2] == "!=") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3]);
        if (pair.first == pair.second) { // do not enter if
            if_enter_flag = true;
        }  
    } else if (tokens[2] == ">") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3]);
        if (pair.first <= pair.second) { // do not enter if
            if_enter_flag = true;
        }
    } else if (tokens[2] == ">=") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3]);
        if (pair.first < pair.second) { // do not enter if
            if_enter_flag = true;
        }
      } else if (tokens[2] == "<") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3]);
        if (pair.first >= pair.second) { // do not enter if
            if_enter_flag = true;
        }
    } else if (tokens[2] == "<=") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3]);
        if (pair.first > pair.second) { // do not enter if
            if_enter_flag = true;
        }
    }
}