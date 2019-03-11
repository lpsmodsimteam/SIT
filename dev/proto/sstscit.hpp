#ifndef SSTSCIT_HPP
#define SSTSCIT_HPP

#include <iostream>

#ifdef ZMQ

#include "zmqsigs.hpp"

#else

#include "socksigs.hpp"

#endif

#endif
