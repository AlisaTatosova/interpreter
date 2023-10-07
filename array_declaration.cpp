#include "interpreter.h"

// parsing array declaration
void Interpreter::array_declaration(std::vector<std::string>& tokens) {
    std::pair<std::string, std::string> array_name_size = separate_name_and_size_in_array_declaration(tokens[1]);
    int size = convert_to_type<int>(array_name_size.second);
    if (tokens[0] == "char") {
        std::vector<char> chars(size);
        char_arr[array_name_size.first] = {tokens[0], chars};
    } else if (tokens[0] == "int") {
        std::vector<int> ints(size);
        int_arr[array_name_size.first] = {tokens[0], ints};
    } else if (tokens[0] == "double") {
        std::vector<double> doubles(size);
        double_arr[array_name_size.first] = {tokens[0], doubles};
    } else if (tokens[0] == "float") {
        std::vector<float> floats(size);
        float_arr[array_name_size.first] = {tokens[0], floats};
    } else if (tokens[0] == "bool") {
        std::vector<bool> bools(size);
        bool_arr[array_name_size.first] = {tokens[0], bools};
    } else if (tokens[0] == "string") {
        std::vector<std::string> strings(size);
        string_arr[array_name_size.first] = {tokens[0], strings};
    }
}