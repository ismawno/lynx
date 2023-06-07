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
    const auto system = m_window.add_render_system<triangle_strip_render_system3D>();
    int frame = 0;

    model3D &mdl = system->push_model(model3D::cube());
    while (!m_window.should_close())
    {
        mdl.transform.translation.z = 0.5f;
        mdl.transform.scale = glm::vec3(0.5f);
        mdl.transform.rotation.z = (float)frame / 25.f;
        mdl.transform.rotation.y = 0.5f * (float)frame++ / 25.f;

        m_window.poll_events();
        // m_window.clear();
        m_window.display();
    }
}
} // namespace lynx