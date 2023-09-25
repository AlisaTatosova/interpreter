#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <ctime>
#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <vector>
#include <stack>

class Interpreter {
public:
    Interpreter();
    void parse(std::ifstream& file, int i); // parsing file line by line
    void check_vars(std::string& str1, std::string& str2); // checking given variables are already declared variables or literals

    template <typename T>
    void add(std::string& res, std::string& str1, std::string& str2);

    template <typename T>
    void sub(std::string& res, std::string& op1, std::string& op2);

    template <typename T>
    void multiply(std::string& res, std::string& op1, std::string& op2);

    template <typename T>
    void divide(std::string& res, std::string& op1, std::string& op2);

private:
    std::map<int, std::vector<std::string>> rows; // contains each line, key - is line number, value - strings of that line
    std::map<std::string, std::pair<std::string, char>> char_vars; // saving char variables // key - variable name, value - variable value
    std::map<std::string, std::pair<std::string, int>> integer_vars; // saving int variables // key - variable name, value - variable value
    std::map<std::string, std::pair<std::string, float>> float_vars; // saving float variables // key - variable name, value - variable value
    std::map<std::string, std::pair<std::string, double>> double_vars; // saving double variables // key - variable name, value - variable value
    std::map<std::string, std::pair<std::string, bool>> bool_vars; // saving bool variables // key - variable name, value - variable value
    std::map<std::string, std::pair<std::string, std::string>> string_vars;// saving string variables // key - variable name, value - variable value
    std::map<int, std::string> types;
    

private:
    //helper functions
    void type_names(); // nameing types
    bool is_number(const std::string& str); // checking if given string is number
    void remove_semicolon(std::string& input); // removing semicolon from string 
    bool contains_semicolon(const std::string& str); // checking if given string contains semicolon ';'
    bool is_semicolon(const std::string& input); // checking if given string is semicolon ';'
    void remove_double_quotes(std::string& input); // removing double quotes ""
    void remove_single_quotes(std::string& input);// removing single quotes ''
    bool has_first_and_last_double_quotes(const std::string& input); // checking existence of "" in string
    bool has_first_and_last_single_quotes(const std::string& input); // checking existence of '' in char
    std::string find_key_by_value(std::map<std::string, int>& my_map, int target_value); // find key of map by value
    bool is_single_char(const std::string& str); // checking if string is single character
    bool last_char_is_semicolon(std::string& str); // checking if last character of line is ';' semilcon
    // char convert_to_char(const std::string& str); // convert string to char by checking if inside string is value that is type of char if not throw exception
    // int convert_to_int(std::string& str); // convert string to int by checking if inside string is value that is type of int if not throw exception
    // double convert_to_double(const std::string& str); // convert string to double by checking if inside string is value that is type of double if not throw exception
    // float convert_to_float(const std::string& str); // convert string to float by checking if inside string is value that is type of float if not throw exception
    // bool convert_to_bool(const std::string& str); // convert string to bool by checking if inside string is value that is type of bool if not throw exception
    std::string type_of_var(const std::string& token); // detecting type of variable

    template<typename T>
    bool is_declared_variable(const std::string& token, const std::map<std::string, T>& vars); // checking if variable is already declared
    bool is_declared_variable(const std::string& token);

    template <typename T>
    T get_value_of_var_by_type(const std::string& type, const std::string& var); // getting value of variable by type

    char get_value_of_char(const std::string& token); // getting value of given char
    int get_value_of_int(const std::string& token); // getting value of given int
    double get_value_of_double(const std::string& token); // getting value of given double
    float get_value_of_float(const std::string& token); // getting value of given float
    bool get_value_of_bool(const std::string& token); // getting value of given bool
    std::string get_value_of_string(const std::string& token); //// getting value of given string

private:
    void fill_with_garbage(const std::string& row0, const std::string& row1); // fill with garbage uninitialized variable
    void fill_var_with_given_value(const std::string& row0, const std::string& row1, std::string& row3); // fill variable with given value
    void fill_var_with_given_variable_value(const std::string& row0, const std::string& row1, std::string& row2);  // fill variable with given variable value
    std::string concatenate_and_remove_spaces(const std::vector<std::string>& vector); // concat all strings in vector and remove spaces
    void check_redefinition(const std::string& str); // check variable redefinition
    bool check_existence_of_semicolons(std::vector<std::string>& tokens, int& eip); // checks existence of semicolons
    void remove_semicolons(std::vector<std::string>& tokens); // remove semicolons
    void zero_token_is_type(std::vector<std::string>& tokens); // case when tokens[0] is type
    void zero_token_is_var(std::vector<std::string>& tokens); //case when tokens[0] is variable
    void cout(std::vector<std::string>& tokens); //cout
    bool parse_header_file(std::vector<std::string>& tokens, int& eip, bool& header_file_exists);
    bool parse_main(std::vector<std::string>& tokens, int& eip, bool& main_exists);

    template <typename T>
    T convert_to_type(std::string& str);
    void adding_cases(std::vector<std::string>& tokens, std::string& dest_var, int first, int second); 
    void sub_cases(std::vector<std::string>& tokens, std::string& dest_var, int first, int second);
    void mul_cases(std::vector<std::string>& tokens, std::string& dest_var, int first, int second);
    void div_cases(std::vector<std::string>& tokens, std::string& dest_var, int first, int second);

    bool is_one_byte_integer(int num);
    void get_var_value_inside(std::string& str);
    void is_valid_parentheses(std::stack<char>& st, char str);
    std::string find_brace(const std::vector<std::string>& vec);
    bool brace_exist(const std::vector<std::string>& vec);
    bool last_char_is_scope(std::string& str);
};

#endif