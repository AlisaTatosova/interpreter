#include "interpreter.h"

// getting values of different types
char Interpreter::get_value_of_char(const std::string& token) {
    for (auto it = char_vars.begin(); it != char_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    return ' ';
}

int Interpreter::get_value_of_int(const std::string& token) {
    for (auto it = integer_vars.begin(); it != integer_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    return 0;
}

double Interpreter::get_value_of_double(const std::string& token) {
    for (auto it = double_vars.begin(); it != double_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    return 0;
}

float Interpreter::get_value_of_float(const std::string& token) {
    for (auto it = float_vars.begin(); it != float_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    return 0;
}

bool Interpreter::get_value_of_bool(const std::string& token) {
    for (auto it = bool_vars.begin(); it != bool_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    return 0;
}

std::string Interpreter::get_value_of_string(const std::string& token) {
    for (auto it = string_vars.begin(); it != string_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    return "";
}