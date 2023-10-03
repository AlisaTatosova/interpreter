#include "interpreter.h"

bool Interpreter::parse_if_statement(std::vector<std::string>& tokens) {
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

    bool if_enter_flag = true;

    // extracting ( ) s from if statement now we have for example if x != y {
    extract_paren(tokens[1], '('); 
    extract_paren(tokens[3], ')');

    std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3]);

    //checking condition
    if (tokens[2] == "==") {
        
        if (pair.first != pair.second) { // do not enter if
            if_enter_flag = false;
        } 

    } else if (tokens[2] == "!=") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first == second) { // do not enter if
                if_enter_flag = false; 
            }     
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first == second) { // do not enter if
                if_enter_flag = false; 
            }  
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first == second) { // do not enter if
                if_enter_flag = false; 
            }  
        }  
    } else if (tokens[2] == ">") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first <= second) { // do not enter if
                if_enter_flag = false;
            }
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first <= second) { // do not enter if
                if_enter_flag = false;
            }
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first <= second) { // do not enter if
                if_enter_flag = false;
            } 
        }  
    } else if (tokens[2] == ">=") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first < second) { // do not enter if
                if_enter_flag = false;
            }
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first < second) { // do not enter if
                if_enter_flag = false;
            }
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first < second) { // do not enter if
                if_enter_flag = false;
            }
        } 
    } else if (tokens[2] == "<") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first >= second) { // do not enter if
                if_enter_flag = false;
            }
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first >= second) { // do not enter if
                if_enter_flag = false;
            }
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first >= second) { // do not enter if
                if_enter_flag = false;
            }
        }  
    } else if (tokens[2] == "<=") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first >= second) { // do not enter if
                if_enter_flag = false;
            }
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first >= second) { // do not enter if
                if_enter_flag = false;
            }
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first >= second) { // do not enter if
                if_enter_flag = false;
            }
        }  
    } else if (tokens[2] == "<=") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first > second) { // do not enter if
                if_enter_flag = false;
            }
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first > second) { // do not enter if
                if_enter_flag = false;
            }
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first > second) { // do not enter if
                if_enter_flag = false;
            }
        }  
    } else {
        throw std::runtime_error("Error: " + tokens[2]  + " in if statment is not appropriate for comparison");
    }

    return if_enter_flag;
}