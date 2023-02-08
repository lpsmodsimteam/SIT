#ifndef BUF_WIDTH_HPP
#define BUF_WIDTH_HPP

#include <string>

std::string align_buffer_width(const std::string& buffer, int width) {
    int _len = buffer.length();
    if (_len < width) {
        return std::string(width - _len, '0') + buffer;
    }
    return buffer;
}

#endif  // BUF_WIDTH_HPP
