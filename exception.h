#pragma once
#include <exception>
#include <sstream>
#include <string>

namespace exception {

class Base : public std::exception
{
    const std::string m_what;
public:
    explicit Base(const std::string_view what) noexcept : m_what(what) { }
    [[nodiscard]] const char* what() const noexcept final { return m_what.c_str(); }
};

namespace base {

struct Datagram : Base { explicit Datagram(const std::string_view what) noexcept : Base(what) { } };

namespace datagram {

struct Header : Datagram { explicit Header(const std::string_view what) noexcept : Datagram(what) { } };
struct Footer : Datagram { explicit Footer(const std::string_view what) noexcept : Datagram(what) { } };
struct Crc    : Datagram { explicit Crc   (const std::string_view what) noexcept : Datagram(what) { } };

}

}


template<std::derived_from<Base> T, typename... Args>
T create(Args&& ... args)
{
    std::ostringstream oss;
    (void(oss << std::forward<Args>(args) << ' '), ...);
    return T(std::move(oss).str());
}

}
