#ifndef FILE_FORMAT_ERROR_H
#define FILE_FORMAT_ERROR_H

#include <stdexcept>

class file_format_error : public std::runtime_error {
public:
    
    explicit file_format_error(const char* err_msg)
    :
    std::runtime_error{err_msg}
    {}
};

#endif // FILE_FORMAT_ERROR_H
