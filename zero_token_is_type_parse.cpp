#include "interpreter.h"

void Interpreter::zero_token_is_type_parse(std::vector<std::string>& tokens, bool flag) {
    const std::string str = (flag == true) ? "inside_if" : "";

    if (tokens.size() == 2) { // case that int x; simple declaration
        if (!flag) {
            check_redefinition(tokens[1]);
        }
        fill_with_garbage(tokens[0], tokens[1]);
    } else if (tokens.size() == 4 && tokens[2] == "=" && !is_declared_variable(tokens[3])) { // case if int x = 5; like declaration
        if (!flag) {
            check_redefinition(tokens[1]);
        }

        fill_var_with_given_value(tokens[0], tokens[1], tokens[3], str);
        
    } else if (tokens.size() == 4 && tokens[2] == "=" && is_declared_variable(tokens[3])) { // int x = y, y is declared
       
        if (!flag) {
            check_redefinition(tokens[1]);
        }
        fill_var_with_given_variable_value(tokens[0], tokens[1], tokens[3], str);
    } 
    // else if (!is_number(tokens[3]) && !has_first_and_last_double_quotes(tokens[3]) && !has_first_and_last_single_quotes(tokens[3]) && tokens[3] != "false" && tokens[3] != "true") {
    //     std::cout << "Error: "  << tokens[3] <<  " unknown assignment " << std::endl; // int x = y, y is not declared
    //     return;
    // }

    if (tokens.size() == 6 && tokens[2] == "=") { 
        if (str != "") {
            if_inside.push_back(tokens[1] + str);
        }

        std::string tmp = tokens[1] + str;
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[3], tokens[5], flag); // checking types of op1 and op2 before some operation and make op1 and op2 as values inside them like op1 = x, op2 = y, after this function op1 = (value inside x), op2 = (value inside y)
        if (tokens[4] == "+") {
            adding_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[4] == "-") {
            sub_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[4] == "*") {
            mul_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[4] == "/") {
            div_cases(tokens, tmp, pair.first, pair.second);
        }
    }
}