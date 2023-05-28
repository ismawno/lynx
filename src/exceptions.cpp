#include "lynx/exceptions.hpp"
#include "lynx/pch.hpp"

namespace lynx
{

device_error::device_error(const char *message) : std::runtime_error(message), m_message(message)
{
}
const char *device_error::what() const noexcept
{
    return m_message;
}

file_not_found_error::file_not_found_error(const char *path) : std::runtime_error(path), m_path(path)
{
}
const char *file_not_found_error::what() const noexcept
{
    return m_path;
}

bad_init::bad_init(const char *message) : std::runtime_error(message), m_message(message)
{
}
const char *bad_init::what() const noexcept
{
    return m_message;
}

bad_deinit::bad_deinit(const char *message) : std::runtime_error(message), m_message(message)
{
}
const char *bad_deinit::what() const noexcept
{
    return m_message;
}

draw_error::draw_error(const char *message) : std::runtime_error(message), m_message(message)
{
}
const char *draw_error::what() const noexcept
{
    return m_message;
}

} // namespace lynx