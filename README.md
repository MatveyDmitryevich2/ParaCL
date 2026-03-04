# ParaCL

A simple imperative programming language with an interpreter implemented in C++. ParaCL uses Flex for lexical analysis and Bison for parsing, with a focus on clean architecture and modern C++ practices.

## Features

- Lexical analysis via Flex
- Syntax parsing via Bison
- Abstract Syntax Tree (AST) with RAII memory management
- Interpreter with variable scoping and arithmetic evaluation
- Unit testing with GoogleTest
- CMake-based build system

## Syntax

 The syntax and grammar are based on the course assignment specification.

![ParaCL Syntax Example](images/Syntax.png)

## Building the Project

### Prerequisites
- C++17 compatible compiler (GCC 9+, Clang 10+, or MSVC 2019+)
- CMake 3.13 or higher
- Flex and Bison
- GoogleTest (for building tests)

### WSL / Windows Subsystem for Linux
```bash
git clone https://github.com/Dariazeml1007/ParaCL.git
cd ParaCL

mkdir build
cmake -B build
cmake --build build
```
### Linux
```bash
git clone https://github.com/Dariazeml1007/ParaCL.git
cd ParaCL

mkdir -p build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```
## Running the Interpreter
```bash
./build/ParaCL program.pcl
```

## Testing

The project includes comprehensive unit tests and end-to-end tests.

### Unit Tests (Google Test)

The unit tests cover both the parser and interpreter functionality .

#### Prerequisites
- Google Test framework (automatically downloaded by CMake)

#### Run all unit tests

```bash
# Using the test executable directly
cd build
./tests/pcl_tests
```

# Or using CTest (if configured)
```bash
cd build/tests
ctest -V
```
# Run specific test suites
```bash
# All interpreter tests (42 tests)
./build/tests/pcl_tests --gtest_filter=InterpreterTest.*

# All parser tests (33 tests)
./build/tests/pcl_tests --gtest_filter=ParserTest.*

# Filter tests by name pattern

## Arithmetic tests
./build/tests/pcl_tests --gtest_filter="*Arithmetic*"

## Variable tests
./build/tests/pcl_tests --gtest_filter="*Variable*"

## While loop tests
./build/tests/pcl_tests --gtest_filter="*While*"
```
# End-to-End Tests
The project includes end-to-end tests that run complete programs.

Test programs:
Fibonacci sequence - generates first N numbers

GCD (Euclidean algorithm) - finds greatest common divisor

Factorial - calculates factorial of a number

Collatz sequence - generates Collatz sequence for a given number

## Run all E2E tests
```bash
cd tests/e_2_e
chmod +x run_tests.sh
./run_tests.sh
```

