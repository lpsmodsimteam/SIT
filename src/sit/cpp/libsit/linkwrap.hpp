#ifndef LINK_WRAP_HPP
#define LINK_WRAP_HPP

#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#include <iomanip>
#include <sstream>

class LinkWrapper : public SST::Link {
   private:
    bool *m_keep_send, *m_keep_recv;
    SST::Link *din_link, *dout_link;
    std::vector<int> buffer_lengths;

    void align_buffer_width(int width, std::string& buffer) {
        int _len = buffer.length();
        if (_len < width) {
            buffer = std::string(width - _len, '0') + buffer;
        }
    }

    std::string align_buffer_width(int width, float buffer) {
        std::ostringstream _data_out;
        _data_out << std::fixed << std::setprecision(width) << buffer;
        return _data_out.str().substr(0, width);
    }

    void append_buffer(const char chr, int width, std::string& buffer) {
        int _len = buffer.length();
        if (_len < width) {
            buffer += std::string(width - _len, chr);
        }
    }

   public:
    LinkWrapper(bool* keep_send, bool* keep_recv) {
        m_keep_send = keep_send;
        m_keep_recv = keep_recv;
    }

    ~LinkWrapper() {
        buffer_lengths.clear();
    }

    template <typename... Args>
    void set_buffer_lengths(const Args&... args) {
        buffer_lengths = {args...};
    }

    void set_din_link(SST::Link* link) {
        din_link = link;
    }

    void set_dout_link(SST::Link* link) {
        dout_link = link;
    }

    template <typename... Args>
    void send(const Args&... args) {

        std::string result;

        if (!buffer_lengths.empty()) {

            int i = 0;
            for (const auto& p : {args...}) {
                std::string temp = p;
                align_buffer_width(buffer_lengths[i++], temp);
                result += temp;
            }

        } else {

            for (const auto& p : {args...}) {
                result += p;
            }
        }

        din_link->send(new SST::Interfaces::StringEvent(
            std::to_string(*m_keep_send) + std::to_string(*m_keep_recv) + result
        ));
    }
};

#endif  // LINK_WRAP_HPP
