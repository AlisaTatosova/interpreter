#include "interpreter.h"

void Interpreter::first_token_is_variable_parse(std::vector<std::string>& tokens) {
    if (tokens.size() == 3 && is_declared_variable(tokens[0]) && tokens[1] == "=") {
        if (!is_declared_variable(tokens[2])) { // case x = 5; // x is declared
            if (!is_number(tokens[2]) && !has_first_and_last_double_quotes(tokens[2]) && !has_first_and_last_single_quotes(tokens[2]) && tokens[2] != "false" && tokens[2] != "true") {
                throw std::runtime_error("Error: Variable " + tokens[2] + " is not declared");
            }
        
            fill_var_with_given_value(type_of_var(tokens[0]), tokens[0], tokens[2]);
        
        } else { // case x = y;
            fill_var_with_given_variable_value(type_of_var(tokens[0]), tokens[0], tokens[2]);
        }
    } else if (tokens.size() == 5 && is_declared_variable(tokens[0]) && tokens[1] == "=") { 
        std::string tmp = tokens[0];

        std::pair<std::string, std::string> pair = check_variables_inside(tokens[2], tokens[4]); // checking types of op1 and op2 before some operation and make op1 and op2 as values inside them like op1 = x, op2 = y, after this function op1 = (value inside x), op2 = (value inside y)
        if (tokens[3] == "+") {
            adding_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[3] == "-") {
            sub_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[3] == "*") {
            mul_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[3] == "/") {
            div_cases(tokens, tmp, pair.first, pair.second);
        }
    } else if (tokens.size() == 3 && is_declared_variable(tokens[0])) {
        std::string tmp = tokens[0];
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[0], tokens[2]); // checking types of op1 and op2 before some operation to make precise castings
        if (tokens[1] == "+=") {
            adding_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[1] == "-=") {
            sub_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[1] == "*=") {
            mul_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[1] == "/=") {
            div_cases(tokens, tmp, pair.first, pair.second);
        }  
    } else if (tokens.size() == 3 && is_declared_variable(tokens[0]) && tokens[1] != "=") {
        throw std::runtime_error("Error: " + tokens[1] + " is undefined");
    } else if (tokens.size() == 5 && is_declared_variable(tokens[0]) && tokens[1] != "=") { 
        throw std::runtime_error("Error: " + tokens[1] + " is undefined");
    } else if (tokens.size() == 3 && tokens[0].find('[') && tokens[0].find(']')) { // to handle arr[3] = 5
        parse_array_assingment(tokens);   
    }
}
