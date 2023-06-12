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
        cube.transform.position.z = 3.f;
    }
    void on_draw() override
    {
        static int frame = 0;
        static lynx::rect3D rect;
        rect.transform.position.z = 4.f;
        cube.transform.position.x = 1.f * cosf(0.03f * frame);
        cube.transform.position.y = 1.f * sinf(0.03f * frame++);
        cube.transform.position.z += 0.004f;

        cam->point_to(cube.transform.position);

        m_window.draw(cube);
        m_window.draw(rect);
    }
    lynx::perspective3D *cam;
    lynx::cube3D cube;
};

int main()
{
    DBG_SET_LEVEL(info)
    example_app3D app;
    app.window().maintain_camera_aspect_ratio(false);
    app.run();
}