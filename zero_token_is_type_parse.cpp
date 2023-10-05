#include "interpreter.h"

void Interpreter::zero_token_is_type_parse(std::vector<std::string>& tokens) {
    if (is_number(tokens[1])) {
        throw std::runtime_error("Error: Variable name can not be a number: " + tokens[1]);
    }

    if (tokens.size() == 2 && !find_open_close_brace(tokens[1], '[') && !find_open_close_brace(tokens[1], ']')) { // case that int x; simple declaration
        check_redefinition(tokens[1]);
        fill_with_garbage(tokens[0], tokens[1]);
    } else if (tokens.size() == 4 && tokens[2] == "=" && !is_declared_variable(tokens[3])) { // case if int x = 5; like declaration
        if (!is_number(tokens[3]) && !has_first_and_last_double_quotes(tokens[3]) && !has_first_and_last_single_quotes(tokens[3]) && tokens[3] != "false" && tokens[3] != "true") {
            throw std::runtime_error("Error: Variable " + tokens[3] + " is not declared");
        }

        check_redefinition(tokens[1]);
        fill_var_with_given_value(tokens[0], tokens[1], tokens[3]);
    } else if (tokens.size() == 4 && tokens[2] == "=" && is_declared_variable(tokens[3])) { // int x = y, y is declared
        check_redefinition(tokens[1]);
        fill_var_with_given_variable_value(tokens[0], tokens[1], tokens[3]);
    } else if (tokens.size() == 4 && tokens[2] != "=" && is_declared_variable(tokens[3])) { // cases that = is error
        throw std::runtime_error("Error: " + tokens[2] + " is undefined");
    } else if (tokens.size() == 4 && tokens[2] != "=" && !is_declared_variable(tokens[3])) { // cases that = is error
        throw std::runtime_error("Error: " + tokens[2] + " is undefined");
    } else if (tokens.size() == 2 && tokens[1].find('[') && tokens[1].find(']') ) {  //array declaration  int arr[3];
        array_declaration(tokens);
    } else if (tokens.size() > 2 && tokens[2] == "=" && find_open_close_brace(tokens[1], '[') && find_open_close_brace(tokens[1], ']')) { // to handle array init list 
        arr_init_list_parse(tokens);
    }

    if (tokens.size() == 6 && tokens[2] == "=") { 
        std::string tmp = tokens[1];
        check_redefinition(tokens[1]);
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[3], tokens[5]); // checking types of op1 and op2 before some operation and make op1 and op2 as values inside them like op1 = x, op2 = y, after this function op1 = (value inside x), op2 = (value inside y)

        if (tokens[4] == "+") {
            adding_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[4] == "-") {
            sub_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[4] == "*") {
            mul_cases(tokens, tmp, pair.first, pair.second);
        } else if (tokens[4] == "/") {
            div_cases(tokens, tmp, pair.first, pair.second);
        }
    } else if (tokens.size() == 6 && tokens[2] != "=") { 
        throw std::runtime_error("Error: " + tokens[2] + " is undefined");
    }
}