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
    void on_start() override
    {
        cam = m_window.camera_as<lynx::perspective3D>();
    }
    void on_draw() override
    {
        static int frame = 0;
        lynx::rect3D rect;
        rect.transform.position.z = 2.f;
        // m_window.camera().transform.rotation.y = (float)frame++ / 50.f;
        rect.transform.rotation.z = (float)frame++ / 50.f;
        m_window.draw(rect);
    }
    lynx::perspective3D *cam;
};

int main()
{
    DBG_SET_LEVEL(info)
    example_app3D app;
    app.window().maintain_camera_aspect_ratio(false);
    app.run();
}