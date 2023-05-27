#include "lynx/pch.hpp"
#include "lynx/pipeline.hpp"
#include "lynx/exceptions.hpp"

#include <fstream>

namespace lynx
{
pipeline::pipeline(const device &dev, const char *vert_path, const char *frag_path, const config_info &config)
    : m_device(dev)
{
    init(vert_path, frag_path, config);
}

void pipeline::init(const char *vert_path, const char *frag_path, const config_info &config) const
{
    const std::vector<char> vert = read_file(vert_path);
    const std::vector<char> frag = read_file(frag_path);

    DBG_INFO("Vertex shader code size: {0}", vert.size())
    DBG_INFO("Fragment shader code size: {0}", frag.size())
}

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