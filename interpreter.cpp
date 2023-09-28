#include "interpreter.h"

Interpreter::Interpreter() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
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
    bool inside_while = false;
    bool do_not_enter_while = false;

    std::string line;
    while (std::getline(file, line)) { // Read the file line by line
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) { // Split the line into words by spaces
            tokens.push_back(token);
        }

        const std::string str = (if_inside.size() != 0) ? "inside_if" : "";

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

        if (do_not_enter_while && !(tokens[0] == "}")) {
            continue;
        }

        // is variable declaration parsing
        if (tokens[0] == "char" || tokens[0] == "int" || tokens[0] == "double" || tokens[0] == "float" || tokens[0] == "bool" || tokens[0] == "string") {
            zero_token_is_type_parse(tokens, inside_if_statement); // token[0] is type // gaving flags to not take as redefinition declaration of same name variable in if statment
        } 
        
        
        else if (is_declared_variable(tokens[0] + str)) { // parse exprression where first token is variable
            first_token_is_variable_parse(tokens, inside_if_statement);
        } else if (tokens[0] == "std::cout" && tokens[1] == "<<" && tokens.size() == 3) {
            cout(tokens, inside_if_statement);
        } else if (tokens[0] == "if" && brace_exist(tokens)) { // handling if statment
            parse_if_statement(tokens, inside_if_statement, do_not_enter_if_statment);
            if (do_not_enter_if_statment) {
                continue;
            }

            // checking braces correctness in if statement
            for (int i = 0; i < find_brace(tokens).size(); ++i) {
                char brace = find_brace(tokens)[i];
                 if (brace == '{') {
                    inside_if_statement = true;
                    continue;
                } else {
                    std::cout << "syntax error in if statment" << std::endl;
                    return;
                }
            }

        } else if (tokens[0] == "}" && inside_if_statement) {
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
        } else if (tokens[0] == "}" && !inside_if_statement ) { /// ? karoxa avelorda
            do_not_enter_if_statment = false; // het e, darcnum false if ic durs galuc heto
        } 

        else if (tokens[0] == "if" && brace_exist(tokens)) {
            parse_while(tokens, inside_while, do_not_enter_while);
        }
        
        // else if (!is_declared_variable(tokens[0])) { // cheack if variable is declared
        //     if ((tokens.size() == 3 && tokens[1] == "=") || (tokens.size() == 5  && tokens[1] == "=" && (tokens[3] == "+" || tokens[3] == "-" || tokens[3] == "*" || tokens[3] == "/")) || (tokens.size() == 3 && (tokens[1] == "+=" || tokens[1] == "-=" || tokens[1] == "*=" || tokens[1] == "/="))) {
        //        std::cout << "Variable " << tokens[0] << " is not declared" << std::endl;
        //     } 
        // }
        
        rows[eip] = tokens;
        ++eip;
    }

    if (!st.empty()) {
        std::cout << "There is problem with braces" << std::endl;
        return;
    }

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

void Interpreter::parse_while(std::vector<std::string>& tokens, bool& while_flag, bool& while_enter_flag) {
    if (find_brace(tokens).size() > 3) {
        std::cout << "extraneous brace" << std::endl;
        return;
    } if (find_brace(tokens).size() < 3) {
        std::cout << "missing brace" << std::endl;
        return;
    }   

    extract_paren(tokens[1], '('); 
    extract_paren(tokens[3], ')');

    //checking condition
    if (tokens[2] == "==") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], while_flag);
        if (pair.first != pair.second) { // do not enter if
            while_enter_flag = true;
            //continue;
        } 
    } else if (tokens[2] == "!=") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], while_flag);
        if (pair.first == pair.second) { // do not enter if
            while_enter_flag = true;
            //continue;
        }  
    } else if (tokens[2] == ">") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], while_flag);
        if (pair.first <= pair.second) { // do not enter if
            while_enter_flag = true;
            //continue;
        }
    } else if (tokens[2] == ">=") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], while_flag);
        if (pair.first < pair.second) { // do not enter if
            while_enter_flag = true;
            //continue;
        }
      } else if (tokens[2] == "<") {
        //std::cout << tokens[1] << " " << tokens[3];
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], while_flag);
        if (pair.first >= pair.second) { // do not enter if
            while_enter_flag = true;
            //continue;
        }
    } else if (tokens[2] == "<=") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], while_flag);
        if (pair.first > pair.second) { // do not enter if
            while_enter_flag = true;
            //continue;
        }
    }
}

void Interpreter::parse_if_statement(std::vector<std::string>& tokens, bool& if_flag, bool& if_enter_flag) {
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
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], if_flag);
        if (pair.first != pair.second) { // do not enter if
            if_enter_flag = true;
            //continue;
        } 
    } else if (tokens[2] == "!=") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], if_flag);
        if (pair.first == pair.second) { // do not enter if
            if_enter_flag = true;
            //continue;
        }  
    } else if (tokens[2] == ">") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], if_flag);
        if (pair.first <= pair.second) { // do not enter if
            if_enter_flag = true;
            //continue;
        }
    } else if (tokens[2] == ">=") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], if_flag);
        if (pair.first < pair.second) { // do not enter if
            if_enter_flag = true;
            //continue;
        }
      } else if (tokens[2] == "<") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], if_flag);
        if (pair.first >= pair.second) { // do not enter if
            if_enter_flag = true;
            //continue;
        }
    } else if (tokens[2] == "<=") {
        std::pair<std::string, std::string> pair = check_variables_inside(tokens[1], tokens[3], if_flag);
        if (pair.first > pair.second) { // do not enter if
            if_enter_flag = true;
            //continue;
        }
    }
}

template <typename T>
void Interpreter::delete_key_from_map(std::map<std::string, T>& map, const std::string& key) {
    auto it = map.find(key);
    if (it != map.end()) {
        map.erase(it);
    }
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

void Interpreter::cout(std::vector<std::string>& tokens, bool flag) {
    const std::string str = (if_inside.size() != 0) ? "inside_if" : "";
    if (is_number(tokens[2])) {
        std::cout << tokens[2];
    } else if (is_declared_variable(tokens[2] + str)) {
        if (type_of_var(tokens[2] + str) == "char") {
            std::cout << char_vars[tokens[2] + str].second;
        } else if (type_of_var(tokens[2] + str) == "int") {
            std::cout << std::endl;
            std::cout << integer_vars[tokens[2] + str].second;
            std::cout << std::endl;
        } else if (type_of_var(tokens[2] + str) == "double") {
            std::cout << double_vars[tokens[2] + str].second;
        } else if (type_of_var(tokens[2] + str) == "float") {
            std::cout << float_vars[tokens[2] + str].second;
        } else if (type_of_var(tokens[2] + str) == "bool") {
            std::cout << bool_vars[tokens[2] + str].second;
        } else if (type_of_var(tokens[2] + str) == "string") {
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
    std::string tmp1 = row1;
    std::string tmp2 = row3;
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
        row1 = tmp1;
        return;
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
        row1 = tmp1;
        return;
    }   
    row1 = tmp1;
    row3 = tmp2;
}

void Interpreter::fill_var_with_given_variable_value(const std::string& row0, std::string& row1, std::string& row2, const std::string& str) {
    std::string tmp = row1;
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
        var.second = get_value_of_var_by_type<char>(type_of_var(row2), row2); 
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
    row1 = tmp;
}


void Interpreter::adding_cases(std::vector<std::string>& tokens, std::string& dest_var, std::string& str1, std::string& str2) {
    std::string type = (tokens.size() == 6) ? tokens[0] : type_of_var(dest_var);
    std::string tmp;

    if (type == "char") {
        
        if (has_first_and_last_single_quotes(str2)) {
            remove_single_quotes(str2);
        }
        add<char>(tmp, str1, str2);
        char_vars[dest_var].second = convert_to_type<char>(tmp);
    } else if (type == "int") {
        add<double>(tmp, str1, str2);
        integer_vars[dest_var].second = convert_to_type<int>(tmp);
        // std::cout << dest_var << "))" << integer_vars[dest_var].second;
    } else if (type == "double") {
        add<double>(tmp, str1, str2);
        double_vars[dest_var].second  = convert_to_type<double>(tmp);
    } else if (type == "float") {
        add<float>(tmp, str1, str2);
        float_vars[dest_var].second = convert_to_type<float>(tmp);
    } else if (type == "bool") {
        add<bool>(tmp, str1, str2);
        bool_vars[dest_var].second = convert_to_type<bool>(tmp);
    } else if (type == "string") {
        if (has_first_and_last_double_quotes(str2)) {
            remove_double_quotes(str2);
        } 
        add<std::string>(tmp, str1, str2);
        string_vars[dest_var].second = tmp;
    } 
}

void Interpreter::sub_cases(std::vector<std::string>& tokens, std::string& dest_var, std::string& str1, std::string& str2) {
    std::string type = (tokens.size() == 6) ? tokens[0] : type_of_var(dest_var);
    std::string tmp;
    if (type == "char") {
        if (has_first_and_last_single_quotes(str2)) {
            remove_single_quotes(str2);
        }
        sub<char>(tmp, str1, str2);
        char_vars[dest_var].second = convert_to_type<char>(tmp);
    } else if (type == "int") {
        sub<double>(tmp, str1, str2); // for correct casting while there are different types like int x = y(int) - z(double)
        integer_vars[dest_var].second = convert_to_type<int>(tmp);
    } else if (type == "double") {
        sub<double>(tmp, str1, str2);
        double_vars[dest_var].second  = convert_to_type<double>(tmp);
    } else if (type == "float") {
        sub<float>(tmp, str1, str2);
        float_vars[dest_var].second = convert_to_type<float>(tmp);
    } else if (type == "bool") {
        sub<bool>(tmp, str1, str2);
        float_vars[dest_var].second = convert_to_type<bool>(tmp);
    } else if (type == "string") {
        throw std::runtime_error("Subtraction not supported for the string.");
    } 
}

void Interpreter::mul_cases(std::vector<std::string>& tokens, std::string& dest_var, std::string& str1, std::string& str2) {
    std::string type = (tokens.size() == 6) ? tokens[0] : type_of_var(dest_var);
    std::string tmp;
    if (type == "char") {
        if (has_first_and_last_single_quotes(str2)) {
            remove_single_quotes(str2);
        }
        multiply<char>(tmp, str1, str2);
        char_vars[dest_var].second = convert_to_type<char>(tmp);
    } else if (type == "int") {
        multiply<double>(tmp, str1, str2); // for correct casting while there are different types like int x = y(int) - z(double)
        integer_vars[dest_var].second = convert_to_type<int>(tmp);
    } else if (type == "double") {
        multiply<double>(tmp, str1, str2);
        double_vars[dest_var].second  = convert_to_type<double>(tmp);
    } else if (type == "float") {
        multiply<float>(tmp, str1, str2);
        float_vars[dest_var].second = convert_to_type<float>(tmp);
    } else if (type == "bool") {
        multiply<bool>(tmp, str1, str2);
        float_vars[dest_var].second = convert_to_type<bool>(tmp);
    } else if (type == "string") {
        throw std::runtime_error("Multiplication not supported for the string.");
    }
}

void Interpreter::div_cases(std::vector<std::string>& tokens, std::string& dest_var, std::string& str1, std::string& str2) {
    std::string type = (tokens.size() == 6) ? tokens[0] : type_of_var(dest_var);
    std::string tmp;
    if (type == "char") {
       if (has_first_and_last_single_quotes(str2)) {
            remove_single_quotes(str2);
        }
        divide<char>(tmp, str1, str2);
        char_vars[dest_var].second = convert_to_type<char>(tmp);
    } else if (type == "int") {
        divide<double>(tmp, str1, str2); // for correct casting while there are different types like int x = y(int) - z(double)
        integer_vars[dest_var].second = convert_to_type<int>(tmp);
    } else if (type == "double") {
        divide<double>(tmp, str1, str2);
        double_vars[dest_var].second  = convert_to_type<double>(tmp);
    } else if (type  == "float") {
        divide<float>(tmp, str1, str2);
        float_vars[dest_var].second = convert_to_type<float>(tmp);
    } else if (type == "bool") {
        divide<bool>(tmp, str1, str2);
        float_vars[dest_var].second = convert_to_type<bool>(tmp);
    } else if (type == "string") {
        throw std::runtime_error("Division not supported for the string.");
    }
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
    std::cout << var;

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




