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
#include <queue>

class Interpreter {
public:
    Interpreter();
    void parse(std::ifstream& file);
    void execute(int i); // parsing file line by line
    std::pair<std::string, std::string> check_variables_inside(const std::string& str1, const std::string& str2); // checking given variables are already declared variables or literals
    bool parse_header_file(std::vector<std::string>& tokens, int& eip, bool& header_file_exists);
    bool parse_main(std::vector<std::string>& tokens, int& eip, bool& main_exists);

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
    std::map<std::string, std::pair<int, int>> for_scopes;

private:
    //helper functions
    template<typename T>
    bool is_declared_variable(const std::string& token, const std::map<std::string, T>& vars); // checking if variable is already declared
    bool is_declared_variable(const std::string& token);
    bool is_number(const std::string& str); // checking if given string is number
    bool is_single_char(const std::string& str); // checking if string is single character

    void remove_semicolon(std::string& input); // removing semicolon from string 
    bool contains_semicolon(const std::string& str); // checking if given string contains semicolon ';'
    bool last_char_is_semicolon(std::string& str); // checking if last character of line is ';' semilcon
    void remove_semicolons(std::vector<std::string>& tokens); // remove semicolons

    void check_redefinition(const std::string& str); // check variable redefinition
    bool check_existence_of_semicolons(std::vector<std::string>& tokens, int& eip); // checks existence of semicolons

    std::string concatenate_and_remove_spaces(const std::vector<std::string>& vector); // concat all strings in vector and remove spaces
    void remove_double_quotes(std::string& input); // removing double quotes ""
    void remove_single_quotes(std::string& input);// removing single quotes ''
    bool has_first_and_last_double_quotes(const std::string& input); // checking existence of "" in string
    bool has_first_and_last_single_quotes(const std::string& input); // checking existence of '' in char

    template <typename T>
    T get_value_of_var_by_type(const std::string& type, const std::string& var); // getting value of variable by type
    char get_value_of_char(const std::string& token); // getting value of given char
    int get_value_of_int(const std::string& token); // getting value of given int
    double get_value_of_double(const std::string& token); // getting value of given double
    float get_value_of_float(const std::string& token); // getting value of given float
    bool get_value_of_bool(const std::string& token); // getting value of given bool
    std::string get_value_of_string(const std::string& token); //// getting value of given string

    void fill_with_garbage(const std::string& row0, const std::string& row1); // fill with garbage uninitialized variable
    void fill_var_with_given_value(const std::string& row0, const std::string& row1, const std::string& row3); // fill variable with given value
    void fill_var_with_given_variable_value(const std::string& row0, const std::string& row1, const std::string& row2);  // fill variable with given variable value
   
    void zero_token_is_type_parse(std::vector<std::string>& tokens); // case when tokens[0] is type
    void zero_token_is_var_parse(std::vector<std::string>& tokens); //case when tokens[0] is variable
    
    template <typename T>
    T convert_to_type(std::string& str);
    void adding_cases(std::vector<std::string>& tokens, const std::string& dest_var, std::string& str1, std::string& str2); 
    void sub_cases(std::vector<std::string>& tokens, std::string& dest_var, std::string& str1, std::string& str2);
    void mul_cases(std::vector<std::string>& tokens, std::string& dest_var, std::string& str1, std::string& str2);
    void div_cases(std::vector<std::string>& tokens, std::string& dest_var, std::string& str1, std::string& str2);

    bool is_one_byte_integer(int num);
    void is_valid_parentheses(std::stack<char>& st, char str);
    void get_var_value_inside(std::string& str); //
    std::string find_brace(const std::vector<std::string>& vec);
    bool brace_exist(const std::vector<std::string>& vec);
    bool last_char_is_scope(std::string& str);
    std::string extract_paren(std::string& input, char brace);
    size_t find_first_close_brace(const std::vector<std::string>& vec);

    std::string find_key_by_value(std::map<std::string, int>& my_map, int target_value); // find key of map by value
    std::string type_of_var(const std::string& token); // detecting type of variable
    void cout(std::vector<std::string>& tokens); //cout

    template <typename T>
    void delete_key_from_map(std::map<std::string, T>& map, const std::string& key); // deleting key from map after if statement to clear the declarations inside if statement

    void first_token_is_variable_parse(std::vector<std::string>& tokens);
    void parse_if_statement(std::vector<std::string>& tokens, bool& if_enter_flag);
    bool parse_while(std::vector<std::string>& tokens);
    std::string remove_quotes_preserve_whitespace(const std::string& input);
    bool find_open_close_brace(const std::string& str, char brace);

    void detect_if_and_while(std::ifstream& file);
    bool start_with_plus_plus(const std::string& str);
    bool end_with_plus_plus(const std::string& str);
    std::string extract_plus_plus(const std::string& input);
    bool start_with_minus_minus(const std::string& str);

    // Function to check if a string ends with "++"
    bool end_with_minus_minus(const std::string& str);

    // Function to extract substrings containing "++" at the beginning or end
    std::string extract_minus_minus(const std::string& input);
};

#endif