#ifndef LYNX_EXCEPTIONS_HPP
#define LYNX_EXCEPTIONS_HPP

#include <stdexcept>

namespace lynx
{
class device_error : public std::runtime_error
{
  public:
    explicit device_error(const char *message);
    const virtual char *what() const noexcept override;

  private:
    const char *m_message;
};

class file_not_found_error : public std::runtime_error
{
  public:
    explicit file_not_found_error(const char *path);
    const virtual char *what() const noexcept override;

  private:
    const char *m_path;
};

class bad_init : public std::runtime_error
{
  public:
    explicit bad_init(const char *message);
    const virtual char *what() const noexcept override;

  private:
    const char *m_message;
};

} // namespace lynx

#endif