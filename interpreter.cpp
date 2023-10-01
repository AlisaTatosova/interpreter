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
    } else {
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
    } else {
        throw std::invalid_argument("Invalid argument: Cannot convert to bool.");
    }
}

template <>
std::string Interpreter::convert_to_type<std::string>(std::string& str) {
    return str;  // Just return the input string for std::string
}

void Interpreter::parse(std::ifstream& file) {
    if (!file.is_open()) {
        std::cerr << "Unable to open the file." << std::endl;
        return;
    }

    int eip = 0;
    std::stack<char> st;
    std::stack<int> inside_if_stack;
    std::stack<int> inside_while_stack;

    std::string line;
    while (std::getline(file, line)) { // Read the file line by line
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        
        // while (iss >> token) { // Split the line into words by spaces
        //     tokens.push_back(token);
        // }
        bool inside_quotes = false;
        std::string current_token;

        while (iss >> token) {
            // as we reading from sstream when we saw string with space as "hello world" we read it space by space but we have to read it as one sting so here this case is handled
            if (token.front() == '"' && (token.back() == '"' || token[token.size() - 2] == '"')) {
                // Entire quoted string is a single token, including the quotes.
                tokens.push_back(token);
            } else if (token.front() == '"' ) {
                inside_quotes = true;
                current_token = token;
            } else if (token.back() == '"' || token[token.size() - 2] == '"') {
                inside_quotes = false;
                current_token += " " + token;
                tokens.push_back(current_token);
            } else if (inside_quotes) {
                current_token += " " + token;
            } else {
                tokens.push_back(token);
            }
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

        if (tokens[0] == "if" && brace_exist(tokens)) { // handling if statment  
            std::cout << "push ";
            for_scopes[tokens[0] + std::to_string(eip)].first = eip;
            inside_if_stack.push(eip); 
        } else if (tokens[0] == "}" && !inside_if_stack.empty()) { 
            std::cout << "pop " << inside_if_stack.size() << " ";
            for_scopes["if" + std::to_string(inside_if_stack.top())].second = eip;
            inside_if_stack.pop(); // Pop the stack when exiting an "if" block
        } else if (tokens[0] == "while" && brace_exist(tokens)) { // handling if statment 
            std::cout << "push_While"; 
            for_scopes[tokens[0] + std::to_string(eip)].first = eip;
            inside_while_stack.push(eip); 
        } else if (tokens[0] == "}" && !inside_while_stack.empty()) { 
            for_scopes["while" + std::to_string(inside_while_stack.top())].second = eip;
            inside_while_stack.pop(); // Pop the stack when exiting an "if" block
        } 

        rows[eip] = tokens;
        // std::cout << eip << " ";
        //     for (int i = 0; i < rows[eip].size(); ++i) {
        //         std::cout << rows[eip][i] << " "; 
        //     }
        // std::cout << std::endl;
        ++eip;
    }

    // for (auto it = for_scopes.begin(); it != for_scopes.end(); ++it) {
    //     std::cout<< it -> first << " " << it -> second.first << " " <<  it -> second.second;
    //      std::cout << std::endl;
    // }
   
    if (!st.empty()) {
        std::cout << "There is problem with braces" << std::endl;
        return;
    }

    file.close(); // Close the file when done
}


void Interpreter::execute(int i) {
    int eip = 0; // instruction ptr
    bool main_exists = false;
    bool header_file_exists = false;
    bool do_not_enter_if_statment = false;
    bool enter_while = false;
    int while_eip_end = 0;
    int while_eip_start = 0;
    
    auto it = rows.find(i);
    while (it != rows.end()) {
        std::vector<std::string> tokens = it -> second;
        ++it;

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


        if (tokens[0] == "if" && brace_exist(tokens)) { // handling if statment
            parse_if_statement(tokens, do_not_enter_if_statment);
            if (do_not_enter_if_statment) {
                std::advance(it, for_scopes["if" + std::to_string(eip)].second - for_scopes["if" + std::to_string(eip)].first);
                //std::cout << eip << " " << "if" + std::to_string(eip) << " " << for_scopes["if" + std::to_string(eip)].first << " " <<  for_scopes["if" + std::to_string(eip)].second << " " << for_scopes["if" + std::to_string(eip)].second - for_scopes["if" + std::to_string(eip)].first  ;
                //std::cout << " ";
                eip = for_scopes["if" + std::to_string(eip)].second;
                do_not_enter_if_statment = false;
            } 
        } else if (tokens[0] == "while" && brace_exist(tokens)) { // handling if statment
            while_eip_start = for_scopes["while" + std::to_string(eip)].first;
            while_eip_end = for_scopes["while" + std::to_string(eip)].second; // while_eip_end the start of while

            enter_while = parse_while(tokens);
            if (!enter_while) {
                std::advance(it, for_scopes["while" + std::to_string(eip)].second - for_scopes["while" + std::to_string(eip)].first);
                eip = for_scopes["while" + std::to_string(eip)].second;
            } 
        } else if (tokens[0] == "char" || tokens[0] == "int" || tokens[0] == "double" || tokens[0] == "float" || tokens[0] == "bool" || tokens[0] == "string") {  // is variable declaration parsing          
            zero_token_is_type_parse(tokens); // token[0] is type // gaving flags to not take as redefinition declaration of same name variable in if statment
        } else if (is_declared_variable(tokens[0])) { // parse exprression where first token is variable
            first_token_is_variable_parse(tokens);
        } else if (tokens[0] == "std::cout" && tokens[1] == "<<" && (tokens.size() == 3 || tokens.size() == 5)) {
            cout(tokens);
        } else if ((tokens.size() == 1 && start_with_plus_plus(tokens[0]) || end_with_plus_plus(tokens[0])) || (start_with_minus_minus(tokens[0]) || end_with_minus_minus(tokens[0]))) {
            increment_decrement_parse(tokens);
        } else if (is_number(tokens[0])) {
            throw std::runtime_error("Error: Literal " + tokens[0] + " can not be an l-value");
        } 

        else if (tokens[0] != "}" && !is_declared_variable(tokens[0])) { 
            throw std::runtime_error("Error: use of undeclared identifier " + tokens[0]);
        } 
    
        // std::cout << "enter while " << enter_while << " && " << " eip " << eip << " while_eip_end " << while_eip_end << std::endl;  
        if (enter_while && eip == while_eip_end) {
            // std::cout << "enter while is true " << std::endl;
            // std::cout << while_eip_end << std::endl;
            // std::cout << for_scopes["while" + std::to_string(eip)].second - for_scopes["while" + std::to_string(while_eip_end)].first;
            int tmp = while_eip_end - while_eip_start + 1;
            std::advance(it, -tmp);
            // for (int i = 0; i < it -> second.size(); ++i) {
            //     std::cout << it -> second[i] << " "; 
            // }
            eip = while_eip_start - 1;
            // std::cout << "tmp " << tmp << " eip " << eip;
            // std::cout << std::endl;
        }  

        //std::cout << std::endl;
        ++eip;
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
        
    } else if (tokens.size() == 3 && tokens[2] == "std::endl") {
        std::cout << std::endl;
    } else {
        //std::cout << "Given variable " << tokens[2] << " is not declared" << std::endl;
    }

    if (tokens.size() == 5 && tokens[3] == "<<" && tokens[4] == "std::endl") {
        std::cout << std::endl;
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

void Interpreter::fill_var_with_given_value(const std::string& row0, const std::string& row1, const std::string& row3) {
    std::string tmp1 = row1;
    std::string tmp2 = row3;

    if (row0 == "char") {
        if (has_first_and_last_single_quotes(tmp2)) {
            remove_single_quotes(tmp2);
        }
        std::pair<std::string, char> var{row0, convert_to_type<char>(tmp2)};
        char_vars[tmp1] = var;
    } else if (row0 == "int") {
        std::pair<std::string, int> var{row0, convert_to_type<int>(tmp2)};
        integer_vars[tmp1] = var;
    } else if (row0 == "double") {
        std::pair<std::string, double> var{row0, convert_to_type<double>(tmp2)};
        double_vars[tmp1] = var;
    } else if (row0 == "float") {
        std::pair<std::string, float> var{row0, convert_to_type<float>(tmp2)};
        float_vars[tmp1] = var;
    } else if (row0 == "bool") {
        std::pair<std::string, bool> var{row0, convert_to_type<bool>(tmp2)};
        bool_vars[tmp1] = var;
    } else if (row0 == "string") {
        if (has_first_and_last_double_quotes(tmp2)) {
            remove_double_quotes(tmp2);
        }
        std::pair<std::string, std::string> var{row0, tmp2};
        string_vars[tmp1] = var;
    }   

}

void Interpreter::fill_var_with_given_variable_value(const std::string& row0, const std::string& row1, const std::string& row2) {
    std::string tmp1 = row1;
    std::string tmp2 = row2;

    if (row0 == "char") {
        if (has_first_and_last_single_quotes(tmp2)) {
            remove_single_quotes(tmp2);
        }
        std::pair<std::string, char> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<char>(type_of_var(tmp2), tmp2); 
        char_vars[tmp1] = var;
    } else if (row0 == "int") {
        std::pair<std::string, int> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<int>(type_of_var(tmp2), tmp2); 
        integer_vars[tmp1] = var;
    } else if (row0 == "double") {
        std::pair<std::string, double> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<double>(type_of_var(tmp2), tmp2); 
        double_vars[tmp1] = var;
    } else if (row0 == "float") {
        std::pair<std::string, float> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<float>(type_of_var(tmp2), tmp2); 
        float_vars[tmp1] = var;
    } else if (row0 == "bool") {
        std::pair<std::string, bool> var;
        var.first = row0;
        var.second = get_value_of_var_by_type<bool>(type_of_var(tmp2), tmp2);
        bool_vars[tmp1] = var;
    } else if (row0 == "string") {
        if (has_first_and_last_double_quotes(row2)) {
            remove_double_quotes(tmp2);
        }
        std::pair<std::string, std::string> var(type_of_var(tmp2), tmp2);
        string_vars[tmp1] = var;
    }   
}

void Interpreter::adding_cases(const std::vector<std::string>& tokens, const std::string& dest_var, std::string& str1, std::string& str2) {
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

void Interpreter::sub_cases(const std::vector<std::string>& tokens, std::string& dest_var, std::string& str1, std::string& str2) {
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

void Interpreter::mul_cases(const std::vector<std::string>& tokens, std::string& dest_var, std::string& str1, std::string& str2) {
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

void Interpreter::div_cases(const std::vector<std::string>& tokens, std::string& dest_var, std::string& str1, std::string& str2) {
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




