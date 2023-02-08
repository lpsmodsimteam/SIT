#ifndef LINK_WRAP_HPP
#define LINK_WRAP_HPP

#include <sst/core/component.h>

#include "bufwidth.hpp"

class LinkWrapper {
   private:
    bool *_keep_send, *_keep_recv;
    std::vector<int> _buffer_lens;
    SST::Link *_din_link, *_dout_link;

   public:
    LinkWrapper(bool* keep_send, bool* keep_recv) {
        _keep_send = keep_send;
        _keep_recv = keep_recv;
    }

    ~LinkWrapper() {
        _buffer_lens.clear();
    }

    void set_din_link(SST::Link* link) {
        _din_link = link;
    }

    void set_dout_link(SST::Link* link) {
        _dout_link = link;
    }

    template <typename... Args>
    void set_buffer_lengths(const Args&... args) {
        _buffer_lens = {args...};
    }

    template <typename... Args>
    void send(const Args&... args) {

        std::string result;

        if (!_buffer_lens.empty()) {

            int i = 0;
            for (const auto& arg : {args...}) {
                result += align_buffer_width(arg, _buffer_lens[i++]);
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
