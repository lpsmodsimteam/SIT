#ifndef LINK_WRAP_HPP
#define LINK_WRAP_HPP

#include <sst/core/component.h>

class LinkWrapper {
   private:
    bool *_keep_send, *_keep_recv;
    std::vector<int> _buffer_lens;
    SST::Link *_din_link, *_dout_link;

    void align_buffer_width(int width, std::string& buffer) {
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

   public:
    LinkWrapper(bool* keep_send, bool* keep_recv) {
        _keep_send = keep_send;
        _keep_recv = keep_recv;
    }

    ~LinkWrapper() {
        _buffer_lens.clear();
    }

    template <typename... Args>
    void set_buffer_lengths(const Args&... args) {
        _buffer_lens = {args...};
    }

    void set_din_link(SST::Link* link) {
        _din_link = link;
    }

    void set_dout_link(SST::Link* link) {
        _dout_link = link;
    }

    template <typename... Args>
    void send(const Args&... args) {

        std::string result;

        if (!_buffer_lens.empty()) {

            int i = 0;
            for (const auto& p : {args...}) {
                std::string temp = p;
                align_buffer_width(_buffer_lens[i++], temp);
                result += temp;
            }

        } else {

            result = (args + ...);
        }

        _din_link->send(new SST::Interfaces::StringEvent(
            std::to_string(*_keep_send) + std::to_string(*_keep_recv) + result
        ));
    }
};

#endif  // LINK_WRAP_HPP
