#include "interpreter.h"

//type , op1, op2
std::pair<std::string, std::string> Interpreter::check_variables_inside(const std::string& str1, const std::string& str2) {
    std::string tmp1 = str1;
    std::string tmp2 = str2;
    if (is_declared_variable(str1) && is_declared_variable(str2)) { // case if op1 and op2 are already declared variables
        
        if (type_of_var(str1) == "string" && type_of_var(str2) != "string") { 
            throw std::invalid_argument("The types are not muching for operation");
        } else if (type_of_var(str1) != "string" && type_of_var(str2) == "string") {
            throw std::invalid_argument("The types are not muching for operation");
        }   

        get_var_value_inside(tmp1); //getting value inside var
        get_var_value_inside(tmp2); //getting value inside var
        
    } else if (is_declared_array(separate_name_and_size_in_array_declaration(str1).first) && is_declared_array(separate_name_and_size_in_array_declaration(str2).first)) {
        tmp1 = get_value_inside_array_index(str1);
        tmp2 = get_value_inside_array_index(str2);
    } else if (is_declared_array(separate_name_and_size_in_array_declaration(str1).first) && !is_declared_array(separate_name_and_size_in_array_declaration(str2).first)) {
        tmp1 = get_value_inside_array_index(str1);

        if (!is_number(tmp2) && !has_first_and_last_double_quotes(tmp2) && !has_first_and_last_single_quotes(tmp2) && tmp2 != "false" && tmp2 != "true") {
            throw std::runtime_error("Error: Variable " + tmp2 + " is not declared");
        }
        
        // if one operand is string the other not it is not appropirate
        if (has_first_and_last_double_quotes(str2) && type_of_var(str1) != "string") {
            throw std::invalid_argument("The types are not muching for operation"); // 
        } 

        if (type_of_var(str1) == "string" && has_first_and_last_double_quotes(str2)) {
            remove_double_quotes(tmp2);
        } else if (has_first_and_last_single_quotes(str2)) {
            remove_single_quotes(tmp2);
        } 
    } else if (!is_declared_array(separate_name_and_size_in_array_declaration(str1).first) && is_declared_array(separate_name_and_size_in_array_declaration(str2).first)) {
        if (!is_number(tmp1) && !has_first_and_last_double_quotes(tmp1) && !has_first_and_last_single_quotes(tmp1) && tmp1 != "false" && tmp1 != "true") {
            throw std::runtime_error("Error: Variable " + tmp1 + " is not declared");
        }
        // if one operand is string the other not it is not appropirate
        if (has_first_and_last_double_quotes(str1) && type_of_var(str2) != "string") {
            throw std::invalid_argument("The types are not muching for operation"); // 
        } 

        if (type_of_var(str2) == "string" && has_first_and_last_double_quotes(str1)) {
            remove_double_quotes(tmp1);
        } else if (has_first_and_last_single_quotes(str1)) {
            remove_single_quotes(tmp1);
        } 

        tmp2 = get_value_inside_array_index(str2);
    } else if (is_declared_variable(str1) && !is_declared_variable(str2)) { // case that first op is declared var second is literal
        if (!is_number(tmp2) && !has_first_and_last_double_quotes(tmp2) && !has_first_and_last_single_quotes(tmp2) && tmp2 != "false" && tmp2 != "true") {
            throw std::runtime_error("Error: Variable " + tmp2 + " is not declared");
        }
        
        // if one operand is string the other not it is not appropirate
        if (has_first_and_last_double_quotes(str2) && type_of_var(str1) != "string") {
            throw std::invalid_argument("The types are not muching for operation"); // 
        } 

        if (type_of_var(str1) == "string" && has_first_and_last_double_quotes(str2)) {
            remove_double_quotes(tmp2);
        } else if (has_first_and_last_single_quotes(str2)) {
            remove_single_quotes(tmp2);
        } 

        get_var_value_inside(tmp1); //getting value inside var
        
    } else if (!is_declared_variable(str1) && is_declared_variable(str2)) { // case that first op is literal second declared var
        if (!is_number(tmp1) && !has_first_and_last_double_quotes(tmp1) && !has_first_and_last_single_quotes(tmp1) && tmp1 != "false" && tmp1 != "true") {
            throw std::runtime_error("Error: Variable " + tmp1 + " is not declared");
        }
        // if one operand is string the other not it is not appropirate
        if (has_first_and_last_double_quotes(str1) && type_of_var(str2) != "string") {
            throw std::invalid_argument("The types are not muching for operation"); // 
        } 

        if (type_of_var(str2) == "string" && has_first_and_last_double_quotes(str1)) {
            remove_double_quotes(tmp1);
        } else if (has_first_and_last_single_quotes(str1)) {
            remove_single_quotes(tmp1);
        } 

        get_var_value_inside(tmp2); //getting value inside var

    } else if (!is_declared_variable(str1) && !is_declared_variable(str2)) { // if both operand1 and operand2 are literals
        if (!is_number(tmp1) && !has_first_and_last_double_quotes(tmp1) && !has_first_and_last_single_quotes(tmp1) && tmp1 != "false" && tmp1 != "true") {
            throw std::runtime_error("Error: Variable " + tmp1 + " is not declared");
        } else if (!is_number(tmp2) && !has_first_and_last_double_quotes(tmp2) && !has_first_and_last_single_quotes(tmp2) && tmp2 != "false" && tmp2 != "true") {
            throw std::runtime_error("Error: Variable " + tmp2 + " is not declared");
        }

        // if one operand is string the other not it is not appropirate
        if (has_first_and_last_double_quotes(str1) && !has_first_and_last_double_quotes(str2)) {
            throw std::invalid_argument("The types are not muching for operation"); // 
        } else if (!has_first_and_last_double_quotes(str1) && has_first_and_last_double_quotes(str2)) {
            throw std::invalid_argument("The types are not muching for operation"); // 
        }  
        
        if (has_first_and_last_double_quotes(str1) && has_first_and_last_double_quotes(str2)) {
            remove_double_quotes(tmp1);
            remove_double_quotes(tmp2);
        } else if (has_first_and_last_single_quotes(str1) && has_first_and_last_single_quotes(str2)) {
            remove_single_quotes(tmp1);
            remove_single_quotes(tmp2);
        } 
    }

    return {tmp1, tmp2};
}

// str i declared variable, like str = 5; we are making str as 5
void Interpreter::get_var_value_inside(std::string& str) {
    if (type_of_var(str) == "char") {
        str = std::to_string(char_vars[str].second);
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
    } else {
        std::cout << "Syntax error " << std::endl;
    }
}