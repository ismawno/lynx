#ifndef LYNX_RENDERER_HPP
#define LYNX_RENDERER_HPP

#include "lynx/core.hpp"
#include "lynx/render_systems.hpp"
#include "lynx/swap_chain.hpp"
#include <vulkan/vulkan.hpp>

namespace lynx
{
class window;
class device;
class renderer
{
  public:
    renderer(const device &dev, window &win);
    ~renderer();

    VkCommandBuffer begin_frame();
    void end_frame();

    void begin_swap_chain_render_pass(VkCommandBuffer command_buffer) const;
    void end_swap_chain_render_pass(VkCommandBuffer command_buffer) const;

    bool frame_in_progress() const;
    VkCommandBuffer current_command_buffer() const;
    std::uint32_t frame_index() const;

    template <typename T, class... Args> const T *add_render_system(Args &&...args)
    {
        static_assert(std::is_base_of<render_system, T>::value, "Type must inherit from render system!");

        auto system = make_scope<T>(std::forward<Args>(args)...);
        const T *ref = system.get();

        system->init(&m_device, m_swap_chain->render_pass());
        m_render_systems.push_back(std::move(system));
        return ref;
    }

  private:
    window &m_window;
    const device &m_device;
    scope<swap_chain> m_swap_chain;
    std::vector<VkCommandBuffer> m_command_buffers;
    std::vector<scope<render_system>> m_render_systems;

    std::uint32_t m_image_index;
    std::uint32_t m_frame_index = 0;
    bool m_frame_started = false;

    void create_command_buffers();
    void create_swap_chain();
    void free_command_buffers();

    renderer(const renderer &) = delete;
    renderer &operator=(const renderer &) = delete;
};
} // namespace lynx

#endif