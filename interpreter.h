#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cctype>
#include <vector>
#include <ctime>
#include <random>


class Interpreter {
public:
    Interpreter();

    void parse(std::ifstream& file); // parsing all the text of file, checking syntax errors
    void execute(int i); // executing file line by line

    //type , op1, op2
    void check_vars(std::string& str1, const std::string& type_str1, std::string& str2, const std::string& type_str2); // checking given variables are already declared variables or literals
    template <typename T>
    void add(std::string &res, std::string &str1, const std::string &str2); // function for adding 

private:
    std::map<int, std::vector<std::string>> rows; // contains each line, key - is line number, value - strings of that line
    std::map<std::string, char> char_vars; // saving char variables // key - variable name, value - variable value
    std::map<std::string, int> integer_vars; // saving int variables // key - variable name, value - variable value
    std::map<std::string, float> float_vars; // saving float variables // key - variable name, value - variable value
    std::map<std::string, double> double_vars; // saving double variables // key - variable name, value - variable value
    std::map<std::string, bool> bool_vars; // saving bool variables // key - variable name, value - variable value
    std::map<std::string, std::string> string_vars;// saving string variables // key - variable name, value - variable value

    std::map<std::string, int> types;
    int eip = 0; // instruction ptr

    void type_names(); // nameing types
    bool is_number(const std::string& str); // checking if given string is number
    void remove_semicolon(std::string& input); // removing semicolon from string 
    bool contains_semicolon(const std::string& str); // checking if given string contains semicolon ';'
    bool is_semicolon(const std::string& input); // checking if given string is semicolon ';'
    std::string find_key_by_value(std::map<std::string, int>& my_map, int target_value); // find key of map by value
    bool is_single_char(const std::string& str); // checking if string is single character
    bool last_char_is_semicolon(std::string& str); // checking if last character of line is ';' semilcon
    char convert_to_char(const std::string& str); // convert string to char by checking if inside string is value that is type of char if not throw exception
    int convert_to_int(const std::string& str); // convert string to int by checking if inside string is value that is type of int if not throw exception
    double convert_to_double(const std::string& str); // convert string to double by checking if inside string is value that is type of double if not throw exception
    float convert_to_float(const std::string& str); // convert string to float by checking if inside string is value that is type of float if not throw exception
    bool convert_to_bool(const std::string& str); // convert string to bool by checking if inside string is value that is type of bool if not throw exception
    char extract_last_char(const std::string& str); // extracting last character from string
    bool main_exist(std::vector<std::string>& tokens); // cheking different cases of int main() {
    bool is_declared_variable(const std::string& token); // checking if variable is already declared
    std::string type_of_var(const std::string& token); // detecting type of variable

};

#endif