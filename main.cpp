#include "interpreter.h"

int main() {
    Interpreter v8;
    std::ifstream file("file.txt");
    v8.parse(file);
    v8.execute(0);
}