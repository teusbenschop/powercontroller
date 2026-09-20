#pragma once
#include <exception>
#include <sstream>
#include <string>

namespace exception {

class Base : public std::exception
{
    std::string m_what{};
public:
    template<typename... Args>
    explicit Base(Args&& ... args)
    {
        std::ostringstream oss;
        (void(oss << std::forward<Args>(args) << ' '), ...);
        m_what = std::move(oss).str();
    }
    [[nodiscard]] const char* what() const noexcept final { return m_what.c_str(); }
};

namespace base {

struct Datagram : Base { using Base::Base; };

namespace datagram {
struct Header : Datagram { using Datagram::Datagram; };
struct Footer : Datagram { using Datagram::Datagram; };
struct Crc    : Datagram { using Datagram::Datagram; };
}

struct Serial : Base { using Base::Base; };

}

// Quality checks: An inherited exception is not larger than the Base exception.
static_assert(sizeof(base::Datagram) == sizeof(Base));
static_assert(sizeof(base::datagram::Header) == sizeof(Base));

}
