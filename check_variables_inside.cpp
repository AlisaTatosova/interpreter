#include "interpreter.h"

//type , op1, op2
std::pair<std::string, std::string> Interpreter::check_variables_inside(std::string& str1, std::string& str2, bool flag) {
    std::string tmp1 = str1;
    std::string tmp2 = str2;
    if (is_declared_variable(str1) && is_declared_variable(str2)) { // case if op1 and op2 are already declared variables
        
        if (type_of_var(str1) == "string" && type_of_var(str2) != "string") { 
            throw std::invalid_argument("The types are not muching for operation");
        } else if (type_of_var(str1) != "string" && type_of_var(str2) == "string") {
            throw std::invalid_argument("The types are not muching for operation");
        }   

        get_var_value_inside(tmp1, flag); //getting value inside var
        get_var_value_inside(tmp2, flag); //getting value inside var
        
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

        get_var_value_inside(tmp1, flag); //getting value inside var
        
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

        // else if (!is_number(str1) && str1 != "true" && str1 != "false") { // cheking if given as vars operands are valid
        //     std::cout << str1 << " is not declared variable or propare literal " << std::endl; 
        //     //return;    
        // }

        get_var_value_inside(tmp2, flag); //getting value inside var

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

        // else if (!is_number(str2) && str2 != "true" && str2 != "false") { // cheking if given as vars operands are valid
        //     std::cout << str2 << " is not declared variable or propare literal " << std::endl;
        //     //return;    
        // }

        // else if (!is_number(str1) && str1 != "true" && str1 != "false") { // cheking if given as vars operands are valid
        //     std::cout << str1 << " is not declared variable or propare literal " << std::endl;  
        //    // return;   
        // }
    }

    return {tmp1, tmp2};
}

// str i declared variable, like str = 5; we are making str as 5
void Interpreter::get_var_value_inside(std::string& str, bool flag) {
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