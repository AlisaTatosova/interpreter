#include "interpreter.h"

void Interpreter::cin(std::vector<std::string>& tokens) {
    if (is_number(tokens[2])) { 
        throw std::invalid_argument("Error: invalid operands to binary expression ('std::istream' (aka 'basic_istream<char>') and 'int')");
    } else if (is_declared_variable(tokens[2])) { // if must be inputed variable
        if (type_of_var(tokens[2]) == "char") {
            char var;
            std::cin >> var;
            char_vars[tokens[2]].second = var;
        } else if (type_of_var(tokens[2]) == "int") {
            int var;
            std::cin >> var;
            integer_vars[tokens[2]].second = var;
        } else if (type_of_var(tokens[2]) == "double") {
            double var;
            std::cin >> var;
            double_vars[tokens[2]].second = var;
        } else if (type_of_var(tokens[2]) == "float") {
            float var;
            std::cin >> var;
            float_vars[tokens[2]].second = var;
        } else if (type_of_var(tokens[2]) == "bool") {
            bool var;
            std::cin >> var;
            bool_vars[tokens[2]].second = var;
        } else if (type_of_var(tokens[2]) == "string") {
            std::string var;
            std::cin >> var;
            string_vars[tokens[2]].second = var;
        }
    } else if (is_declared_array(separate_name_and_size_in_array_declaration(tokens[2]).first)) { // for inputing array
        std::pair<std::string, std::string> name_size =  separate_name_and_size_in_array_declaration(tokens[2]);
        if (type_of_var(separate_name_and_size_in_array_declaration(tokens[2]).first) == "char") {
            char var;
            std::string name = name_size.first;
            int size = convert_to_type<int>(name_size.second); // the elem to cin
            std::cin >> var;
            char_arr[name].second[size] = var;
        } else if (type_of_var(separate_name_and_size_in_array_declaration(tokens[2]).first) == "int") {
            int var;
            std::string name = name_size.first;
            int size = convert_to_type<int>(name_size.second); // the elem to cin
            std::cin >> var;
            int_arr[name].second[size] = var;
        } else if (type_of_var(separate_name_and_size_in_array_declaration(tokens[2]).first) == "double") {
            double var;
            std::string name = name_size.first;
            int size = convert_to_type<int>(name_size.second); // the elem to cin
            std::cin >> var;
            double_arr[name].second[size] = var;
        } else if (type_of_var(separate_name_and_size_in_array_declaration(tokens[2]).first) == "float") {
            float var;
            std::string name = name_size.first;
            int size = convert_to_type<int>(name_size.second); // the elem to cin
            std::cin >> var;
            float_arr[name].second[size] = var;
        } else if (type_of_var(separate_name_and_size_in_array_declaration(tokens[2]).first) == "bool") {
            bool var;
            std::string name = name_size.first;
            int size = convert_to_type<int>(name_size.second); // the elem to cin
            std::cin >> var;
            bool_arr[name].second[size] = var;
        } 
    } else {
        throw std::runtime_error("Error: use of undeclared identifier " + tokens[2]);
    }
}

void Interpreter::cout(std::vector<std::string>& tokens) {
    if (is_number(tokens[2])) {
        std::cout << tokens[2];
    } else if (is_declared_variable(tokens[2])) { // for ptinting variable value
        if (type_of_var(tokens[2]) == "char") {
            std::cout << char_vars[tokens[2]].second;
        } else if (type_of_var(tokens[2]) == "int") {
            std::cout << integer_vars[tokens[2]].second;
        } else if (type_of_var(tokens[2]) == "double") {
            std::cout << double_vars[tokens[2]].second;
        } else if (type_of_var(tokens[2]) == "float") {
            std::cout << float_vars[tokens[2]].second;
        } else if (type_of_var(tokens[2]) == "bool") {
            std::cout << bool_vars[tokens[2]].second;
        } else if (type_of_var(tokens[2]) == "string") {
            std::cout << string_vars[tokens[2]].second;
        }
    } else if (has_first_and_last_double_quotes(tokens[2])) { // for printing string
        remove_double_quotes(tokens[2]);
        std::cout << tokens[2];
    } else if (has_first_and_last_single_quotes(tokens[2])) { // for printing char
        remove_single_quotes(tokens[2]);
        std::cout << tokens[2];
    } else if (is_declared_variable(separate_name_and_size_in_array_declaration(tokens[2]).first) && type_of_var(separate_name_and_size_in_array_declaration(tokens[2]).first) == "string") { // cout << str[i]
        std::pair<std::string, std::string> name_size =  separate_name_and_size_in_array_declaration(tokens[2]);
        std::string name = name_size.first;
        int size = convert_to_type<int>(name_size.second);
        std::cout << string_vars[name].second[size];
    } else if (tokens[2].find('[') && tokens[2].find(']')) { // for printing array element
        std::pair<std::string, std::string> name_size = separate_name_and_size_in_array_declaration(tokens[2]);
        if (type_of_var(name_size.first) == "char") {
            if (convert_to_type<int>(name_size.second) < (char_arr[name_size.first].second).size()) {
                std::cout << (char_arr[name_size.first].second)[convert_to_type<int>(name_size.second)];   
            } else {
                throw std::runtime_error("Warning: out of range print in array!");
            }
        } else if (type_of_var(name_size.first) == "int") {
            if (convert_to_type<int>(name_size.second) < (int_arr[name_size.first].second).size()) {
                std::cout << (int_arr[name_size.first].second)[convert_to_type<int>(name_size.second)];
            } else {
                throw std::runtime_error("Warning: out of range print in array!");
            }
        } else if (type_of_var(name_size.first) == "float") {
            if (convert_to_type<int>(name_size.second) < (float_arr[name_size.first].second).size()) {
                std::cout << (float_arr[name_size.first].second)[convert_to_type<int>(name_size.second)];
            } else {
                throw std::runtime_error("Warning: out of range print in array!");
            }
        } else if (type_of_var(name_size.first) == "double") {
            if (convert_to_type<int>(name_size.second) < (double_arr[name_size.first].second).size()) {
               std::cout << (double_arr[name_size.first].second)[convert_to_type<int>(name_size.second)];
            } else {
                throw std::runtime_error("Warning: out of range print in array!");
            }
        } else if (type_of_var(name_size.first) == "bool") {
            if (convert_to_type<int>(name_size.second) < (bool_arr[name_size.first].second).size()) {
               std::cout << (bool_arr[name_size.first].second)[convert_to_type<int>(name_size.second)];
            } else {
                throw std::runtime_error("Warning: out of range print in array!");
            }
        } 
    } else if (tokens.size() == 3 && tokens[2] == "std::endl") { // for printing new line
        std::cout << std::endl;
    } else {
        throw std::runtime_error("Error: use of undeclared identifier " + tokens[2]);
    }

    if (tokens.size() == 5 && tokens[3] == "<<" && tokens[4] == "std::endl") {
        std::cout << std::endl;
    }
}