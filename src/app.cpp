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
    int frame = 0;

    while (!m_window.should_close())
    {
        // cube3D cube1;
        // cube1.transform.translation.z = 2.f;
        // cube1.transform.scale = glm::vec3(0.5f);
        // cube1.transform.rotation.z = (float)frame / 25.f;
        // cube1.transform.rotation.y = 0.5f * (float)frame++ / 25.f;

        rect3D rect;
        rect.transform.position.z = 2.f;
        rect.transform.rotation.y = (float)frame / 25.f;
        rect.transform.rotation.z = 0.5f * (float)frame++ / 25.f;

        m_window.poll_events();
        m_window.clear();
        m_window.draw(rect);
        m_window.display();
    }
}
} // namespace lynx