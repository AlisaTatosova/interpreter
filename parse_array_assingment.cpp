#include "interpreter.h"

// parsing array assignment
void Interpreter::parse_array_assingment(std::vector<std::string>& tokens) { // case arr[i] = j;
    std::pair<std::string, std::string> name_size =  separate_name_and_size_in_array_declaration(tokens[0]);
    if (is_declared_array(name_size.first)) {
        std::string tmp = tokens[2];
        if (is_declared_variable(tokens[2])) { // if the var to be assigned is variable
            get_var_value_inside(tmp);
        }

        int size = convert_to_type<int>(name_size.second);
        if (type_of_var(name_size.first) == "char") { // array type is char
            if (size < (char_arr[name_size.first].second).size()) {
                if (has_first_and_last_single_quotes(tmp)) {
                    remove_single_quotes(tmp);
                }
                char_arr[name_size.first].second[size] = convert_to_type<char>(tmp);
            } else {
                throw std::runtime_error("Error: out of range initialization in array!");
            }
        } else if (type_of_var(name_size.first) == "int") { // array type is int
            if (size < (int_arr[name_size.first].second).size()) {
                int_arr[name_size.first].second[size] = convert_to_type<int>(tmp);
            } else {
                throw std::runtime_error("Error: out of range initialization in array!");
            } 
        } else if (type_of_var(name_size.first) == "float") {
            if (size < (float_arr[name_size.first].second).size()) {
                float_arr[name_size.first].second[size] = convert_to_type<float>(tmp);
            } else {
                throw std::runtime_error("Error: out of range initialization in array!");
            } 
        } else if (type_of_var(name_size.first) == "double") {
            if (size < (double_arr[name_size.first].second).size()) {
                double_arr[name_size.first].second[size] = convert_to_type<double>(tmp);
            } else {
                throw std::runtime_error("Error: out of range initialization in array!");
            } 
        } else if (type_of_var(name_size.first) == "bool") {
            if (size < (bool_arr[name_size.first].second).size()) {
                bool_arr[name_size.first].second[size] = convert_to_type<bool>(tmp);
            } else {
                throw std::runtime_error("Error: out of range initialization in array!");
            }    
        } 
    } else {
        throw std::runtime_error("Error: There is no such array name" + name_size.first);
    }

}