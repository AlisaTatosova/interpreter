#include "interpreter.h"

char Interpreter::get_value_of_char(const std::string& token) {
    for (auto it = char_vars.begin(); it != char_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
}

int Interpreter::get_value_of_int(const std::string& token) {
    for (auto it = integer_vars.begin(); it != integer_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
}

double Interpreter::get_value_of_double(const std::string& token) {
    for (auto it = double_vars.begin(); it != double_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
}

float Interpreter::get_value_of_float(const std::string& token) {
    for (auto it = float_vars.begin(); it != float_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
}

bool Interpreter::get_value_of_bool(const std::string& token) {
    for (auto it = bool_vars.begin(); it != bool_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
}

std::string Interpreter::get_value_of_string(const std::string& token) {
    for (auto it = string_vars.begin(); it != string_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
}