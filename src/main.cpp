#include "lynx/app.hpp"
#include "lynx/camera.hpp"
#include <iostream>

class example_app2D : public lynx::app2D
{
    void on_draw() override
    {
        static int frame = 0;
        lynx::rect2D rect;
        rect.transform.rotation = (float)frame++ / 25.f;
        m_window.draw(rect);
    }
};

class example_app3D : public lynx::app3D
{
    void on_draw() override
    {
        static int frame = 0;
        lynx::rect3D rect;
        rect.transform.position.z = 2.f;
        rect.transform.rotation.z = (float)frame++ / 25.f;
        rect.transform.rotation.y = (float)frame++ / 25.f;
        m_window.draw(rect);
    }
};

int main()
{
    DBG_SET_LEVEL(info)
    example_app3D app;
    app.window().maintain_camera_aspect_ratio(false);
    app.run();
}