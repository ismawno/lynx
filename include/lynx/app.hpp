#ifndef LYNX_APP_HPP
#define LYNX_APP_HPP

#include "lynx/pipeline.hpp"
#include "lynx/window.hpp"

#define LYNX_VERTEX_SHADER_PATH SHADER_PATH "shader.vert.spv"
#define LYNX_FRAGMENT_SHADER_PATH SHADER_PATH "shader.frag.spv"

namespace lynx
{
class app
{
  public:
    app(std::uint32_t width = 800, std::uint32_t height = 600, const char *m_name = "Window");

    void run() const;

    window m_window;
    pipeline m_pipeline{LYNX_VERTEX_SHADER_PATH, LYNX_FRAGMENT_SHADER_PATH};
};
} // namespace lynx

#endif