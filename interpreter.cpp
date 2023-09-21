#include "interpreter.h"

Interpreter::Interpreter() {
    type_names();
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void Interpreter::parse(std::ifstream& file) {
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

        // checking for ; existence at the end of code 
        if (eip > 1 && tokens[tokens.size() - 1] != ";" && !last_char_is_semicolon(tokens[tokens.size() - 1])) {
            if (tokens[tokens.size() - 1] != "}") {
                std::cout << "You forgot ; at the end of line" << std::endl;
                return;
            }
        } 

        // removing ; s from code
        for (int i = 0; i < tokens.size(); ++i) {
            if (tokens[i] == ";") {
                tokens.pop_back();

            } else if (contains_semicolon(tokens[i])) {
                remove_semicolon(tokens[i]);
            } 
        }

        // if int main() { 
        if (tokens[0] == "int" && (tokens[tokens.size() - 1] == "{" || extract_last_char(tokens[tokens.size() - 1]) == '{' )) { // if int main() {
            rows[eip] = tokens;
            ++eip;
            continue;
        } else if (tokens[0] == "char" || tokens[0] == "int" || tokens[0] == "double" || tokens[0] == "float" || tokens[0] == "bool" || tokens[0] == "string") {
            if (tokens.size() == 2) { // int x;
                rows[eip] = tokens;
                ++eip;
                continue;;
            } else if (tokens[2] == "=") { // int x = 5; 
                if (tokens[0] == "char") {
                    remove_single_quotes(tokens[3]);
                    convert_to_char(tokens[3]);
                } else if (tokens[0] == "int") {
                    convert_to_int(tokens[3]);
                } else if (tokens[0] == "double") {
                    convert_to_double(tokens[3]);
                } else if (tokens[0] == "float") {
                    convert_to_float(tokens[3]);
                } else if (tokens[0] == "bool") {
                    convert_to_bool(tokens[3]);
                } else if (tokens[0] == "string") {
                    remove_double_quotes(tokens[3]);
                } 
            } 
        } else if (is_declared_variable(tokens[0]) && tokens[1] == "=" && (is_declared_variable(tokens[2]) || is_number(tokens[2])) && (is_declared_variable(tokens[4]) || is_number(tokens[4]))) {
            // x = x + a
            // x = 10 + a
            // x = a + 5;
            //  x = 5 + 6;
            if (!(tokens[3] == "+" || tokens[3] == "-" || tokens[3] == "*" || tokens[3] == "/")) {
                std::cout << "Not valid operation" << std::endl;
            }   
        } else if (tokens.size() == 3 && is_declared_variable(tokens[0]) && tokens[1] == "=") {
            std::cout << "Aa";
            if (!is_declared_variable(tokens[2])) {
                if (type_of_var(tokens[0]) == "char") {
                    convert_to_char(tokens[2]);
                } else if (type_of_var(tokens[0]) == "int") {
                    convert_to_int(tokens[2]);
                } else if (type_of_var(tokens[0])== "double") {
                    convert_to_double(tokens[2]);
                } else if (type_of_var(tokens[0]) == "float") {
                    convert_to_float(tokens[2]);
                } else if (type_of_var(tokens[0]) == "bool") {
                    convert_to_bool(tokens[2]);
                } else if (type_of_var(tokens[0]) == "string") {
                    //
                } 
            } else {
                if (type_of_var(tokens[0]) == "char") {
                    // tokens[2] i mejiny
                    get_value_of_var_by_type<char>(tokens[2], type_of_var(tokens[2])); // po lyubomu petqa qcem tokens[0] i tipi mej
                } else if (type_of_var(tokens[0]) == "int") {
                   get_value_of_var_by_type<int>(tokens[2], type_of_var(tokens[2]));
                } else if (type_of_var(tokens[0])== "double") {
                    get_value_of_var_by_type<double>(tokens[2], type_of_var(tokens[2]));
                } else if (type_of_var(tokens[0]) == "float") {
                    get_value_of_var_by_type<float>(tokens[2], type_of_var(tokens[2]));
                } else if (type_of_var(tokens[0]) == "bool") {
                    get_value_of_var_by_type<bool>(tokens[2], type_of_var(tokens[2]));
                } else if (type_of_var(tokens[0]) == "string") {
                    //
                } 
            }
            
        } 
         
        else if (tokens[0] == "std::cout" && tokens[1] == "<<" && tokens.size() == 3) {
            //okay
        }
            
            
        rows[eip] = tokens;
        //    // std::cout << eip << " ";
        //     for (int i = 0; i < rows[eip].size(); ++i) {
        //         std::cout << rows[eip][i] << " "; 
        //     }
        //    std::cout << std::endl;
        ++eip;
    }

    if (!main_exist(rows[1])) {
        std::cout << "There is no main function to start program" << std::endl;
        return;
    }
    --eip;
    if (rows[eip][rows[eip].size() - 1] != "}") { // checking '}' existence at the end 
        std::cout << "There is no closing scope '}' at the en of main function" << std::endl;
    }

    file.close(); // Close the file when done
}


void Interpreter::execute(int i) {
    auto it = rows.find(i);
    while (it != rows.end()) {
        std::vector<std::string>& row = it -> second;

        if (row[0] == find_key_by_value(types, 0) || row[0] == find_key_by_value(types, 1) || row[0] == find_key_by_value(types, 2) || row[0] == find_key_by_value(types, 3) || row[0] == find_key_by_value(types, 4) || row[0] == find_key_by_value(types, 5)) {
            if (row.size() == 2) { // case that int x; simple declaration
                if (row[0] == "char") {
                    std::pair<std::string, char> var{row[0], 'A' + (std::rand() % 26)};
                    char_vars[row[1]] = var;
                } else if (row[0] == "int") {
                    std::pair<std::string, int> var{row[0], std::rand() % 100 + 1};
                    integer_vars[row[1]] = var;
                } else if (row[0] == "double") {
                    std::pair<std::string, double> var{row[0], var.second = static_cast<double>(std::rand()) / RAND_MAX};
                    double_vars[row[1]] = var;
                } else if (row[0] == "float") {
                    std::pair<std::string, float> var{row[0], static_cast<double>(std::rand()) / RAND_MAX};
                    float_vars[row[1]] = var;
                } else if (row[0] == "bool") {
                    std::pair<std::string, bool> var{row[0], false};
                    bool_vars[row[1]] = var;
                } else if (row[0] == "string") {
                    std::pair<std::string, std::string> var{row[0], ""};
                    string_vars[row[1]] = var;
                }
            } else if (row[2] == "=") { // case if int x = 5; like declaration
                if (row[0] == "char") {
                    std::pair<std::string, char> var{row[0], convert_to_char(row[3])};
                    char_vars[row[1]] = var;
                } else if (row[0] == "int") {
                    std::pair<std::string, int> var{row[0], convert_to_int(row[3])};
                    integer_vars[row[1]] = var;
                } else if (row[0] == "double") {
                    std::pair<std::string, double> var{row[0], convert_to_double(row[3])};
                    double_vars[row[1]] = var;
                } else if (row[0] == "float") {
                    std::pair<std::string, float> var{row[0], convert_to_float(row[3])};
                    float_vars[row[1]] = var;
                } else if (row[0] == "bool") {
                    std::pair<std::string, bool> var{row[0], convert_to_bool(row[3])};
                    bool_vars[row[1]] = var;
                } else if (row[0] == "string") {
                    std::pair<std::string, std::string> var{row[0], row[3]};
                    string_vars[row[1]] = var;
                }
            } 
        } else if (row.size() == 3 && is_declared_variable(row[0]) && row[1] == "=") {
            if (!is_declared_variable(row[2])) {
                if (type_of_var(row[0]) == "char") {
                    std::pair<std::string, char> var{row[0], convert_to_char(row[2])};
                    char_vars[row[1]] = var;
                } else if (type_of_var(row[0]) == "int") {
                    std::pair<std::string, int> var{row[0], convert_to_int(row[2])};
                    integer_vars[row[1]] = var;
                } else if (type_of_var(row[0])== "double") {
                    std::pair<std::string, double> var{row[0], convert_to_double(row[2])};
                    double_vars[row[1]] = var;
                } else if (type_of_var(row[0]) == "float") {
                    std::pair<std::string, float> var{row[0], convert_to_float(row[2])};
                    float_vars[row[1]] = var;
                } else if (type_of_var(row[0]) == "bool") {
                    std::pair<std::string, bool> var{row[0], convert_to_bool(row[2])};
                    bool_vars[row[1]] = var;
                } else if (type_of_var(row[0]) == "string") {
                    string_vars[row[1]] = {row[0], row[2]};
                } 
            } else {
                if (type_of_var(row[0]) == "char") {
                    // tokens[2] i mejiny
                    std::pair<std::string, char> var{row[0], get_value_of_var_by_type<char>(row[2], type_of_var(row[2]))}; // po lyubomu petqa qcem tokens[0] i tipi mej
                    var.first = row[0];
                    var.second = get_value_of_var_by_type<char>(row[2], type_of_var(row[2])); 
                    char_vars[row[1]] = var;
                } else if (type_of_var(row[0]) == "int") {
                    std::pair<std::string, int> var;
                    var.first = row[0];
                    var.second = get_value_of_var_by_type<int>(row[2], type_of_var(row[2]));
                    integer_vars[row[1]] = var;
                } else if (type_of_var(row[0])== "double") {
                    std::pair<std::string, double> var;
                    var.first = row[0];
                    var.second = get_value_of_var_by_type<double>(row[2], type_of_var(row[2]));
                    double_vars[row[1]] = var;
                } else if (type_of_var(row[0]) == "float") {
                    std::pair<std::string, float> var;
                    var.first = row[0];
                    var.second = get_value_of_var_by_type<float>(row[2], type_of_var(row[2]));;
                    float_vars[row[1]] = var;
                } else if (type_of_var(row[0]) == "bool") {
                    std::pair<std::string, bool> var;
                    var.first = row[0];
                    var.second = get_value_of_var_by_type<bool>(row[2], type_of_var(row[2]));
                    bool_vars[row[1]] = var;
                } else if (type_of_var(row[0]) == "string") {
                    std::pair<std::string, std::string> var;
                    var.first = row[0];
                    var.second = get_value_of_var_by_type<bool>(row[2], type_of_var(row[2]));
                    string_vars[row[1]] = var;
                } 
            }
        }  
        
        else if (row.size() == 5 && is_declared_variable(row[0]) && row[1] == "=") { 
            if (row[3] == "+") {
                std::string tmp = row[0];
                check_vars(row[2], type_of_var(row[2]), row[4], type_of_var(row[4])); // checking types of op1 and op2 before some operation to make precise castings
                if (type_of_var(row[0]) == "char") {
                    add<char>(row[0], row[2], row[4]);
                    char_vars[tmp].second = convert_to_char(row[0]);
                } else if (type_of_var(row[0]) == "int") {
                    add<int>(row[0], row[2], row[4]);
                    integer_vars[tmp].second = convert_to_int(row[0]);
                } else if (type_of_var(row[0]) == "double") {
                    add<double>(row[0], row[2], row[4]);
                    double_vars[tmp].second  = convert_to_double (row[0]);
                } else if (type_of_var(row[0])  == "float") {
                    add<float>(row[0], row[2], row[4]);
                    float_vars[tmp].second = convert_to_float(row[0]);
                } else if (type_of_var(row[0]) == "bool") {
                    add<bool>(row[0], row[2], row[4]);
                    bool_vars[tmp].second = convert_to_bool(row[0]);
                } else if (type_of_var(row[0]) == "string") {
                    add<std::string>(row[0], row[2], row[4]);
                    string_vars[tmp].second = row[0];
                }     
            } else if (row[3] == "-") {
                std::string tmp = row[0];
                std::string type1 = type_of_var(row[2]);
                std::string type2 = type_of_var(row[4]);
                check_vars(row[2], type_of_var(row[2]), row[4], type_of_var(row[4])); // checking types of op1 and op2 before some operation to make precise castings
                if (type_of_var(row[0]) == "char") {
                    sub<char>(row[0], row[2], row[4]);
                    char_vars[tmp].second = convert_to_char(row[0]);
                } else if (type_of_var(row[0]) == "int") {
                    std::cout << "int " << row[0]; ;
                    sub<int>(row[0], row[2], row[4]);
                    integer_vars[tmp].second = convert_to_int(row[0]);
                } else if (type_of_var(row[0]) == "double") {
                    sub<double>(row[0], row[2], row[4]);
                    double_vars[tmp].second  = convert_to_double (row[0]);
                } else if (type_of_var(row[0])  == "float") {
                    sub<float>(row[0], row[2], row[4]);
                    float_vars[tmp].second = convert_to_float(row[0]);
                } else if (type_of_var(row[0]) == "bool") {
                    throw std::runtime_error("Subtraction not supported for the bool.");
                } else if (type_of_var(row[0]) == "string") {
                    throw std::runtime_error("Subtraction not supported for the string.");
                } 

            } else if (row[3] == "*") {

            } else if (row[3] == "/") {
                    
            }

               
        } else if (row[0] == "std::cout" && row[1] == "<<" && row.size() == 3) {
            if (is_number(row[2])) {
                std::cout << row[2];
            } else if (is_declared_variable(row[2])) {
                if (type_of_var(row[2]) == "char") {
                    std::cout << char_vars[row[2]].second;
                } else if (type_of_var(row[2]) == "int") {
                    std::cout << integer_vars[row[2]].second;
                } else if (type_of_var(row[2]) == "double") {
                    std::cout << double_vars[row[2]].second;
                } 
                else if (type_of_var(row[2]) == "float") {
                    std::cout << float_vars[row[2]].second;
                } 
                else if (type_of_var(row[2]) == "bool") {
                    std::cout << bool_vars[row[2]].second;
                } else if (type_of_var(row[2]) == "string") {
                    std::cout << string_vars[row[2]].second;
                }
            }
        }

        // for (int i = 0; i < row.size(); ++i) {
        //     std::cout << row[i];
        // }
        // std::cout << std::endl;
        ++it;
    }
}

//type , op1, op2
void Interpreter::check_vars(std::string& str1, const std::string& type_str1, std::string& str2, const std::string& type_str2) {
    if (is_declared_variable(str1)) { 
        if (is_declared_variable(str2)) { // case if op1 and op2 are already declared variables
            if (type_of_var(str1) == "bool" || type_of_var(str2) == "bool") { 
                throw std::invalid_argument("The types are not muching for operation");
            } else if (type_of_var(str1) == "string" && type_of_var(str2) != "string") {
                throw std::invalid_argument("The types are not muching for operation");
            } else if (type_of_var(str1) != "string" && type_of_var(str2) == "string") {
                throw std::invalid_argument("The types are not muching for operation");
            } else if (type_of_var(str1) != "char" && type_of_var(str2) == "char") {
                throw std::invalid_argument("The types are not muching for operation");
            } else if (type_of_var(str1) == "char" && type_of_var(str2) == "char") {
                throw std::invalid_argument("The types are not muching for operation");
            }
        }

        if (type_str1 == "char") {
            std::pair<std::string, char> var{type_str1, char_vars[str1].second};
            str1 = std::to_string(var.second);
        } else if (type_str1 == "int") {
            std::pair<std::string, int> var{type_str1, integer_vars[str1].second};
            str1 = std::to_string(var.second);
        } else if (type_str1 == "double") {
            std::pair<std::string, double> var{type_str1, double_vars[str1].second};
            str1 = std::to_string(var.second);
        } else if (type_str1 == "float") {
            std::pair<std::string, float> var{type_str1, float_vars[str1].second};
            str1 = std::to_string(var.second);
        } else if (type_str1 == "bool") {
            std::pair<std::string, bool> var{type_str1, bool_vars[str1].second};
            str1 = std::to_string(var.second);
        } else if (type_str1 == "string") {
            std::pair<std::string, std::string> var{type_str1, string_vars[str1].second};
            str1 = var.second;
        }
    } else { 
        if (is_declared_variable(str2)) { // case that first op is literal second declared var
            if (type_of_var(str2) == "string" && has_first_and_last_double_quotes(str1)) {
                remove_double_quotes(str1);
            } else if (type_of_var(str2) == "char" && has_first_and_last_single_quotes(str1)) {
                remove_single_quotes(str1);
            } else if (!(type_of_var(str2) != "string" && type_of_var(str2) != "char" && type_of_var(str2) != "bool" && !has_first_and_last_single_quotes(str1) && !has_first_and_last_double_quotes(str1))) {
                throw std::invalid_argument("The types are not muching for operation"); 
            } 
        } else { // if both operand1 and operand2 are literals
            if (!((is_number(str1) && is_number(str2)) || (has_first_and_last_single_quotes(str1) && has_first_and_last_single_quotes(str2)) || (has_first_and_last_double_quotes(str1) && has_first_and_last_double_quotes(str2)))) {
                throw std::invalid_argument("The types are not muching for operation"); // 
            }  
        }
    } 

    if (is_declared_variable(str2)) {
        if (type_str2 == "char") {
            std::pair<std::string, char> var{type_str2, char_vars[str2].second};
            str2 = std::to_string(var.second);
        } else if (type_str2 == "int") {
            std::pair<std::string, int> var{type_str2, integer_vars[str2].second};
            str2 = std::to_string(var.second);
        } else if (type_str2 == "double") {
            std::pair<std::string, double> var{type_str2, double_vars[str2].second};
            str2 = std::to_string(var.second);
        } else if (type_str2 == "float") {
            std::pair<std::string, float> var{type_str2, float_vars[str2].second};;
            str2 = std::to_string(var.second);
        } else if (type_str2 == "bool") {
            std::pair<std::string, bool> var{type_str2, bool_vars[str2].second};
            str2 = std::to_string(var.second);
        } else if (type_str2 == "string") {
            std::pair<std::string, std::string> var{type_str2, string_vars[str2].second};
            str2 = var.second;
        }    
    } else {
        if (is_declared_variable(str1)) { // case that op1 is declared, op2 is literal
            if (type_of_var(str1) == "string" && has_first_and_last_double_quotes(str2)) {
                remove_double_quotes(str1);
            } else if (type_of_var(str1) == "char" && has_first_and_last_single_quotes(str2)) {
                remove_single_quotes(str1);
            } else if (!(type_of_var(str1) != "string" && type_of_var(str1) != "char" && type_of_var(str1) != "bool" && !has_first_and_last_single_quotes(str2) && !has_first_and_last_double_quotes(str2))) {
                throw std::invalid_argument("The types are not muching for operation"); 
            } 
        } 
    }
}

template <typename T>
void Interpreter::add(std::string &res, std::string& str1, const std::string& str2) {
    if ((type_of_var(str1) == "string" && type_of_var(str2) == "string") || (type_of_var(str1) == "char" && type_of_var(str2) == "char")) {
        res = str1 + str2;
        return;
    }
    // Convert the input strings to numerical values
    T num1, num2;
    std::istringstream(str1) >> num1;
    std::istringstream(str2) >> num2;

    // Perform the addition
    T result = num1 + num2;

    // Convert the result back to a string
    std::ostringstream oss;
    oss << result;
    res = oss.str();
}

template <typename T>
T Interpreter::convert_to_type(const std::string& str) {
    std::istringstream ss(str);
    T result;
    ss >> result;
    return result;
}

template <typename T>
void Interpreter::sub(std::string& res, const std::string& op1, const std::string& op2) {
    T operand1 = convert_to_type<T>(op1);
    T operand2 = convert_to_type<T>(op2);
    T result = operand1 - operand2;
    std::ostringstream ss;
    ss << result;
    res = ss.str();
}

void Interpreter::type_names() {
    types["char"] = 0;
    types["int"] = 1;
    types["double"] = 2;
    types["float"] = 3;
    types["bool"] = 4;
    types["string"] = 5;
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

void Interpreter::remove_semicolon(std::string& input) {
    input = input.substr(0, input.size() - 1); // extract the substring without the semicolon
}

bool Interpreter::contains_semicolon(const std::string& str) {
    return str.find(';') != std::string::npos;
}

bool Interpreter::is_semicolon(const std::string& input) {
    return input == ";";
}

std::string Interpreter::find_key_by_value(std::map<std::string, int>& my_map, int target_value) {
    for (auto& pair : my_map) {
        if (pair.second == target_value) {
            return pair.first; // return the key when the value matches
        }
    }
    
    return ""; // Return an empty string if the value is not found
}

bool Interpreter::is_single_char(const std::string& str) {
    return str.length() == 1;
}

bool Interpreter::last_char_is_semicolon(std::string& str) {
    if (str[str.size() - 1] == ';') {
        return true;
    }
    return false;
}

char Interpreter::convert_to_char(const std::string& str) {
    if (is_single_char(str)) {
        return str[0]; // Extract and return the first character
    } else {
        throw std::invalid_argument("Input is not a single character.");
    }
}

int Interpreter::convert_to_int(const std::string& str) {
    try {
        return std::stoi(str); // Attempt to convert to int
    } catch (const std::invalid_argument&) {
        throw std::invalid_argument("Input is not integer.");
    } catch (const std::out_of_range&) {
        throw std::invalid_argument("Input is not a integer.");
    }
}

double Interpreter::convert_to_double(const std::string& str) {
    try {
        return std::stod(str); // Attempt to convert to double
    } catch (const std::invalid_argument&) {
        throw std::invalid_argument("Input is not a double.");
    } catch (const std::out_of_range&) {
        throw std::invalid_argument("Input is not a double.");
    }
}

float Interpreter::convert_to_float(const std::string& str) {
    try {
        return std::stof(str); // Attempt to convert to double
    } catch (const std::invalid_argument&) {
        throw std::invalid_argument("Input is not float.");
    } catch (const std::out_of_range&) {
        throw std::invalid_argument("Input is not a float.");
    }
}

bool Interpreter::convert_to_bool(const std::string& str) {
    if (str == "true") {
        return true;
    } else if (str == "false") {
        return false;
    } else {
        throw std::invalid_argument("Invalid argument: Cannot convert to bool.");
    }
}

char Interpreter::extract_last_char(const std::string& str) {
    return str[str.size() - 1];
}

// cheking different cases of int main() {
bool Interpreter::main_exist(std::vector<std::string>& tokens) {
    bool main_exists = false;
    if (tokens.size() == 2 && tokens[0] == "int" && tokens[1] == "main(){" ) {
        main_exists = true; // int main(){
    } else if (tokens.size() == 3 && tokens[0] == "int" && tokens[1] == "main(" && tokens[2] == "){" ) {
        main_exists = true; // int main(  ){
    }  else if (tokens.size() == 3 && tokens[0] == "int" && tokens[1] == "main" && tokens[2] == "(){" ) {
        main_exists = true; // int main (){
    }  else if (tokens.size() == 3 && tokens[0] == "int" && tokens[1] == "main()" && tokens[2] == "{" ) {
        main_exists = true; // int main() {
    }  else if (tokens.size() == 4 && tokens[0] == "int" && tokens[1] == "main" && tokens[2] == "("  && tokens[3] == "){") {
        main_exists = true; // int main (  ){
    } else if (tokens.size() == 4 && tokens[0] == "int" && tokens[1] == "main" && tokens[2] == "()"  && tokens[3] == "{") {
        main_exists = true; // int main () {
    } else if (tokens.size() == 4 && tokens[0] == "int" && tokens[1] == "main(" && tokens[2] == ")"  && tokens[3] == "{") {
        main_exists = true; // int main(  ) {
    } else if (tokens.size() == 5 && tokens[0] == "int" && tokens[1] == "main" && tokens[2] == "("  && tokens[3] == ")" && tokens[4] == "{") {
        main_exists = true; // int main (  ) {
    } else {
        throw std::invalid_argument("Syntax     error");
    }
            
    if (!main_exists) {
        return false;
    }  

    return true; 
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

std::string Interpreter::type_of_var(const std::string& token) {
    std::string type = "";
    for (auto it = char_vars.begin(); it != char_vars.end(); ++it) {
        if (it -> first == token) {
            type = "char";
        }
    }

    for (auto it = integer_vars.begin(); it != integer_vars.end(); ++it) {
        if (it -> first == token) {
            type = "int";
            return type;
        }
    }

    for (auto it = float_vars.begin(); it != float_vars.end(); ++it) {
        if (it -> first == token) {
            type = "float";
            return type;
        }
    }

    for (auto it = double_vars.begin(); it != double_vars.end(); ++it) {
        if (it -> first == token) {
            type = "double";
            return type;
        }
    }

    for (auto it = bool_vars.begin(); it != bool_vars.end(); ++it) {
        if (it -> first == token) {
            type = "bool";
            return type;
        }
    }

    for (auto it = string_vars.begin(); it != string_vars.end(); ++it) {
        if (it -> first == token) {
            type = "string";
            return type;
        }
    }
}

void Interpreter::remove_double_quotes(std::string& input) {
    if (input.front() == '"' && input.back() == '"') {
        input = input.substr(1, input.length() - 2);
    } else {
        throw std::invalid_argument("There is no chakert ");
    }
}

void Interpreter::remove_single_quotes(std::string& input) {
    if (!input.empty() && input.front() == '\'') {
        input = input.substr(1);
    }

    if (!input.empty() && input.back() == '\'') {
        input.pop_back();
    } else {
        throw std::invalid_argument("There is no chakert ");
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
    throw std::invalid_argument("There is no oppurtunity to give such value to variable");
}

int Interpreter::get_value_of_int(const std::string& token) {
    for (auto it = integer_vars.begin(); it != integer_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    throw std::invalid_argument("There is no oppurtunity to give such value to variable");
}

double Interpreter::get_value_of_double(const std::string& token) {
    for (auto it = double_vars.begin(); it != double_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    throw std::invalid_argument("There is no oppurtunity to give such value to variable");
}

float Interpreter::get_value_of_float(const std::string& token) {
    for (auto it = float_vars.begin(); it != float_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    throw std::invalid_argument("There is no oppurtunity to give such value to variable");
}

bool Interpreter::get_value_of_bool(const std::string& token) {
    for (auto it = bool_vars.begin(); it != bool_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    throw std::invalid_argument("There is no oppurtunity to give such value to variable");
}

std::string Interpreter::get_value_of_string(const std::string& token) {
    for (auto it = string_vars.begin(); it != string_vars.end(); ++it) {
        if (it -> first == token) {
            return it -> second.second;
        }
    }
    throw std::invalid_argument("There is no oppurtunity to give such value to variable");
}

