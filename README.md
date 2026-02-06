# ParaCL - Simple Programming Language

## ✨ Features

- Lexer generation via Flex
- Parser generation via Bison
- Unit tests with GoogleTest

### 📦 Building the Project
```bash
# Clone the repository
git clone https://github.com/Dariazeml1007/ParaCL.git
cd ParaCL

# Create build directory and compile
## WSL
mkdir build
cmake -B build
cmake --build build
--------------------------------------
## Linux
mkdir -p build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
------------------------------------
./build/paracl
./build/paracl_tests
