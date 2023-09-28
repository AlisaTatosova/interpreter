#include "interpreter.h"

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

bool Interpreter::is_one_byte_integer(int num) {
    // Check if the number is within the range of an 8-bit signed integer (-128 to 127)
    if (num >= -128 && num <= 127) {
        return true;
    }
    // Check if the number is within the range of an 8-bit unsigned integer (0 to 255)
    if (num >= 0 && num <= 255) {
        return true;
    }
    return false;
}

std::string Interpreter::extract_paren(std::string& input, char brace) {
    size_t pos = input.find(brace);  // Find the position of the first '('
    if (pos != std::string::npos) {  // Check if '(' was found
        input.erase(pos, 1);   // Extract the substring up to the first '('
    }
}

bool Interpreter::last_char_is_semicolon(std::string& str) {
    if (str[str.size() - 1] == ';') {
        return true;
    }
    return false;
}

bool Interpreter::last_char_is_scope(std::string& str) {
    if (str[str.size() - 1] == '}' || str[str.size() - 1] == '{') {
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

bool Interpreter::check_existence_of_semicolons(std::vector<std::string>& tokens, int& eip) {
    if (eip > 1 && tokens[tokens.size() - 1] != ";" && !last_char_is_semicolon(tokens[tokens.size() - 1])) {
        if (!(tokens[tokens.size()- 1] == "}" || tokens[tokens.size()- 1] == "{" || last_char_is_scope(tokens[tokens.size() - 1]))) {
            std::cout << "You forgot ; at the end of line" << std::endl;
            return false;
        }
    }  
    return true;
}

// removing ; s from code
void Interpreter::remove_semicolons(std::vector<std::string>& tokens) {
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

void Interpreter::is_valid_parentheses(std::stack<char>& st, char str) {
    if (!st.empty()) {
        if (st.top() == '{' && str == '}') {
            st.pop();
        } else if (st.top() == '(' && str == ')') {
            st.pop(); 
        } else if (st.top() == '[' && str == ']') {
            st.pop();
        } else {
            st.push(str);
        }
    } else {
        st.push(str);
    }
}

bool Interpreter::brace_exist(const std::vector<std::string>& vec) {
    for (const std::string& str : vec) {
        size_t open_brace1 = str.find("{");
        size_t close_brace1 = str.find("}");
        size_t open_brace2 = str.find("(");
        size_t close_brace2 = str.find(")");
        size_t open_brace3 = str.find("[");
        size_t close_brace3 = str.find("]");

        if (open_brace1 != std::string::npos || close_brace1 != std::string::npos || open_brace2 != std::string::npos || close_brace2 != std::string::npos || open_brace3 != std::string::npos || close_brace3 != std::string::npos) {
            return true;  // Return true if either "{" or "}" is found
        }
    }
    return false;
}

std::string Interpreter::find_brace(const std::vector<std::string>& vec) {
    std::string found_braces;

    for (const std::string& str : vec) {
        for (char ch : str) {
            if (ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == '[' || ch == ']') {
                found_braces += ch;
            }
        }
    }

    return found_braces;
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

bool Interpreter::is_single_char(const std::string& str) {
    return str.length() == 1;
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
