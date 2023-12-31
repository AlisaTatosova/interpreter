#include "interpreter.h"

//function for parsing the line where first token is while loop
bool Interpreter::parse_while(std::vector<std::string>& tokens) {
    if (find_brace(tokens).size() > 5) {
        throw std::runtime_error("Error: extraneous brace in while statment");
    } else if (find_brace(tokens).size() < 3) {
        throw std::runtime_error("Error: missing brace in while statment");
    }

    if (!check_open_parent(tokens[1])) {
        size_t pos = tokens[1].find('(');
        throw std::runtime_error("Error: use of undeclared identifier " + tokens[1].substr(0, pos) + " in while statment");
    }

    if (!check_close_parent(tokens[3])) {
        size_t pos = tokens[3].find(')');
        throw std::runtime_error("Error: use of undeclared identifier " + tokens[3].substr(pos + 1) + " in while statment");
    }

    bool while_enter_flag = true;

    // extracting ( ) s from if statement now we have for example if x != y {
    std::string op1 = extract_paren(tokens[1], '('); 
    std::string op2 = extract_paren(tokens[3], ')');
  
    std::pair<std::string, std::string> pair = check_variables_inside(op1, op2);

    //checking condition
    if (tokens[2] == "==") {
        if (pair.first != pair.second) { // do not enter while
            while_enter_flag = false;
        }
    } else if (tokens[2] == "!=") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first == second) { // do not enter while
                while_enter_flag = false; 
            }     
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first == second) { // do not enter while
                while_enter_flag = false; 
            }  
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first == second) { // do not enter while
                while_enter_flag = false; 
            }  
        }  
    } else if (tokens[2] == ">") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first <= second) { // do not enter while
                while_enter_flag = false;
            }
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first <= second) { // do not enter while
                while_enter_flag = false;
            }
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first <= second) { // do not enter while
                while_enter_flag = false;
            } 
        }  
    } else if (tokens[2] == ">=") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first < second) { // do not enter while
                while_enter_flag = false;
            }
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first < second) { // do not enter while
                while_enter_flag = false;
            }
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first < second) { // do not enter while
                while_enter_flag = false;
            }
        }  
    } else if (tokens[2] == "<") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first >= second) { // do not enter while
                while_enter_flag = false;
            }
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first >= second) { // do not enter while
                while_enter_flag = false;
            }
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first >= second) { // do not enter while
                while_enter_flag = false;
            }
        }  
    } else if (tokens[2] == "<=") {
        if (is_number(pair.first) || is_number(pair.second)) {
            double first = convert_to_type<double>(pair.first);
            double second = convert_to_type<double>(pair.second);
            if (first > second) { // do not enter while
                while_enter_flag = false;
            }
        } else if (has_first_and_last_double_quotes(pair.first) && has_first_and_last_double_quotes(pair.second)) {
            std::string first = convert_to_type<std::string>(pair.first);
            std::string second = convert_to_type<std::string>(pair.second);
            if (first > second) { // do not enter while
                while_enter_flag = false;
            }
        } else if (has_first_and_last_single_quotes(pair.first) && has_first_and_last_single_quotes(pair.second)) {
            char first = convert_to_type<char>(pair.first);
            char second = convert_to_type<char>(pair.second);
            if (first > second) { // do not enter while
                while_enter_flag = false;
            }
        }  
    } else {
        throw std::runtime_error("Error: " + tokens[2]  + " in while is not appropriate for comparison");
    }
     
    return while_enter_flag;
}