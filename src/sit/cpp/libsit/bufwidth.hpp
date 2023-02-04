#ifndef BUF_WIDTH_HPP
#define BUF_WIDTH_HPP

#include <string>

void align_buffer_width(std::string& buffer, int width) {
    int _len = buffer.length();
    if (_len < width) {
        buffer = std::string(width - _len, '0') + buffer;
    }
}

// std::string align_buffer_width(int width, float buffer) {
//     std::ostringstream _data_out;
//     _data_out << std::fixed << std::setprecision(width) << buffer;
//     return _data_out.str().substr(0, width);
// }

// void append_buffer(const char chr, int width, std::string& buffer) {
//     int _len = buffer.length();
//     if (_len < width) {
//         buffer += std::string(width - _len, chr);
//     }
// }

#endif  // BUF_WIDTH_HPP
