#include "interpreter.h"

Interpreter::Interpreter() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

// converting to char
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

// converting to int
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

// converting to double
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

// converting to float
template <>
float Interpreter::convert_to_type(std::string& str) {
    if (str == "false") {
        return 0.0f;
    } else if (str == "true") {
        return 1.0f;
    }
    return std::stof(str);
}

// converting to bool
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

// converting to string
template <>
std::string Interpreter::convert_to_type<std::string>(std::string& str) {
    return str;  // Just return the input string for std::string
}

// parsing 
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
        
        bool inside_quotes = false;
        std::string current_token;

        while (iss >> token) {
            // as we reading from sstream when we saw string with space as "hello world" we read it space by space but we have to read it as one sting so here this case is handled
            if (token.front() == '"' && (token.back() == '"' || token[token.size() - 2] == '"')) {
                // entire quoted string is a single token, including the quotes.
                tokens.push_back(token);
            } else if (token.front() == '"') {
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

        // giving different names to vars inside if with same var name as outside
        if (!inside_if_stack.empty()) {
            if (tokens[0] == "char" || tokens[0] == "int" || tokens[0] == "double" || tokens[0] == "float" || tokens[0] == "bool" || tokens[0] == "string") {
                tokens[1] += "if";
                tokens[1] += std::to_string(inside_if_stack.top());
                ifs.push_back(tokens[1]);
            } else if (find_string_in_vector(ifs, tokens[0] + "if" + std::to_string(inside_if_stack.top()))) {
                tokens[0] += "if";
                tokens[0] += std::to_string(inside_if_stack.top());
            } else if ((tokens[0] == "std::cout" || tokens[0] == "std::cin") && find_string_in_vector(ifs, tokens[2] + "if" + std::to_string(inside_if_stack.top()))) {
                tokens[2] += "if";
                tokens[2] += std::to_string(inside_if_stack.top());
            }  else if (tokens[0] == "if") {
                std::string tmp = extract_paren(tokens[1], '(');
                if (find_string_in_vector(ifs, tmp + "if" + std::to_string(inside_if_stack.top()))) {
                    tokens[1] += "if";
                    tokens[1] += std::to_string(inside_if_stack.top()); 
                }
            }
        } 

         // giving different names to vars inside while with same var name as outside
        if (!inside_while_stack.empty()) {
            if (tokens[0] == "char" || tokens[0] == "int" || tokens[0] == "double" || tokens[0] == "float" || tokens[0] == "bool" || tokens[0] == "string") {
                tokens[1] += "while";
                tokens[1] += std::to_string(inside_while_stack.top());
                whiles.push_back(tokens[1]);
            } else if (find_string_in_vector(whiles, tokens[0] + "while" + std::to_string(inside_while_stack.top()))) {
                tokens[0] += "while";
                tokens[0] += std::to_string(inside_while_stack.top());
            } else if ((tokens[0] == "std::cout" || tokens[0] == "std::cin") && find_string_in_vector(whiles, tokens[2] + "while" + std::to_string(inside_while_stack.top()))) {
                tokens[2] += "while";
                tokens[2] += std::to_string(inside_while_stack.top());
            }  else if (tokens[0] == "while") {
                std::string tmp = extract_paren(tokens[1], '(');
                if (find_string_in_vector(whiles, tmp + "while" + std::to_string(inside_while_stack.top()))) {
                    tokens[1] += "while";
                    tokens[1] += std::to_string(inside_while_stack.top()); 
                }
            }
        } 
        

        if (tokens[0] == "if" && brace_exist(tokens)) { // handling if statment  
            for_scopes[tokens[0] + std::to_string(eip)].first = eip;
            inside_if_stack.push(eip); 
        } else if (tokens[0] == "}" && !inside_if_stack.empty()) { 
            for_scopes["if" + std::to_string(inside_if_stack.top())].second = eip;
            inside_if_stack.pop(); // Pop the stack when exiting an "if" block
        } else if (tokens[0] == "while" && brace_exist(tokens)) { // handling if statment 
            for_scopes[tokens[0] + std::to_string(eip)].first = eip;
            inside_while_stack.push(eip); 
        } else if (tokens[0] == "}" && !inside_while_stack.empty()) { 
            for_scopes["while" + std::to_string(inside_while_stack.top())].second = eip;
            inside_while_stack.pop(); // Pop the stack when exiting an "if" block
        } 

        rows[eip] = tokens;
        ++eip;
    }
   
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
    bool enter_if_statment = false;
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
            enter_if_statment =  parse_if_statement(tokens);
            if (!enter_if_statment) {
                std::advance(it, for_scopes["if" + std::to_string(eip)].second - for_scopes["if" + std::to_string(eip)].first);
                eip = for_scopes["if" + std::to_string(eip)].second;
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
        } else if (is_declared_variable(tokens[0]) || is_declared_array(separate_name_and_size_in_array_declaration(tokens[0]).first)) { // parse exprression where first token is variable
            first_token_is_variable_parse(tokens);
        } else if (tokens[0] == "std::cout" && tokens[1] == "<<" && (tokens.size() == 3 || tokens.size() == 5)) {
            cout(tokens);
        } else if (tokens[0] == "std::cin" && tokens[1] == ">>" && tokens.size() == 3) {
            cin(tokens);
        } else if ((tokens.size() == 1 && start_with_plus_plus(tokens[0]) || end_with_plus_plus(tokens[0])) || (start_with_minus_minus(tokens[0]) || end_with_minus_minus(tokens[0]))) {
            increment_decrement_parse(tokens);
        } else if (enter_if_statment && !enter_while && tokens[0] == "continue") { // handling case that continue is not in loop statement
            throw std::runtime_error("Error: 'continue' statement is not in loop statement");
        } else  if (enter_if_statment && !enter_while && tokens[0] == "break") { // handling case that break is not in loop statement
            throw std::runtime_error("Error: 'break' statement is not in loop statement");
        } else if (tokens[0] == "break" && enter_if_statment && enter_while) { // handling case that continue is in loop statement
            std::advance(it, while_eip_end - eip);
            eip = while_eip_end;
            enter_while = false;
        } else if (tokens[0] == "continue" && enter_if_statment && enter_while) { // handling case that continue is in loop statement
            int tmp = eip - while_eip_start + 1;
            std::advance(it, -tmp);
            eip = while_eip_start - 1;
        } else if (is_number(tokens[0])) {
            throw std::runtime_error("Error: Literal " + tokens[0] + " can not be an l-value");
        } else if (tokens[0] != "}" && !is_declared_variable(tokens[0])) { 
            throw std::runtime_error("Error: use of undeclared identifier " + tokens[0]);
        } 

        if (enter_while && eip == while_eip_end) {
            int tmp = while_eip_end - while_eip_start + 1;
            std::advance(it, -tmp);
            eip = while_eip_start - 1;
        }

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

// checking if token is declared variable name
template<typename T>
bool Interpreter::is_declared_variable(const std::string& token, const std::map<std::string, T>& vars) {
    return vars.find(token) != vars.end();
}

// checking if token is declared variable name
bool Interpreter::is_declared_variable(const std::string& token) {
    return is_declared_variable(token, char_vars) ||
           is_declared_variable(token, integer_vars) ||
           is_declared_variable(token, float_vars) ||
           is_declared_variable(token, double_vars) ||
           is_declared_variable(token, bool_vars) ||
           is_declared_variable(token, string_vars);
}

// checking if token is declared array name
template<typename T>
bool Interpreter::is_declared_array(const std::string& token, const std::map<std::string, std::vector<T>>& arr) {
    return arr.find(token) != arr.end();
}

// checking if token is declared array name
bool Interpreter::is_declared_array(const std::string& token) {
    return is_declared_variable(token, char_arr) ||
           is_declared_variable(token, int_arr) ||
           is_declared_variable(token, float_arr) ||
           is_declared_variable(token, double_arr) ||
           is_declared_variable(token, bool_arr);
}

// filling variable with garbage
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

// filling variable with given literal
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

// filling variable with given variable value
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

// adding cases
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

// subtraction cases
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

// multiplication cases
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

// division cases
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

// getting value of variable
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
    } else if (type == "string") {} 
	return 0;
}

// function for adding
template <typename T>
void Interpreter::add(std::string& res, std::string& str1, std::string& str2) {
    T operand1 = convert_to_type<T>(str1);
    T operand2 = convert_to_type<T>(str2);
    T result = operand1 + operand2;
    std::ostringstream ss;
    ss << result;
    res = ss.str();
}

// function for subtraction
template <typename T>
void Interpreter::sub(std::string& res, std::string& op1, std::string& op2) {
    T operand1 = convert_to_type<T>(op1);
    T operand2 = convert_to_type<T>(op2);
    T result = operand1 - operand2;
    std::ostringstream ss;
    ss << result;
    res = ss.str();
}

// function for multiplication
template <typename T>
void Interpreter::multiply(std::string& res, std::string& op1, std::string& op2) {
    T operand1 = convert_to_type<T>(op1);
    T operand2 = convert_to_type<T>(op2);
    T result = operand1 * operand2; 
    std::ostringstream ss;
    ss << result;
    res = ss.str();
}

// function for division
template <typename T>
void Interpreter::divide(std::string& res, std::string& op1, std::string& op2) {
    T operand1 = convert_to_type<T>(op1);
    T operand2 = convert_to_type<T>(op2);
    T result = operand1 / operand2; 
    std::ostringstream ss;
    ss << result;
    res = ss.str();
}




