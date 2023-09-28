#include "interpreter.h"

void Interpreter::zero_token_is_var_parse(std::vector<std::string>& tokens, bool flag) {
    const std::string str = "";
    if (flag && is_declared_variable(tokens[0] + "inside_if")) {
        const std::string str = "inside_if";
    }

    if (!is_declared_variable(tokens[2] + str)) { // case x = 5; // x is declared
        if (!is_number(tokens[2]) && !has_first_and_last_double_quotes(tokens[2]) && !has_first_and_last_single_quotes(tokens[2]) && tokens[2] != "false" && tokens[2] != "true") {
            std::cout << "Error: "  << tokens[2] <<  " was not declared in this scope " << std::endl; // int x = y, y is not declared
            return;
        }
        
        fill_var_with_given_value(type_of_var(tokens[0]), tokens[0], tokens[2], str);
        
    } else { // case x = y;
        fill_var_with_given_variable_value(type_of_var(tokens[0]), tokens[0], tokens[2], str);
    }
}