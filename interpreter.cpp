#include "interpreter.h"

Interpreter::Interpreter() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
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

template <>
char Interpreter::convert_to_type(std::string& str) {
    if (is_single_char(str)) {
        return str[0];
    } else if (is_one_byte_integer(std::stoi(str))) {
        return static_cast<char>(std::stoi(str));
    } else {
        throw std::invalid_argument("Given l-value is not a single character. Cannot convert to char");
    }
}

template <>
int Interpreter::convert_to_type(std::string& str) {
    if (has_first_and_last_double_quotes(str)) {
        remove_double_quotes(str);
    } else if (has_first_and_last_single_quotes(str)) {
        remove_single_quotes(str);
    }

    if (str == "false") {
        return 0;
    } else if (str == "true") {
        return 1;
    } else if (is_single_char(str) && !is_number(str)) {
        return static_cast<char>(str[0]);
    }

    return std::stoi(str);
}

template <>
double Interpreter::convert_to_type(std::string& str) {
    if (has_first_and_last_single_quotes(str)) {
        remove_single_quotes(str);
    }
    if (str.size() == 1 && !is_number(str)) {
        // If the string is a single character, convert it to its ASCII value as a double
        return static_cast<double>(str[0]);
    } else if (is_number(str)) {
        return std::stod(str);
    } 
    else {
        // Conversion failed, and it's not a single character
        throw std::invalid_argument("Invalid input for conversion to double.");
    }
}

template <>
float Interpreter::convert_to_type(std::string& str) {
    if (str == "false") {
        return 0.0f;
    } else if (str == "true") {
        return 1.0f;
    }
    return std::stof(str);
}

template <>
bool Interpreter::convert_to_type(std::string& str) {
    if (str == "false" || str == "0") {
        return false;
    } else if (str == "true" || str == "1" || is_number(str) || is_single_char(str)) {
        return true;
    }else {
        throw std::invalid_argument("Invalid argument: Cannot convert to bool.");
    }
}

template <>
std::string Interpreter::convert_to_type<std::string>(std::string& str) {
    return str;  // Just return the input string for std::string
}

void Interpreter::parse(std::ifstream& file, int i) {
    if (!file.is_open()) {
        std::cerr << "Unable to open the file." << std::endl;
        return;
    }

    int eip = 0; // instruction ptr
    bool main_exists = false;
    bool header_file_exists = false;
    std::stack<char> st;
    bool inside_if_statement = false;
    bool do_not_enter_if_statment = false;

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

        // checking correctness of braces
        if (brace_exist(tokens)) {
            for (int i = 0; i < find_brace(tokens).size(); ++i) {
                char brace = find_brace(tokens)[i];
                is_valid_parentheses(st, brace);
            }
        }

        // checking existence of <iostream> header file
        if (!header_file_exists) {
            if(!parse_header_file(tokens, eip, header_file_exists)) {
                return;
            } else {
                continue;
            }
        } 

        // checking existence of int main
        if (!main_exists) {
            if (!parse_main(tokens, eip, main_exists)) {
                return;
            } else {
                continue;
            }
        }

        // checking for ; existence at the end of code 
        if (!check_existence_of_semicolons(tokens, eip)) {
            return;
        }
        // removing ; s from code
        remove_semicolons(tokens);


        // it is for jmping if statment body if statment is not true
        if (do_not_enter_if_statment && !(tokens[0] == "}")) {
            continue;
        }


        if (tokens[0] == "char" || tokens[0] == "int" || tokens[0] == "double" || tokens[0] == "float" || tokens[0] == "bool" || tokens[0] == "string") {
            zero_token_is_type(tokens, inside_if_statement); // token[0] is type // gaving flags to not take as redefinition declaration of same name variable in if statment
        } else if (tokens.size() == 3 && is_declared_variable(tokens[0]) && tokens[1] == "=") {
            zero_token_is_var(tokens, inside_if_statement); // // token[0] is variable
        } else if (tokens[0] == "std::cout" && tokens[1] == "<<" && tokens.size() == 3) {
            cout(tokens, inside_if_statement);
        } 
        
        else if (tokens.size() == 5 && is_declared_variable(tokens[0]) && tokens[1] == "=") { 
            std::string tmp;
            if (!if_inside.empty()) { // if if_inside vector is not empty it means that we are inside if statment // and the declaration of variable with same name as outside of if will not cause redefinition and it will have another name as varnameinside_if
                tmp = tokens[0] + "inside_if";
            } else {
                tmp = tokens[0];
            }

           check_vars(tokens[2], tokens[4], inside_if_statement); // checking types of op1 and op2 before some operation and make op1 and op2 as values inside them like op1 = x, op2 = y, after this function op1 = (value inside x), op2 = (value inside y)
            if (tokens[3] == "+") {
                adding_cases(tokens, tmp, 2, 4);
            } else if (tokens[3] == "-") {
                sub_cases(tokens, tmp, 2, 4);
            } else if (tokens[3] == "*") {
                mul_cases(tokens, tmp, 2, 4);
            } else if (tokens[3] == "/") {
                div_cases(tokens, tmp, 2, 4);
            }
        } else if (tokens.size() == 3 && is_declared_variable(tokens[0])) {
            std::string tmp;
            if (!if_inside.empty()) {
                tmp = tokens[0] + "inside_if";
            } else {
                tmp = tokens[0];
            }

            check_vars(tokens[0], tokens[2], inside_if_statement); // checking types of op1 and op2 before some operation to make precise castings
            if (tokens[1] == "+=") {
                adding_cases(tokens, tmp, 0, 2);
            } else if (tokens[1] == "-=") {
                sub_cases(tokens, tmp, 0, 2);
            } else if (tokens[1] == "*=") {
                mul_cases(tokens, tmp, 0, 2);
            } else if (tokens[1] == "/=") {
                div_cases(tokens, tmp, 0, 2);
            }           
        } 
        
        
        else if (tokens[0] == "if" && brace_exist(tokens)) { // handling if statment
            if (find_brace(tokens).size() > 3) {
                std::cout << "extraneous brace" << std::endl;
                return;
            } if (find_brace(tokens).size() < 3) {
                std::cout << "missing brace" << std::endl;
                return;
            }

            // extracting ( ) s from if statement now we have for example if x != y {
            extract_paren(tokens[1], '('); 
            extract_paren(tokens[3], ')');

            //checking condition
            if (tokens[2] == "==") {
                check_vars(tokens[1], tokens[3], inside_if_statement);
                if (tokens[1] != tokens[3]) { // do not enter if
                    do_not_enter_if_statment = true;
                    continue;
                } 
            } else if (tokens[2] == "!=") {
                check_vars(tokens[1], tokens[3], inside_if_statement);
                if (tokens[1] == tokens[3]) { // do not enter if
                    do_not_enter_if_statment = true;
                    continue;
                }  
            } else if (tokens[2] == ">") {
                check_vars(tokens[1], tokens[3], inside_if_statement);
                if (tokens[1] <= tokens[3]) { // do not enter if
                    do_not_enter_if_statment = true;
                    continue;
                }
            } else if (tokens[2] == ">=") {
                check_vars(tokens[1], tokens[3], inside_if_statement);
                if (tokens[1] < tokens[3]) { // do not enter if
                    do_not_enter_if_statment = true;
                    continue;
                }
            } else if (tokens[2] == "<") {
                check_vars(tokens[1], tokens[3], inside_if_statement);
                if (tokens[1] >= tokens[3]) { // do not enter if
                    do_not_enter_if_statment = true;
                    continue;
                }
            } else if (tokens[2] == "<=") {
                check_vars(tokens[1], tokens[3], inside_if_statement);
                if (tokens[1] > tokens[3]) { // do not enter if
                    do_not_enter_if_statment = true;
                    continue;
                }
            }


            // checking braces correctness in if statement
            for (int i = 0; i < find_brace(tokens).size(); ++i) {
                char brace = find_brace(tokens)[i];
                 if (brace == '{') {
                    inside_if_statement = true;
                    continue;
                } else {
                    std::cout << "wrong brace in if statment" << std::endl;
                    return;
                }
            }

            

        } else if (tokens[0] == "}") {
            if (inside_if_statement) {
                inside_if_statement = false; 
                // deleting variables declared inside if stament after closing } scope
                for (const std::string& key : if_inside) {
                    delete_key_from_map(char_vars, key);
                    delete_key_from_map(integer_vars, key);
                    delete_key_from_map(float_vars, key);
                    delete_key_from_map(double_vars, key);
                    delete_key_from_map(bool_vars, key);
                    delete_key_from_map(string_vars, key);
                }
                if_inside.clear(); // clearing vector which keeps the declared vars in if statment scope
            } else {
                do_not_enter_if_statment = false;
            }
        }
        

        rows[eip] = tokens;
        ++eip;
    }

    if (!st.empty()) {
        std::cout << "There is problem with braces" << std::endl;
        return;
    }

    // std::cout << std::endl;
    // std::cout << "here it is ";
    // std::cout << std::endl;

    // for (auto it = integer_vars.begin(); it != integer_vars.end(); ++it) {
    //     std::cout << it -> first  << " " << it -> second.second << " ";
    // }

    // std::cout << std::endl;
    // for (int i = 0; i < if_inside.size(); ++i) {
    //     std::cout << if_inside[i] << " ";
    // }

    file.close(); // Close the file when done
}

std::string Interpreter::extract_paren(std::string& input, char brace) {
    size_t pos = input.find(brace);  // Find the position of the first '('
    if (pos != std::string::npos) {  // Check if '(' was found
        input.erase(pos, 1);   // Extract the substring up to the first '('
    }
}

template <typename T>
void Interpreter::delete_key_from_map(std::map<std::string, T>& map, const std::string& key) {
    auto it = map.find(key);
    if (it != map.end()) {
        map.erase(it);
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

bool Interpreter::parse_header_file(std::vector<std::string>& tokens, int& eip, bool& header_file_exists) {
    // checking existence of <iostream> header file
    if (eip == 0 && tokens[0] == "#include" && tokens[1] == "<iostream>") { 
        ++eip;
        header_file_exists = true;
    } else if (eip == 0 && tokens[0] != "#include" && tokens[1] != "<iostream>") {
        std::cout <<  "There is no header file included. Please input the header file" << std::endl;
    } 
    return header_file_exists;
}

bool Interpreter::parse_main(std::vector<std::string>& tokens, int& eip, bool& main_exists) {
    // checking existence of int main
    if (eip == 1 && tokens[0] == "int") {
        std::string concat = concatenate_and_remove_spaces(tokens);
        if (concat == "intmain(){") {
            main_exists = true;
            rows[eip] = tokens;
            ++eip;
        } else if (concat == "intmain()") {
            //std::cout << "There is no opening scope '{' for main function" << std::endl;
        } else {
            std::cout << "There is no main function to start program" << std::endl;
        }
    }
    return main_exists;
}

void Interpreter::zero_token_is_type(std::vector<std::string>& tokens, bool flag) {
    const std::string str = (flag == true) ? "inside_if" : "";

    if (tokens.size() == 2) { // case that int x; simple declaration
        if (!flag) {
            check_redefinition(tokens[1]);
        }
        fill_with_garbage(tokens[0], tokens[1]);
    } else if (tokens[2] == "=" && !is_declared_variable(tokens[3])) { // case if int x = 5; like declaration
        if (!flag) {
            check_redefinition(tokens[1]);
        }
        if (!is_number(tokens[3]) && !has_first_and_last_double_quotes(tokens[3]) && !has_first_and_last_single_quotes(tokens[3]) && tokens[3] != "false" && tokens[3] != "true") {
            std::cout << "Error: "  << tokens[3] <<  " was not declared in this scope " << std::endl; // int x = y, y is not declared
            return;
        }
        
        fill_var_with_given_value(tokens[0], tokens[1], tokens[3], str);
        
    } else if (tokens[2] == "=" && is_declared_variable(tokens[3])) { // int x = y, y is declared
        if (!flag) {
            check_redefinition(tokens[1]);
        }
        fill_var_with_given_variable_value(tokens[0], tokens[1], tokens[3], str);
    } 
}

void Interpreter::zero_token_is_var(std::vector<std::string>& tokens, bool flag) {
    const std::string str = "";
    if (flag && is_declared_variable(tokens[0] + "inside_if")) {
        const std::string str = "inside_if";
    }
    
    if (!is_declared_variable(tokens[2])) { // case x = 5; // x is declared
        if (!is_number(tokens[2]) && !has_first_and_last_double_quotes(tokens[2]) && !has_first_and_last_single_quotes(tokens[2]) && tokens[2] != "false" && tokens[2] != "true") {
            std::cout << "Error: "  << tokens[2] <<  " was not declared in this scope " << std::endl; // int x = y, y is not declared
            return;
        }
        
        fill_var_with_given_value(type_of_var(tokens[0]), tokens[0], tokens[2], str);
        
    } else { // case x = y;
        fill_var_with_given_variable_value(tokens[0], tokens[1], tokens[2], str);
    }
}

void Interpreter::cout(std::vector<std::string>& tokens, bool flag) {
    const std::string str = (if_inside.size() != 0) ? "inside_if" : "";
    // es masy petqa dzel verevi parse() um petqa arandzin funkcia sarqel is declaraition 
    if (is_number(tokens[2])) {
        std::cout << tokens[2];
    } else if (is_declared_variable(tokens[2])) {
        if (type_of_var(tokens[2]) == "char") {
            std::cout << char_vars[tokens[2] + str].second;
        } else if (type_of_var(tokens[2]) == "int") {
            std::cout << integer_vars[tokens[2] + str].second;
        } else if (type_of_var(tokens[2]) == "double") {
            std::cout << double_vars[tokens[2] + str].second;
        } else if (type_of_var(tokens[2]) == "float") {
            std::cout << float_vars[tokens[2] + str].second;
        } else if (type_of_var(tokens[2]) == "bool") {
            std::cout << bool_vars[tokens[2] + str].second;
        } else if (type_of_var(tokens[2]) == "string") {
            std::cout << string_vars[tokens[2] + str].second;
        }
    } else if (has_first_and_last_double_quotes(tokens[2])) {
        remove_double_quotes(tokens[2]);
        std::cout << tokens[2];
    } else if (has_first_and_last_single_quotes(tokens[2])) {
        remove_single_quotes(tokens[2]);
        std::cout << tokens[2];
    }
}

// str i declared variable, like str = 5; we are making str as 5
void Interpreter::get_var_value_inside(std::string& str, bool flag) {
    const std::string name = (flag == true) ? "inside_if" : "";
    str += name;
    if (type_of_var(str) == "char") {
        str= std::to_string(char_vars[str].second);
    } else if (type_of_var(str) == "int") {
        str = std::to_string(integer_vars[str].second);
    } else if (type_of_var(str) == "double") {
        str = std::to_string(double_vars[str].second);
    } else if (type_of_var(str) == "float") {
        str = std::to_string(float_vars[str].second);
    } else if (type_of_var(str) == "bool") {
        str = std::to_string(bool_vars[str].second);
    } else if (type_of_var(str) == "string") {
        str = string_vars[str].second;
    }
}

//type , op1, op2
void Interpreter::check_vars(std::string& str1, std::string& str2, bool flag) {
    if (is_declared_variable(str1) && is_declared_variable(str2)) { // case if op1 and op2 are already declared variables
        
        if (type_of_var(str1) == "string" && type_of_var(str2) != "string") { 
            throw std::invalid_argument("The types are not muching for operation");
        } else if (type_of_var(str1) != "string" && type_of_var(str2) == "string") {
            throw std::invalid_argument("The types are not muching for operation");
        }   

        get_var_value_inside(str1, flag); //getting value inside var
        get_var_value_inside(str2, flag); //getting value inside var

        //std::cout << str1 << " " << str2;
    } else if (is_declared_variable(str1) && !is_declared_variable(str2)) { // case that first op is declared var second is literal
        // if one operand is string the other not it is not appropirate
        if (has_first_and_last_double_quotes(str2) && type_of_var(str1) != "string") {
            throw std::invalid_argument("The types are not muching for operation"); // 
        } 

        if (type_of_var(str1) == "string" && has_first_and_last_double_quotes(str2)) {
            remove_double_quotes(str2);
        } else if (has_first_and_last_single_quotes(str2)) {
            remove_single_quotes(str2);
        } 

        get_var_value_inside(str1, flag); //getting value inside var
        
    } else if (!is_declared_variable(str1) && is_declared_variable(str2)) { // case that first op is literal second declared var
        // if one operand is string the other not it is not appropirate
        if (has_first_and_last_double_quotes(str1) && type_of_var(str2) != "string") {
            throw std::invalid_argument("The types are not muching for operation"); // 
        } 

        if (type_of_var(str2) == "string" && has_first_and_last_double_quotes(str1)) {
            remove_double_quotes(str1);
        } else if (has_first_and_last_single_quotes(str1)) {
            remove_single_quotes(str1);
        } 

        get_var_value_inside(str2, flag); //getting value inside var

    } else if (!is_declared_variable(str1) && !is_declared_variable(str2)) { // if both operand1 and operand2 are literals
        // if one operand is string the other not it is not appropirate
        if (has_first_and_last_double_quotes(str1) && !has_first_and_last_double_quotes(str2)) {
            throw std::invalid_argument("The types are not muching for operation"); // 
        } else if (!has_first_and_last_double_quotes(str1) && has_first_and_last_double_quotes(str2)) {
            throw std::invalid_argument("The types are not muching for operation"); // 
        }  
        
        if (has_first_and_last_double_quotes(str1) && has_first_and_last_double_quotes(str2)) {
            remove_double_quotes(str1);
            remove_double_quotes(str2);
        } else if (has_first_and_last_single_quotes(str1) && has_first_and_last_single_quotes(str2)) {
            remove_single_quotes(str1);
            remove_single_quotes(str2);
        } 
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

void Interpreter::fill_var_with_given_value(const std::string& row0, std::string& row1, std::string& row3, const std::string& str) {
    row1 += str;
    if (str != "") {
        if_inside.push_back(row1);
    }

    if (row0 == "char") {
        if (has_first_and_last_single_quotes(row3)) {
            remove_single_quotes(row3);
        }
        std::pair<std::string, char> var{row0, convert_to_type<char>(row3)};
        char_vars[row1] = var;
    } else if (row0 == "int") {
        std::pair<std::string, int> var{row0, convert_to_type<int>(row3)};
        integer_vars[row1] = var;
    } else if (row0 == "double") {
        std::pair<std::string, double> var{row0, convert_to_type<double>(row3)};
        double_vars[row1] = var;
    } else if (row0 == "float") {
        std::pair<std::string, float> var{row0, convert_to_type<float>(row3)};
        float_vars[row1] = var;
    } else if (row0 == "bool") {
        std::pair<std::string, bool> var{row0, convert_to_type<bool>(row3)};
        bool_vars[row1] = var;
    } else if (row0 == "string") {
        if (has_first_and_last_double_quotes(row3)) {
            remove_double_quotes(row3);
        }
        std::pair<std::string, std::string> var{row0, row3};
        string_vars[row1] = var;
    }   
}

void Interpreter::fill_var_with_given_variable_value(const std::string& row0, std::string& row1, std::string& row2, const std::string& str) {
    row1 += str;
    if (str != "") {
        if_inside.push_back(row1);
    }

    if (row0 == "char") {
        if (has_first_and_last_single_quotes(row2)) {
            remove_single_quotes(row2);
        }
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
        float_vars[row1] = var;
    } else if (row0 == "bool") {
        std::pair<std::string, bool> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<bool>(type_of_var(row2), row2);
        bool_vars[row1] = var;
    } else if (row0 == "string") {
        if (has_first_and_last_double_quotes(row2)) {
            remove_double_quotes(row2);
        }
        std::pair<std::string, std::string> var(type_of_var(row2), row0);
        string_vars[row1] = var;
    }   
}


void Interpreter::adding_cases(std::vector<std::string>& tokens, std::string& dest_var, int first, int second) {
    if (type_of_var(dest_var) == "char") {
        if (has_first_and_last_single_quotes(tokens[second])) {
            remove_single_quotes(tokens[second]);
        }
        add<char>(tokens[0], tokens[first], tokens[second]);
        char_vars[dest_var].second = convert_to_type<char>(tokens[0]);
    } else if (type_of_var(dest_var) == "int") {
        add<double>(tokens[0], tokens[first], tokens[second]);
        integer_vars[dest_var].second = convert_to_type<int>(tokens[0]);
    } else if (type_of_var(dest_var) == "double") {
        add<double>(tokens[0], tokens[first], tokens[second]);
        double_vars[dest_var].second  = convert_to_type<double>(tokens[0]);
    } else if (type_of_var(dest_var)  == "float") {
        add<float>(tokens[0], tokens[first], tokens[second]);
        float_vars[dest_var].second = convert_to_type<float>(tokens[0]);
    } else if (type_of_var(dest_var) == "bool") {
        add<bool>(tokens[0], tokens[first], tokens[second]);
        bool_vars[dest_var].second = convert_to_type<bool>(tokens[0]);
    } else if (type_of_var(dest_var) == "string") {
        if (has_first_and_last_double_quotes(tokens[second])) {
            remove_double_quotes(tokens[second]);
        }
        add<std::string>(tokens[0], tokens[first], tokens[second]);
        string_vars[dest_var].second = tokens[0];
    }   
}

void Interpreter::sub_cases(std::vector<std::string>& tokens, std::string& dest_var, int first, int second) {
    if (type_of_var(dest_var) == "char") {
        if (has_first_and_last_single_quotes(tokens[second])) {
            remove_single_quotes(tokens[second]);
        }
        sub<char>(tokens[0], tokens[first], tokens[second]);
        char_vars[dest_var].second = convert_to_type<char>(tokens[0]);
    } else if (type_of_var(dest_var) == "int") {
        sub<double>(tokens[0], tokens[first], tokens[second]); // for correct casting while there are different types like int x = y(int) - z(double)
        integer_vars[dest_var].second = convert_to_type<int>(tokens[0]);
    } else if (type_of_var(dest_var) == "double") {
        sub<double>(tokens[0], tokens[first], tokens[second]);
        double_vars[dest_var].second  = convert_to_type<double>(tokens[0]);
    } else if (type_of_var(dest_var)  == "float") {
        sub<float>(tokens[0], tokens[first], tokens[second]);
        float_vars[dest_var].second = convert_to_type<float>(tokens[0]);
    } else if (type_of_var(dest_var) == "bool") {
        sub<bool>(tokens[0], tokens[first], tokens[second]);
        float_vars[dest_var].second = convert_to_type<bool>(tokens[0]);
    } else if (type_of_var(dest_var) == "string") {
        throw std::runtime_error("Subtraction not supported for the string.");
    } 
}

void Interpreter::mul_cases(std::vector<std::string>& tokens, std::string& dest_var, int first, int second) {
    if (type_of_var(dest_var) == "char") {
        if (has_first_and_last_single_quotes(tokens[second])) {
            remove_single_quotes(tokens[second]);
        }
        multiply<char>(tokens[0], tokens[first], tokens[second]);
        char_vars[dest_var].second = convert_to_type<char>(tokens[0]);
    } else if (type_of_var(dest_var) == "int") {
        multiply<double>(tokens[0], tokens[first], tokens[second]); // for correct casting while there are different types like int x = y(int) - z(double)
        integer_vars[dest_var].second = convert_to_type<int>(tokens[0]);
    } else if (type_of_var(dest_var) == "double") {
        multiply<double>(tokens[0], tokens[first], tokens[second]);
        double_vars[dest_var].second  = convert_to_type<double>(tokens[0]);
    } else if (type_of_var(dest_var)  == "float") {
        multiply<float>(tokens[0], tokens[first], tokens[second]);
        float_vars[dest_var].second = convert_to_type<float>(tokens[0]);
    } else if (type_of_var(dest_var) == "bool") {
        multiply<bool>(tokens[0], tokens[first], tokens[second]);
        float_vars[dest_var].second = convert_to_type<bool>(tokens[0]);
    } else if (type_of_var(dest_var) == "string") {
        throw std::runtime_error("Multiplication not supported for the string.");
    } 
}

void Interpreter::div_cases(std::vector<std::string>& tokens, std::string& dest_var, int first, int second) {
    if (type_of_var(dest_var) == "char") {
        if (has_first_and_last_single_quotes(tokens[second])) {
            remove_single_quotes(tokens[second]);
        }
        divide<char>(tokens[0], tokens[first], tokens[second]);
        char_vars[dest_var].second = convert_to_type<char>(tokens[0]);
    } else if (type_of_var(dest_var) == "int") {
        divide<double>(tokens[0], tokens[first], tokens[second]); // for correct casting while there are different types like int x = y(int) - z(double)
        integer_vars[dest_var].second = convert_to_type<int>(tokens[0]);
    } else if (type_of_var(dest_var) == "double") {
        divide<double>(tokens[0], tokens[first], tokens[second]);
        double_vars[dest_var].second  = convert_to_type<double>(tokens[0]);
    } else if (type_of_var(dest_var)  == "float") {
        divide<float>(tokens[0], tokens[first], tokens[second]);
        float_vars[dest_var].second = convert_to_type<float>(tokens[0]);
    } else if (type_of_var(dest_var) == "bool") {
        divide<bool>(tokens[0], tokens[first], tokens[second]);
        float_vars[dest_var].second = convert_to_type<bool>(tokens[0]);
    } else if (type_of_var(dest_var) == "string") {
        throw std::runtime_error("Division not supported for the string.");
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

template <typename T>
void Interpreter::add(std::string& res, std::string& str1, std::string& str2) {
    T operand1 = convert_to_type<T>(str1);
    T operand2 = convert_to_type<T>(str2);
    T result = operand1 + operand2;
    std::ostringstream ss;
    ss << result;
    res = ss.str();
}

template <typename T>
void Interpreter::sub(std::string& res, std::string& op1, std::string& op2) {
    T operand1 = convert_to_type<T>(op1);
    T operand2 = convert_to_type<T>(op2);
    T result = operand1 - operand2;
    std::ostringstream ss;
    ss << result;
    res = ss.str();
}

template <typename T>
void Interpreter::multiply(std::string& res, std::string& op1, std::string& op2) {
    T operand1 = convert_to_type<T>(op1);
    T operand2 = convert_to_type<T>(op2);
    T result = operand1 * operand2; 
    std::ostringstream ss;
    ss << result;
    res = ss.str();
}

template <typename T>
void Interpreter::divide(std::string& res, std::string& op1, std::string& op2) {
    T operand1 = convert_to_type<T>(op1);
    T operand2 = convert_to_type<T>(op2);
    T result = operand1 / operand2; 
    std::ostringstream ss;
    ss << result;
    res = ss.str();
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



