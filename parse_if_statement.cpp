#include "interpreter.h"

void Interpreter::parse_if_statement(std::vector<std::string>& tokens, bool& if_enter_flag) {
    if (find_brace(tokens).size() > 3) {
        throw std::runtime_error("Error: extraneous brace in if statment");
    } else if (find_brace(tokens).size() < 3) {
        throw std::runtime_error("Error: missing brace in if statment");
    }

    if (!check_open_parent(tokens[1])) {
        size_t pos = tokens[1].find('(');
        throw std::runtime_error("Error: use of undeclared identifier " + tokens[1].substr(0, pos) + " in if statment");
    }

    if (!check_close_parent(tokens[3])) {
        size_t pos = tokens[3].find(')');
        throw std::runtime_error("Error: use of undeclared identifier " + tokens[3].substr(pos + 1) + " in if statment");
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
    } else {
        throw std::runtime_error("Error: " + tokens[2]  + " in if statment is not appropriate for comparison");
    }
}