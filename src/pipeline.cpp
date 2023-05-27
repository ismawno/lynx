#include "lynx/pipeline.hpp"
#include "lynx/pch.hpp"

#include <fstream>

namespace lynx
{
pipeline::pipeline(const char *vert_path, const char *frag_path)
{
    init(vert_path, frag_path);
}

void pipeline::init(const char *vert_path, const char *frag_path)
{
    const std::vector<char> vert = read_file(vert_path);
    const std::vector<char> frag = read_file(frag_path);

    DBG_INFO("Vertex shader code size: {0}", vert.size())
    DBG_INFO("Fragment shader code size: {0}", frag.size())
}

class file_not_found_error : public std::runtime_error
{
  public:
    explicit file_not_found_error(const char *path) : std::runtime_error(path), m_path(path)
    {
    }
    const virtual char *what() const noexcept override
    {
        return m_path;
    }

  private:
    const char *m_path;
};

std::vector<char> pipeline::read_file(const char *path)
{
    std::ifstream file{path, std::ios::ate | std::ios::binary};
    if (!file.is_open())
        throw file_not_found_error(path);

    const long file_size = file.tellg();
    std::vector<char> buffer((std::size_t)file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);
    return buffer;
}
} // namespace lynx