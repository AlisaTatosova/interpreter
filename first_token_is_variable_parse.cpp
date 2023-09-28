#include "interpreter.h"

void Interpreter::first_token_is_variable_parse(std::vector<std::string>& tokens) {
    if (tokens.size() == 3 && is_declared_variable(tokens[0]) && tokens[1] == "=") {
        zero_token_is_var_parse(tokens); // // token[0] is variable
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
    } 
}
