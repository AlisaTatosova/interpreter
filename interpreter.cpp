#include "interpreter.h"

Interpreter::Interpreter() {
    type_names();
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void Interpreter::parse(std::ifstream& file, int i) {
    if (!file.is_open()) {
        std::cerr << "Unable to open the file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) { // Read the file line by line
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        
        while (iss >> token) { // Split the line into words by spaces
            tokens.push_back(token);
        }

        if (tokens.empty()) { // if line is empty
            continue;
        }

        // checking existence of <iostream> header file
        if (eip == 0 && tokens[0] == "#include" && tokens[1] == "<iostream>") { 
            ++eip;
            continue;
        } else if (eip == 0 && tokens[0] != "#include" && tokens[1] != "<iostream>") {
            std::cout << "There is no header file included.Please input header file" << std::endl;
            return;
        } 


        // checking existence of int main
        if (eip == 1 && tokens[0] == "int") {
            std::string concat = concatenate_and_remove_spaces(tokens);
            if (concat == "intmain(){") {
                main_exists = true;
                rows[eip] = tokens;
                ++eip;
                continue;
            } else if (concat == "intmain()") {
                std::cout << "There is no opening scope '{' for main function" << std::endl;
                return;
            } else {
                std::cout << "There is no main function to start program" << std::endl;
                return;
            }
        }

        // checking for ; existence at the end of code 
        check_existence_of_semicolons(tokens);

        // removing ; s from code
        remove_semicolons(tokens);


        if (tokens[0] == "char" || tokens[0] == "int" || tokens[0] == "double" || tokens[0] == "float" || tokens[0] == "bool" || tokens[0] == "string") {
            zero_token_is_type(tokens); // token[0] is type
        } else if (tokens.size() == 3 && is_declared_variable(tokens[0]) && tokens[1] == "=") {
            zero_token_is_var(tokens); // // token[0] is variable
        } else if (tokens[0] == "std::cout" && tokens[1] == "<<" && tokens.size() == 3) {
            cout(tokens);
        }

        rows[eip] = tokens;
        ++eip;
    }

    --eip;
    if (rows[eip][rows[eip].size() - 1] != "}") { // checking '}' existence at the end 
        std::cout << "There is no closing scope '}' at the en of main function" << std::endl;
    }

    file.close(); // Close the file when done
}

void Interpreter::type_names() {
    types[0] = "char";
    types[1] = "int";
    types[2] = "double";
    types[3] = "float";
    types[4] = "bool";
    types[5] = "string";
}

bool Interpreter::last_char_is_semicolon(std::string& str) {
    if (str[str.size() - 1] == ';') {
        return true;
    }
    return false;
}

bool Interpreter::contains_semicolon(const std::string& str) {
    return str.find(';') != std::string::npos;
}

void Interpreter::remove_semicolon(std::string& input) {
    input = input.substr(0, input.size() - 1); // extract the substring without the semicolon
}

void Interpreter::check_existence_of_semicolons(std::vector<std::string>& tokens) {
    if (eip > 1 && tokens[tokens.size() - 1] != ";" && !last_char_is_semicolon(tokens[tokens.size() - 1])) {
        if (tokens[tokens.size() - 1] != "}") {
            std::cout << "You forgot ; at the end of line" << std::endl;
            return;
        }
    } 
}

void Interpreter::remove_semicolons(std::vector<std::string>& tokens) {
    // removing ; s from code
    for (int i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == ";") {
            tokens.pop_back();
        } else if (contains_semicolon(tokens[i])) {
            remove_semicolon(tokens[i]);
        } 
    }
}

std::string Interpreter::concatenate_and_remove_spaces(const std::vector<std::string>& vector) {
    std::string concat;

    for (const std::string& str : vector) {
        concat += str;
    }

    concat.erase(std::remove_if(concat.begin(), concat.end(), ::isspace), concat.end());

    return concat;
}

template<typename T>
bool Interpreter::is_declared_variable(const std::string& token, const std::map<std::string, T>& vars) {
    return vars.find(token) != vars.end();
}

bool Interpreter::is_declared_variable(const std::string& token) {
    return is_declared_variable(token, char_vars) ||
           is_declared_variable(token, integer_vars) ||
           is_declared_variable(token, float_vars) ||
           is_declared_variable(token, double_vars) ||
           is_declared_variable(token, bool_vars) ||
           is_declared_variable(token, string_vars);
}

bool Interpreter::is_number(const std::string& str) {
    if (str.empty()) {
        return false; // Empty string is not a number
    }

    // Check for negative sign at the beginning
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }

    bool has_dot = false; // To track if there's a decimal point

    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '.') {
            // Found a decimal point
            if (has_dot) {
                return false; // Multiple decimal points are not allowed
            }
            has_dot = true;
        } else if (!std::isdigit(str[i])) {
            return false; // Found a non-digit character
        }
    }

    try {
        // Try converting the string to a double to handle scientific notation, etc.
        std::stod(str);
        return true;
    } catch (const std::invalid_argument& e) {
        return false; // Conversion failed
    }
}

void Interpreter::remove_double_quotes(std::string& input) {
    if (input.front() == '"' && input.back() == '"') {
        input = input.substr(1, input.length() - 2);
    } else {
        throw std::invalid_argument("There is no double quotes "" ");
    }
}

void Interpreter::remove_single_quotes(std::string& input) {
    if (!input.empty() && input.front() == '\'') {
        input = input.substr(1);
    }

    if (!input.empty() && input.back() == '\'') {
        input.pop_back();
    } else {
        throw std::invalid_argument("There is no single quotes '' ");
    }
}

bool Interpreter::has_first_and_last_double_quotes(const std::string& input) {
    if (input.size() >= 2) {
        return (input.front() == '"' && input.back() == '"');
    } else {
        return false; // The string is too short to have first and last quotes
    }
}

bool Interpreter::has_first_and_last_single_quotes(const std::string& input) {
    if (input.size() >= 2) {
        return (input.front() == '\'' && input.back() == '\'');
    } else {
        return false; // The string is too short to have first and last single quotes
    }
}

void Interpreter::zero_token_is_type(std::vector<std::string>& tokens) {
    if (tokens.size() == 2) { // case that int x; simple declaration
        check_redefinition(tokens[1]);
        fill_with_garbage(tokens[0], tokens[1]);
    } else if (tokens[2] == "=" && !is_declared_variable(tokens[3])) { // case if int x = 5; like declaration
        check_redefinition(tokens[1]);
        if (!is_number(tokens[3]) && !has_first_and_last_double_quotes(tokens[3]) && !has_first_and_last_single_quotes(tokens[3]) && tokens[3] != "false" && tokens[3] != "true") {
            std::cout << "Error: "  << tokens[3] <<  " was not declared in this scope " << std::endl; // int x = y, y is not declared
            return;
        }
        fill_var_with_given_value(tokens[0], tokens[1], tokens[3]);
    } else if (tokens[2] == "=" && is_declared_variable(tokens[3])) { // int x = y, y is declared
        check_redefinition(tokens[1]);
        fill_var_with_given_variable_value(tokens[0], tokens[1], tokens[3]);
    } 
}

void Interpreter::zero_token_is_var(std::vector<std::string>& tokens) {
    if (!is_declared_variable(tokens[2])) { // case x = 5; // x is declared
        if (!is_number(tokens[2]) && !has_first_and_last_double_quotes(tokens[2]) && !has_first_and_last_single_quotes(tokens[2]) && tokens[2] != "false" && tokens[2] != "true") {
            std::cout << "Error: "  << tokens[2] <<  " was not declared in this scope " << std::endl; // int x = y, y is not declared
            return;
        }
        fill_var_with_given_value(type_of_var(tokens[0]), tokens[0], tokens[2]);
    } else { // case x = y;
        fill_var_with_given_variable_value(tokens[0], tokens[1], tokens[2]);
    }
}

void Interpreter::cout(std::vector<std::string>& tokens) {
    if (is_number(tokens[2])) {
        std::cout << tokens[2];
    } else if (is_declared_variable(tokens[2])) {
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
    } else if (has_first_and_last_double_quotes(tokens[2])) {
        remove_double_quotes(tokens[2]);
        std::cout << tokens[2];
    } else if (has_first_and_last_single_quotes(tokens[2])) {
        remove_single_quotes(tokens[2]);
        std::cout << tokens[2];
    }
}

void Interpreter::fill_with_garbage(const std::string& row0, const std::string& row1) {
    if (row0 == "char") {
        std::pair<std::string, char> var{row0, 'A' + (std::rand() % 26)};
        char_vars[row1] = var;
    } else if (row0 == "int") {
        std::pair<std::string, int> var{row0, std::rand() % 100 + 1};
        integer_vars[row1] = var;
    } else if (row0 == "double") {
        std::pair<std::string, double> var{row0, var.second = static_cast<double>(std::rand()) / RAND_MAX};
        double_vars[row1] = var;
    } else if (row0 == "float") {
        std::pair<std::string, float> var{row0, static_cast<double>(std::rand()) / RAND_MAX};
        float_vars[row1] = var;
    } else if (row0 == "bool") {
        std::pair<std::string, bool> var{row0, false};
        bool_vars[row1] = var;
    } else if (row0 == "string") {
        std::pair<std::string, std::string> var{row0, ""};
        string_vars[row1] = var;
    }
} 

void Interpreter::fill_var_with_given_value(const std::string& row0, const std::string& row1, std::string& row3) {
    if (row0 == "char") {
        remove_single_quotes(row3);
        std::pair<std::string, char> var{row0, convert_to_char(row3)};
        char_vars[row1] = var;
    } else if (row0 == "int") {
        std::pair<std::string, int> var{row0, convert_to_int(row3)};
        integer_vars[row1] = var;
    } else if (row0 == "double") {
        std::pair<std::string, double> var{row0, convert_to_double(row3)};
        double_vars[row1] = var;
    } else if (row0 == "float") {
        std::pair<std::string, float> var{row0, convert_to_float(row3)};
        float_vars[row1] = var;
    } else if (row0 == "bool") {
        std::pair<std::string, bool> var{row0, convert_to_bool(row3)};
        bool_vars[row1] = var;
    } else if (row0 == "string") {
        remove_double_quotes(string_vars[row3].first);
        std::pair<std::string, std::string> var{row0, row3};
        string_vars[row1] = var;
    }   
}

void Interpreter::fill_var_with_given_variable_value(const std::string& row0, const std::string& row1, std::string& row2) {
    if (row0 == "char") {
        remove_single_quotes(row2);
        std::pair<std::string, char> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<char>(row2, type_of_var(row2)); 
        char_vars[row1] = var;
    } else if (row0 == "int") {
        std::pair<std::string, int> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<int>(type_of_var(row2), row2); 
        integer_vars[row1] = var;
    } else if (row0 == "double") {
        std::pair<std::string, double> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<double>(type_of_var(row2), row2); 
        double_vars[row1] = var;
    } else if (row0 == "float") {
        std::pair<std::string, float> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<float>(type_of_var(row2), row2); 
    } else if (row0 == "bool") {
        std::pair<std::string, bool> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<bool>(type_of_var(row2), row2);
        bool_vars[row1] = var;
    } else if (row0 == "string") {
        remove_double_quotes(string_vars[row2].first);
        std::pair<std::string, std::string> var(type_of_var(row2), row0);
        string_vars[row1] = var;
    }   
}

bool Interpreter::is_single_char(const std::string& str) {
    return str.length() == 1;
}

char Interpreter::convert_to_char(const std::string& str) {
    if (is_single_char(str)) {
        return str[0]; // Extract and return the first character
    } else {
        throw std::invalid_argument("Given l-value is not single character.Cannot convert to char");
    }
}

int Interpreter::convert_to_int(const std::string& str) {
    try {
        if (str == "false") {
            return 0;
        } else if (str == "true") {
            return 1;
        }
        return std::stoi(str); // Attempt to convert to int
    } catch (const std::invalid_argument&) {
        throw std::invalid_argument("Cannot convert to integer");
    } catch (const std::out_of_range&) {
        throw std::invalid_argument("Cannot convert to integer");
    }
}

double Interpreter::convert_to_double(const std::string& str) {
    try {
        return std::stod(str); // Attempt to convert to double
    } catch (const std::invalid_argument&) {
        throw std::invalid_argument("Cannot convert to double");
    } catch (const std::out_of_range&) {
        throw std::invalid_argument("Cannot convert to double");
    }
}

float Interpreter::convert_to_float(const std::string& str) {
    try {
        if (str == "false") {
            return 0;
        } else if (str == "true") {
            return 1;
        }
        return std::stof(str); // Attempt to convert to double
    } catch (const std::invalid_argument&) {
        throw std::invalid_argument("Cannot convert to float.");
    } catch (const std::out_of_range&) {
        throw std::invalid_argument("Cannot convert to float.");
    }
}

bool Interpreter::convert_to_bool(const std::string& str) {
    if (str == "true") {
        return true;
    } else if (str == "false") {
        return false;
    } else if (str == "0") {
        return false;    
    } else if (is_number(str)) {
        return true;    
    } else {
        throw std::invalid_argument("Invalid argument: Cannot convert to bool.");
    }
}

std::string Interpreter::type_of_var(const std::string& token) {
    if (char_vars.find(token) != char_vars.end()) {
        return "char";
    }
    else if (integer_vars.find(token) != integer_vars.end()) {
        return "int";
    }
    else if (float_vars.find(token) != float_vars.end()) {
        return "float";
    }
    else if (double_vars.find(token) != double_vars.end()) {
        return "double";
    }
    else if (bool_vars.find(token) != bool_vars.end()) {
        return "bool";
    }
    else if (string_vars.find(token) != string_vars.end()) {
        return "string";
    }
    return "";
}

template <typename T>
T Interpreter::get_value_of_var_by_type(const std::string& type, const std::string& var) {
    if (type == "char") {
        return get_value_of_char(var);
    } else if (type == "int") {
        return get_value_of_int(var);
    } else if (type == "double") {
        return get_value_of_double(var);
    } else if (type == "float") {
        return get_value_of_float(var);
    } else if (type == "bool") {
        return get_value_of_bool(var);
    } else if (type == "string") {
         //
    } 
}

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

void Interpreter::check_redefinition(const std::string& str) {
    try {
        if (is_declared_variable(str)) {
            throw std::runtime_error("Redefinition of variable");
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
    
}

