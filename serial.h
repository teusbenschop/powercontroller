#pragma once

#include <string>
#include <string_view>
#include <sys/termios.h>

class Serial
{
public:
    explicit Serial(std::string_view portname, speed_t speed);
    ~Serial();
    Serial(const Serial&) = delete;
    Serial(const Serial&&) = delete;
    Serial& operator=(Serial&) = delete;
    Serial& operator=(Serial&&) = delete;
    [[nodiscard]] std::string read() const;
private:
    int m_fd{0};
};
