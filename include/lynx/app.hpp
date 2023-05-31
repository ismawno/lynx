#ifndef LYNX_APP_HPP
#define LYNX_APP_HPP

#include "lynx/core.hpp"
#include "lynx/pipeline.hpp"
#include "lynx/window.hpp"
#include "lynx/device.hpp"
#include "lynx/swap_chain.hpp"
#include "lynx/model.hpp"

#define LYNX_VERTEX_SHADER_PATH SHADER_PATH "shader.vert.spv"
#define LYNX_FRAGMENT_SHADER_PATH SHADER_PATH "shader.frag.spv"

namespace lynx
{
class app
{
  public:
    app(std::uint32_t width = 800, std::uint32_t height = 600, const char *m_name = "Window");
    ~app();

    void run();

  private:
    window m_window;
    device m_device{m_window};
    scope<swap_chain> m_swap_chain = nullptr;
    scope<pipeline> m_pipeline = nullptr;
    scope<model> m_model = nullptr;

    VkPipelineLayout m_pipeline_layout;
    std::vector<VkCommandBuffer> m_command_buffers;

    void load_models();
    void create_pipeline_layout();
    void create_swap_chain();
    void create_pipeline();
    void create_command_buffers();
    void draw_frame();
    void record_command_buffer(std::size_t image_index);

    void free_command_buffers();

    app(const app &) = delete;
    app &operator=(const app &) = delete;
};
} // namespace lynx

#endif