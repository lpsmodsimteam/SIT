#ifndef SIGWIDTH_HPP
#define SIGWIDTH_HPP

#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#include <iomanip>
#include <sstream>

class LinkWrapper : public SST::Link {
   private:
    bool *m_keep_send, *m_keep_recv;
    SST::Link *din_link, *dout_link;

   public:
    LinkWrapper(bool* keep_send, bool* keep_recv) {
        m_keep_send = keep_send;
        m_keep_recv = keep_recv;
    }

    inline std::string const& to_string(std::string const& s) {
        return s;
    }

    void set_din_link(SST::Link* link) {
        din_link = link;
    }

    void set_dout_link(SST::Link* link) {
        dout_link = link;
    }

    template <typename... Args>
    void send(Args const&... args) {

        std::string s;
        int unpack[]{0, (s += to_string(args), 0)...};
        static_cast<void>(unpack);

        din_link->send(new SST::Interfaces::StringEvent(
            std::to_string(*m_keep_send) + std::to_string(*m_keep_recv) + s
        ));
    }
};

#endif  // SIGWIDTH_HPP
