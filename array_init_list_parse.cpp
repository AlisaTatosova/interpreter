#include "interpreter.h"

void Interpreter::arr_init_list_parse(std::vector<std::string>& tokens) {
    if (!check_open_figure_parent(tokens[3])) {
        throw std::runtime_error("Error: there is no opening scope in array definition");
    }

    if (!check_close_figure_parent(tokens[tokens.size() - 1])) {
        throw std::runtime_error("Error: there is no closing scope in array definition");
    }

    std::pair<std::string, std::string> array_name_size = separate_name_and_size_in_array_declaration(tokens[1]); // this is for checking existence of []
    std::string extract1 = remove_comma_at_the_end(extract_figure_paren(tokens[3], '{')); // removing { at front and also removing , at end
    std::string extract2 = extract_figure_paren(tokens[tokens.size() - 1], '}');

    int size = convert_to_type<int>(array_name_size.second);
    if (tokens.size() - 3 > size){
        throw std::out_of_range("Error: out of range in array definition");
    }

    if (tokens[0] == "char") {
        std::vector<char> chars(size);
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
        std::vector<int> ints(size);
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
        std::vector<double> doubles(size);
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
        std::vector<float> floats(size);
        floats[0] = convert_to_type<float>(extract1);
        int j = 4;
        for (int i = 1; i < floats.size() - 1; ++i) {
            std::string comma_removed = remove_comma_at_the_end(tokens[j]);                floats[i] = convert_to_type<float>(comma_removed);
            ++j;
        }

        floats[floats.size() - 1] = convert_to_type<float>(extract2);
        float_arr[array_name_size.first] = {tokens[0], floats};
    } else if (tokens[0] == "bool") {
        std::vector<bool> bools(size);
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
