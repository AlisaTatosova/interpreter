#include "interpreter.h"

// parsing increment and decrement
void Interpreter::increment_decrement_parse(const std::vector<std::string>& tokens) {
    if (tokens.size() == 1 && start_with_plus_plus(tokens[0]) || end_with_plus_plus(tokens[0])) {
        std::string without_plus_plus = extract_plus_plus(tokens[0]);
        if (is_declared_variable(without_plus_plus)) {
            std::string tmp = without_plus_plus;
            std::pair<std::string, std::string> pair = check_variables_inside(without_plus_plus, std::to_string(1)); // checking types of op1 and op2 before some operation and make op1 and op2 as values inside them like op1 = x, op2 = y, after this function op1 = (value inside x), op2 = (value inside y)
            adding_cases(tokens, tmp, pair.first, pair.second);
        } else {
            throw std::runtime_error("Error: " + without_plus_plus + " must be declared variable to increment");
        }
    } else if (start_with_minus_minus(tokens[0]) || end_with_minus_minus(tokens[0])) {
        std::string without_minus_minus = extract_minus_minus(tokens[0]);
        if (is_declared_variable(without_minus_minus)) {
            std::string tmp = without_minus_minus;
            std::pair<std::string, std::string> pair = check_variables_inside(without_minus_minus, std::to_string(1)); // checking types of op1 and op2 before some operation and make op1 and op2 as values inside them like op1 = x, op2 = y, after this function op1 = (value inside x), op2 = (value inside y)
            sub_cases(tokens, tmp, pair.first, pair.second);
        } else {
            throw std::runtime_error("Error: " + without_minus_minus + " must be declared variable to decrement");
        }
    } 
}