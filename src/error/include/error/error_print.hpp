#ifndef ERROR_PRINT_HPP
#define ERROR_PRINT_HPP

#include "error/error.hpp"

#include <iosfwd>
#include <string>

void PrintDiagnostic(const Diagnostic& diagnostic,
                     const std::string& filename);

#endif // ERROR_PRINT_HPP