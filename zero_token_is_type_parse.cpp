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
    } else if (tokens.size() == 2 && tokens[1].find('[') && tokens[1].find(']') ) {
        std::pair<std::string, std::string> array_name_size = separate_name_and_size_in_array_declaration(tokens[1]);
        if (tokens[0] == "char") {
            std::vector<char> chars(convert_to_type<int>(array_name_size.second));
            char_arr[array_name_size.first] = {tokens[0], chars};
        } else if (tokens[0] == "int") {
            std::vector<int> ints(convert_to_type<int>(array_name_size.second));
            int_arr[array_name_size.first] = {tokens[0], ints};
        } else if (tokens[0] == "double") {
            std::vector<double> doubles(convert_to_type<int>(array_name_size.second));
            double_arr[array_name_size.first] = {tokens[0], doubles};
        } else if (tokens[0] == "float") {
            std::vector<float> floats(convert_to_type<int>(array_name_size.second));
            float_arr[array_name_size.first] = {tokens[0], floats};
        } else if (tokens[0] == "bool") {
            std::vector<bool> bools(convert_to_type<int>(array_name_size.second));
            bool_arr[array_name_size.first] = {tokens[0], bools};
        } else if (tokens[0] == "string") {
            std::vector<std::string> strings(convert_to_type<int>(array_name_size.second));
            string_arr[array_name_size.first] = {tokens[0], strings};
        }

    } else if (tokens.size() > 2 && tokens[2] == "=") {
        if (!check_open_figure_parent(tokens[3])) {
            throw std::runtime_error("Error: there is no opening scope in array definition");
        }

        if (!check_close_figure_parent(tokens[tokens.size() - 1])) {
            throw std::runtime_error("Error: there is no closing scope in array definition");
        }

        std::pair<std::string, std::string> array_name_size = separate_name_and_size_in_array_declaration(tokens[1]); // this is for checking existence of []
        std::string extract1 = remove_comma_at_the_end(extract_figure_paren(tokens[3], '{')); // removing { at front and also removing , at end
        std::string extract2 = extract_figure_paren(tokens[tokens.size() - 1], '}');

        if (tokens.size() - 3 > convert_to_type<int>(array_name_size.second)){
            throw std::out_of_range("Error: out of range in array definition");
        }

        if (tokens[0] == "char") {
            std::vector<char> chars(convert_to_type<int>(array_name_size.second));
            if (has_first_and_last_single_quotes(extract1)) {
                remove_single_quotes(extract1);
            }
            chars[0] = convert_to_type<char>(extract1);
            int j = 4;
            for (int i = 1; i < chars.size() - 1; ++i) {
                std::string comma_removed = remove_comma_at_the_end(tokens[j]);
                if (has_first_and_last_single_quotes(comma_removed)) {
                    remove_single_quotes(comma_removed);
                }
                chars[i] = convert_to_type<char>(comma_removed);
                ++j;
            }
            
            if (has_first_and_last_single_quotes(extract2)) {
                remove_single_quotes(extract2);
            }
            chars[chars.size() - 1] = convert_to_type<char>(extract2);
            char_arr[array_name_size.first] = {tokens[0], chars};
        } else if (tokens[0] == "int") {
            std::vector<int> ints(convert_to_type<int>(array_name_size.second));
            ints[0] = convert_to_type<int>(extract1);
            int j = 4;
            for (int i = 1; i < ints.size() - 1; ++i) {
                std::string comma_removed = remove_comma_at_the_end(tokens[j]);
                ints[i] = convert_to_type<int>(comma_removed);
                ++j;
            }

            ints[ints.size() - 1] = convert_to_type<int>(extract2);
            int_arr[array_name_size.first] = {tokens[0], ints};
        } else if (tokens[0] == "double") {
            std::vector<double> doubles(convert_to_type<int>(array_name_size.second));
            doubles[0] = convert_to_type<double>(extract1);
            int j = 4;
            for (int i = 1; i < doubles.size() - 1; ++i) {
                std::string comma_removed = remove_comma_at_the_end(tokens[j]);
                doubles[i] = convert_to_type<double>(comma_removed);
                ++j;
            }

            doubles[doubles.size() - 1] = convert_to_type<double>(extract2);
            double_arr[array_name_size.first] = {tokens[0], doubles};
        } else if (tokens[0] == "float") {
            std::vector<float> floats(convert_to_type<int>(array_name_size.second));
            floats[0] = convert_to_type<float>(extract1);
            int j = 4;
            for (int i = 1; i < floats.size() - 1; ++i) {
                std::string comma_removed = remove_comma_at_the_end(tokens[j]);
                floats[i] = convert_to_type<float>(comma_removed);
                ++j;
            }

            floats[floats.size() - 1] = convert_to_type<float>(extract2);
            float_arr[array_name_size.first] = {tokens[0], floats};
        } else if (tokens[0] == "bool") {
            std::vector<bool> bools(convert_to_type<int>(array_name_size.second));
            bools[0] = convert_to_type<bool>(extract1);
            int j = 4;
            for (int i = 1; i < bools.size() - 1; ++i) {
                std::string comma_removed = remove_comma_at_the_end(tokens[j]);
                bools[i] = convert_to_type<bool>(comma_removed);
                ++j;
            }

            bools[bools.size() - 1] = convert_to_type<bool>(extract2);
            bool_arr[array_name_size.first] = {tokens[0], bools};
        }
    }

    if (tokens.size() == 6 && tokens[2] == "=") { 
        std::string tmp = tokens[1];
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