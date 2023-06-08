#include "lynx/pch.hpp"
#include "lynx/app.hpp"
#include "lynx/exceptions.hpp"

namespace lynx
{
app::app(const std::uint32_t width, const std::uint32_t height, const char *name) : m_window(width, height, name)
{
}

void app::run()
{
    // const auto system = m_window.get_render_system<line_render_system2D>();
    int frame = 0;

    while (!m_window.should_close())
    {
        transform3D transform;
        transform.translation.z = 0.5f;
        transform.scale = glm::vec3(0.5f);
        transform.rotation.z = (float)frame / 25.f;
        transform.rotation.y = 0.5f * (float)frame++ / 25.f;

        m_window.poll_events();
        m_window.clear();
        line2D line{{-.5f, 0.8f}, {.5f, 0.8f}};

        // system->draw(line);
        m_window.draw(model3D::cube(), TRIANGLE_LIST, transform);
        m_window.draw(line);
        m_window.display();
    }
}
} // namespace lynx