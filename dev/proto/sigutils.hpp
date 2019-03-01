#ifndef SIGUTILS_HPP
#define SIGUTILS_HPP

#include <msgpack.hpp>
#include <zmq.hpp>

#include <sstream>
#include <unistd.h>
#include <unordered_map>

struct signal_packet {

    std::unordered_map<std::string, std::string> data;
    MSGPACK_DEFINE (data);

};

/* -------------------- DECLARATIONS -------------------- */

void send_sigs(zmq::socket_t &, msgpack::packer<msgpack::sbuffer> &,
               msgpack::sbuffer &, zmq::message_t &, signal_packet &);

void recv_sigs(zmq::socket_t &, msgpack::unpacked &, zmq::message_t &,
               signal_packet &);

template<typename T>
std::string _to_string(const T &);

template<typename T>
int _sc_signal_to_int(const T &);

/* -------------------- IMPLEMENTATIONS -------------------- */


void send_sigs(zmq::socket_t &socket, msgpack::packer<msgpack::sbuffer> &packer,
               msgpack::sbuffer &sbuf, zmq::message_t &buf_out, signal_packet &data_out) {

    packer.pack(data_out);
    buf_out.rebuild(sbuf.size());
    std::memcpy(buf_out.data(), sbuf.data(), sbuf.size());
    socket.send(buf_out);
    sbuf.clear();

}

void recv_sigs(zmq::socket_t &socket, msgpack::unpacked &unpacker, zmq::message_t &buf_in,
               signal_packet &data_in) {

    socket.recv(&buf_in);
    msgpack::unpack(unpacker, (char *) (buf_in.data()), buf_in.size());
    unpacker.get().convert(data_in);

}


template<typename T>
std::string _to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}

template<typename T>
int _sc_signal_to_int(const T &value) {

    return std::stoi(_to_string(value));

}

#endif
