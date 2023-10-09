#include "interpreter.h"

void Interpreter::parse_value_in_array_to_var(const std::string& tokens0, const std::string& tokens1, const std::string& tokens2) {
    std::pair<std::string, std::string> name_size = separate_name_and_size_in_array_declaration(tokens2);
    std::string tmp = name_size.second;
    if (is_declared_variable(tmp)) { 
        get_var_value_inside(tmp);
    }

    int size = convert_to_type<int>(tmp);
    bool out_of_range = true;
    if (type_of_var(name_size.first) == "char") { // array type is char
        if (size < (char_arr[name_size.first].second).size()) {
            fill_var_with_given_value(tokens0, tokens1, std::to_string(char_arr[name_size.first].second[size]));
            out_of_range = false;
        } 
    } else if (type_of_var(name_size.first) == "int") { // array type is int
        if (size < (int_arr[name_size.first].second).size()) {
            fill_var_with_given_value(tokens0, tokens1, std::to_string(int_arr[name_size.first].second[size]));
            out_of_range = false;
        } 
    } else if (type_of_var(name_size.first) == "float") {
        if (size < (float_arr[name_size.first].second).size()) {
            fill_var_with_given_value(tokens0, tokens1, std::to_string(float_arr[name_size.first].second[size]));
            out_of_range = false;
        } 
    } else if (type_of_var(name_size.first) == "double") {
        if (size < (double_arr[name_size.first].second).size()) {
            fill_var_with_given_value(tokens0, tokens1, std::to_string(double_arr[name_size.first].second[size]));
            out_of_range = false;
        }
    } else if (type_of_var(name_size.first) == "bool") {
        if (size < (bool_arr[name_size.first].second).size()) {
            fill_var_with_given_value(tokens0, tokens1, std::to_string(bool_arr[name_size.first].second[size]));
            out_of_range = false;
        }    
    } 

    if (out_of_range) {
        throw std::runtime_error("Error: out of range assingnment in array!");
    }
}