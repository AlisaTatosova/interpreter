#include "interpreter.h"

// getting value inside array index
std::string Interpreter::get_value_inside_array_index(const std::string& str) {
    std::pair<std::string, std::string> name_size = separate_name_and_size_in_array_declaration(str);
    std::string tmp = name_size.second;
    if (is_declared_variable(name_size.second)) {
        get_var_value_inside(tmp);
    }

    int size = convert_to_type<int>(tmp);
    if (type_of_var(name_size.first) == "char") {
        if (size < (char_arr[name_size.first].second).size()) {
            return std::to_string(char_arr[name_size.first].second[size]);   
        } else {
            throw std::runtime_error("Warning: out of range access in array!");
        }
    } else if (type_of_var(name_size.first) == "int") {
        if (size < (int_arr[name_size.first].second).size()) {
            return std::to_string(int_arr[name_size.first].second[size]);
        } else {
            throw std::runtime_error("Warning: out of range access in array!");
        }
    } else if (type_of_var(name_size.first) == "float") {
        if (size < (float_arr[name_size.first].second).size()) {
            return std::to_string(float_arr[name_size.first].second[size]);
        } else {
            throw std::runtime_error("Warning: out of range access in array!");
        }
    } else if (type_of_var(name_size.first) == "double") {
        if (size < (double_arr[name_size.first].second).size()) {
            return std::to_string(double_arr[name_size.first].second[size]);
        } else {
            throw std::runtime_error("Warning: out of range access in array!");
        }
    } else if (type_of_var(name_size.first) == "bool") {
        if (size < (bool_arr[name_size.first].second).size()) {
            return std::to_string(bool_arr[name_size.first].second[size]);
        } else {
            throw std::runtime_error("Warning: out of range access in array!");
        }
    } 
} 