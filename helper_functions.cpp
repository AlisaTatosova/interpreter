#include "interpreter.h"

// checking if given string is number
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

// checking if given num is one byte integer
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

// separating name and size in array declaration
std::pair<std::string, std::string> Interpreter::separate_name_and_size_in_array_declaration(const std::string& str) {
    size_t open = str.find('[');
    size_t close = str.find(']');
    std::string extract1;
    std::string extract2;

    if (open != std::string::npos && str.front() != '[' && close != std::string::npos && close > open) {
        extract1 = str.substr(0, open);
        if (is_number(extract1)) {
            throw std::runtime_error("Error: the name of array can not be a number");
        }
        extract2 = str.substr(open + 1, close - open - 1);
        if (is_declared_variable(extract2)) {
            get_var_value_inside(extract2);
        } else if (!is_declared_variable(extract2) && !is_number(extract2)) {
            throw std::runtime_error("Error: the size of array must be const literal");
        }
    } 

    return {extract1, extract2}; // extract1 - name of arrray , extract2 - size
}

// extracting parentheses
std::string Interpreter::extract_paren(const std::string& str, char brace) {
    std::string extract;
    if (brace == '(') {
        extract = str.substr(1, str.size() - 1);
    } else if (brace == ')') {
        extract = str.substr(0, str.size() - 1);
    }
    return extract;
}

// checking if last char is semicolon
bool Interpreter::last_char_is_semicolon(std::string& str) {
    if (str[str.size() - 1] == ';') {
        return true;
    }
    return false;
}

// checking if last char is scope
bool Interpreter::last_char_is_scope(std::string& str) {
    if (str[str.size() - 1] == '}' || str[str.size() - 1] == '{') {
        return true;
    }
    return false;
}

// checking if string contains semicolon
bool Interpreter::contains_semicolon(const std::string& str) {
    return str.find(';') != std::string::npos;
}

// removing semicolon
void Interpreter::remove_semicolon(std::string& input) {
    input = input.substr(0, input.size() - 1); // extract the substring without the semicolon
}

// checking existence of semicolon
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

// concatenate by removing spaces
std::string Interpreter::concatenate_and_remove_spaces(const std::vector<std::string>& vector) {
    std::string concat;
    for (const std::string& str : vector) {
        concat += str;
    }
    concat.erase(std::remove_if(concat.begin(), concat.end(), ::isspace), concat.end());
    return concat;
}

// removing double quotes
void Interpreter::remove_double_quotes(std::string& input) {
    if (input.front() == '"' && input.back() == '"') {
        input = input.substr(1, input.length() - 2);
    } else {
        throw std::invalid_argument("There is no double quotes "" ");
    }
}

// removing single quotes
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

// checking existence of first and last double quotes
bool Interpreter::has_first_and_last_double_quotes(const std::string& input) {
    if (input.size() >= 2) {
        return (input.front() == '"' && input.back() == '"');
    } else {
        return false; // The string is too short to have first and last quotes
    }
}

// checking existence of first and last single quotes
bool Interpreter::has_first_and_last_single_quotes(const std::string& input) {
    if (input.size() >= 2) {
        return (input.front() == '\'' && input.back() == '\'');
    } else {
        return false; // The string is too short to have first and last single quotes
    }
}

// checking for valid parentheses
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

// checking if braces exist
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

// finding brace
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

bool Interpreter::find_open_close_brace(const std::string& str, char brace) {
        for (char ch : str) {
            if (ch == brace) {
                return true;
            }
        }

    return false;
}

bool Interpreter::start_with_plus_plus(const std::string& str) {
    return str.substr(0, 2) == "++";
}

// Function to check if a string ends with "++"
bool Interpreter::end_with_plus_plus(const std::string& str) {
    return str.size() >= 2 && str.substr(str.size() - 2, 2) == "++";
}

bool Interpreter::start_with_minus_minus(const std::string& str) {
    return str.substr(0, 2) == "--";
}

// Function to check if a string ends with "++"
bool Interpreter::end_with_minus_minus(const std::string& str) {
    return str.size() >= 2 && str.substr(str.size() - 2, 2) == "--";
}

// Function to extract substrings containing "++" at the beginning or end
std::string Interpreter::extract_minus_minus(const std::string& input) {
    std::string result = input;;
    std::string plus_plus = "--";
    size_t pos = 0;

    while ((pos = result.find(plus_plus)) != std::string::npos) {
        if (pos == 0 || pos == result.size() - 2) {
            result.erase(pos, 2);
        }
    }
    return result;
}

// Function to extract substrings containing "++" at the beginning or end
std::string Interpreter::extract_plus_plus(const std::string& input) {
    std::string result = input;;
    std::string plus_plus = "++";
    size_t pos = 0;

    while ((pos = result.find(plus_plus)) != std::string::npos) {
        if (pos == 0 || pos == result.size() - 2) {
            result.erase(pos, 2);
        }
    }
    return result;
}

// checking redefinition
void Interpreter::check_redefinition(const std::string& str) {
    try {
        if (is_declared_variable(str)) {
            throw std::runtime_error("Redefinition of variable ");
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
    
} 

bool Interpreter::check_open_parent(const std::string& str) {
    if (str.front() == '(') {
        return true;
    }
    return false;
}


bool Interpreter::check_close_parent(const std::string& str) {
    if (str.back() == ')') {
        return true;
    }
    return false;
}

bool Interpreter::check_open_figure_parent(const std::string& str) {
    if (str.front() == '{') {
        return true;
    }
    return false;
}

// extracting figure scope
bool Interpreter::check_close_figure_parent(const std::string& str) {
    if (str.back() == '}') {
        return true;
    }
    return false;
}

// extracting figure scope
std::string Interpreter::extract_figure_paren(const std::string& str, char brace) {
    std::string extract;
    if (brace == '{') {
        extract = str.substr(1, str.size() - 1);
    } else if (brace == '}') {
        extract = str.substr(0, str.size() - 1);
    }
    return extract;
}

// removing comma
std::string Interpreter::remove_comma_at_the_end(const std::string& str) {
    std::string res;
    if (str.back() != ',') {
        throw std::runtime_error("There is skipped comma in initializer list");
    } else {
        res = str.substr(0, str.size() - 1);
    }
    return res;
}

// checking if string is single character
bool Interpreter::is_single_char(const std::string& str) {
    return str.length() == 1;
}

// searching string in vector
bool Interpreter::find_string_in_vector(const std::vector<std::string>& strings, const std::string& target) {
    // Use std::find to search for the target string in the vector
    auto it = std::find(strings.begin(), strings.end(), target);

    // Check if the string was found
    if (it != strings.end()) {
        return true; // Found the string
    } else {
        return false; // String not found
    }
}

// getting type of variable or array
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
    
    else if (char_arr.find(token) != char_arr.end()) {
        return "char";
    }
    else if (int_arr.find(token) != int_arr.end()) {
        return "int";
    }
    else if (float_arr.find(token) != float_arr.end()) {
        return "float";
    }
    else if (double_arr.find(token) != double_arr.end()) {
        return "double";
    }
    else if (bool_arr.find(token) != bool_arr.end()) {
        return "bool";
    }
   
    return "";
}
