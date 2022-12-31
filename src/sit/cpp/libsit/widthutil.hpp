#ifndef WIDTH_UTIL_HPP
#define WIDTH_UTIL_HPP

#include <iomanip>
#include <sstream>
#include <string>

/*
Align buffer widths by left-padding with place-holding '0'
*/
void align_buffer_width(int width, std::string& buffer) {
    int _len = buffer.length();
    if (_len < width) {
        buffer = std::string(width - _len, '0') + buffer;
    }
}

void align_buffer_width(int width, float buffer, std::string& new_buffer) {
    std::ostringstream _data_out;
    _data_out << std::fixed << std::setprecision(width) << buffer;
    new_buffer = _data_out.str().substr(0, width);
}

void append_buffer(const char chr, int width, std::string& buffer) {
    int _len = buffer.length();
    if (_len < width) {
        buffer += std::string(width - _len, chr);
    }
}

#endif  // WIDTH_UTIL_HPP
