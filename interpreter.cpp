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
            if (contains_semicolon(tokens[i])) {
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
                    string_vars[tokens[1]] = tokens[3];
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
        } else if (tokens[0] == "std::cout" && tokens[1] == "<<" && tokens.size() == 3) {
                
        }
            
            
        rows[eip] = tokens;
        //     std::cout << eip << " ";
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
                    char_vars[row[1]] = 'A' + (std::rand() % 26); 
                } else if (row[0] == "int") {
                    integer_vars[row[1]] = std::rand() % 100 + 1;
                } else if (row[0] == "double") {
                    double_vars[row[1]] = static_cast<double>(std::rand()) / RAND_MAX;
                } else if (row[0] == "float") {
                    float_vars[row[1]] = static_cast<float>(std::rand()) / RAND_MAX;
                } else if (row[0] == "bool") {
                    bool_vars[row[1]] = false;
                } else if (row[0] == "string") {
                    string_vars[row[1]] = "";
                }
            } else if (row[2] == "=") { // case if int x = 5; like declaration
                if (row[0] == "char") {
                    char_vars[row[1]] = convert_to_char(row[3]);
                } else if (row[0] == "int") {
                    integer_vars[row[1]] = convert_to_int(row[3]);
                } else if (row[0] == "double") {
                    double_vars[row[1]] = convert_to_double(row[3]);
                } else if (row[0] == "float") {
                    float_vars[row[1]] = convert_to_float(row[3]);
                    //std::cout << float_vars[row[1]]<< "::";
                } else if (row[0] == "bool") {
                    bool_vars[row[1]] = convert_to_bool(row[3]);
                } else if (row[0] == "string") {
                    string_vars[row[1]] = row[3];
                }
            } 
        } else if (is_declared_variable(row[0]) && row[1] == "=") { 
            if (row[3] == "+") {
                    
            } else if (row[3] == "-") {

            } else if (row[3] == "*") {

            } else if (row[3] == "/") {
                    
            }

               
        } else if (row[0] == "std::cout" && row[1] == "<<" && row.size() == 3) {
            if (is_number(row[2])) {
                std::cout << row[2];
            } else if (is_declared_variable(row[2])) {
                if (type_of_var(row[2]) == "char") {
                    std::cout << char_vars[row[2]];
                } else if (type_of_var(row[2]) == "int") {
                    std::cout << integer_vars[row[2]];
                } else if (type_of_var(row[2]) == "double") {
                    std::cout << double_vars[row[2]];
                } else if (type_of_var(row[2]) == "float") {
                    std::cout << float_vars[row[2]];
                } else if (type_of_var(row[2]) == "bool") {
                    std::cout << bool_vars[row[2]];
                } else if (type_of_var(row[2]) == "string") {
                    std::cout << string_vars[row[2]];
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
        if (type_str1 == "char") {
            str1 = std::to_string(char_vars[str1]);
        } else if (type_str1 == "int") {
            str1 = std::to_string(integer_vars[str1]);
        } else if (type_str1 == "double") {
            str1 = std::to_string(double_vars[str1]);
        } else if (type_str1 == "float") {
            str1 = std::to_string(float_vars[str1]);
        } else if (type_str1 == "bool") {
            str1 = std::to_string(bool_vars[str1]);
        } else if (type_str1 == "string") {
            str1 = string_vars[str1];
        }
    }

    if (is_declared_variable(str2)) {
        if (type_str2 == "char") {
            str2 = std::to_string(char_vars[str2]);
        } else if (type_str2 == "int") {
            str2 = std::to_string(integer_vars[str2]);
        } else if (type_str2 == "double") {
            str2 = std::to_string(double_vars[str2]);
        } else if (type_str2 == "float") {
            str2 = std::to_string(float_vars[str2]);
        } else if (type_str2 == "bool") {
            str2 = std::to_string(bool_vars[str2]);
        } else if (type_str2 == "string") {
            str2 = string_vars[str2];
        }    
    }
}

template <typename T>
void Interpreter::add(std::string &res, std::string &str1, const std::string &str2) {
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



void Interpreter::type_names() {
    types["char"] = 0;
    types["int"] = 1;
    types["double"] = 2;
    types["float"] = 3;
    types["bool"] = 4;
    types["string"] = 5;
}


bool Interpreter::is_number(const std::string& str) {
    // Check each character in the string
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false; // if any character is not a digit, it's not a valid number
        }
    }

    return true; // all characters are digits, so it's a valid number
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

bool Interpreter::is_declared_variable(const std::string& token) {
    for (auto it = char_vars.begin(); it != char_vars.end(); ++it) {
        if (it -> first == token) {
            return true;
        }
    }

    for (auto it = integer_vars.begin(); it != integer_vars.end(); ++it) {
        if (it -> first == token) {  
            return true;
        }
    }

    for (auto it = float_vars.begin(); it != float_vars.end(); ++it) {
        if (it -> first == token) {
            return true;
        }
    }

    for (auto it = double_vars.begin(); it != double_vars.end(); ++it) {
        if (it -> first == token) {
            return true;
        }
    }

    for (auto it = bool_vars.begin(); it != bool_vars.end(); ++it) {
        if (it -> first == token) {
            return true;
        }
    }

    for (auto it = string_vars.begin(); it != string_vars.end(); ++it) {
        if (it -> first == token) {
            return true;
        }
    }
    return false;
}

std::string Interpreter::type_of_var(const std::string& token) {
    std::string type = "";
    for (auto it = char_vars.begin(); it != char_vars.end(); ++it) {
        if (it -> first == token) {
            type = "char";
            return type;
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
    return type;
}
