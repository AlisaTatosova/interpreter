# Simple C++ Interpreter

## Table of Contents
1. [Introduction](#introduction)
2. [Usage](#usage)
3. [Project Files](#project-files)
4. [Design and Structure](#design-and-structure)
5. [Examples](#examples)
6. 
## Introduction
This is a simple C++ interpreter that can execute C++ programs following a specific structure. It reads and executes C++ source code from a file, which includes variable declarations, loops, input/output statements, and basic mathematical operations.

## Usage
To use this interpreter, follow these steps:

1. Compile the interpreter using a C++ compiler (e.g., g++).
   g++ *.cpp -o interpreter
   
2. Run the interpreter:
./interpreter

## Project Files

* array_declaration.cpp: Handles array variable declarations.
* array_init_list_parse.cpp: Parses array initialization lists.
* check_variables_inside.cpp: Checks the inside of variables within code.
* cin_cout.cpp: Handles input and output statements using cin and cout.
* file.txt: A sample text file for demonstration purposes.
* get_value_inside_array_index.cpp: Retrieves values from array indices.
* getting_value_of_different_types.cpp: Handles variable assignments and operations with different data types.
* helper_functions.cpp: Contains helper functions for the interpreter.
* increment_decrement_parse.cpp: Parses and handles increment and decrement operations.
* interpreter.cpp: The main code for the interpreter logic.
* interpreter.h: Header file for the interpreter with function prototypes.
* main.cpp: The entry point for the interpreter program.
* parse_array_assignment.cpp: Parses array assignment.
* parse_header_file.cpp: Handles the inclusion of header files.
* parse_if_statement.cpp: Parses and executes if statements.
* parse_main.cpp: Handles the execution of the main function.
* parse_while.cpp: Parses and executes while loops.
* zero_token_is_type_parse.cpp: Parses variable declarations and identifies types.
* zero_token_is_variable_parse.cpp: Parses variable declarations.
  
## Design and Structure
The interpreter comprises multiple files, each responsible for different aspects of parsing and executing C++ programs. The interpreter.cpp file is the core of the interpreter, orchestrating the interactions between these components.
The interpreter comprises the following components:

Parser: Reads the C++ source code from a file, breaks the input code into tokens, validates the code's structure and identifies variables, loops, statements, and expressions.
Interpreter: Executes the code based on the parsed information.

## Examples
Here's an example of a target program:

#include <iostream>

int main() {
    int size = 5;
    int numbers[size];
    
    int i = 0;
    while (i < size) {
        std::cout << "Enter number: ";
        std::cin >> numbers[i];
        i++;
    }

    std::cout << "Array Contents:";
    int j = 0;
    while (j < size) {
        std::cout << numbers[j] << std::endl;
        j++;
    }

    int sum = 0;
    int product = 1;
    int counter = 0;
    while (counter < size) {
        sum += numbers[counter];
        product *= numbers[counter];
        counter++;
    }

    std::cout << "Sum of array elements: " << sum << std::endl;
    std::cout << "Product of array elements: " << product << std::endl;
}

Interpreter is able to execute this program and display the sum and product of the array elements.

