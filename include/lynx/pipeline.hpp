#ifndef LYNX_PIPELINE_HPP
#define LYNX_PIPELINE_HPP

#include "lynx/device.hpp"
#include <vector>

namespace lynx
{
class pipeline
{
  public:
    struct config_info
    {
    };

    pipeline(const device &dev, const char *vert_path, const char *frag_path, const config_info &config);

  private:
    const device &m_device;

    void init(const char *vert_path, const char *frag_path, const config_info &config) const;
    void create_shader_module(const std::vector<char> &code, VkShaderModule *shader_module);

    static std::vector<char> read_file(const char *path);

    pipeline(const pipeline &) = delete;
    void operator=(const pipeline &) = delete;

    pipeline(pipeline &&) = delete;
    pipeline &operator=(pipeline &&) = delete;
};
} // namespace lynx

#endif