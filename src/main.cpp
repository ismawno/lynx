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

    // example_app3D app;
    // app.window().maintain_camera_aspect_ratio(false);
    // app.run();

    lynx::window3D win1{800, 600, "One"};
    lynx::window2D win2{800, 600, "Two"};
    while (!win1.closed() || !win2.closed())
    {
        if (!win1.closed())
        {
            win1.poll_events();
            win1.clear();
            static int frame = 0;
            lynx::cube3D cube;
            cube.transform.position.z = 3.f;
            cube.transform.rotation.y = (float)frame++ / 25.f;
            win1.draw(cube);
            win1.display();
        }
        if (!win2.closed())
        {
            win2.poll_events();
            win2.clear();
            static int frame = 0;
            lynx::rect2D rect;
            rect.transform.rotation = (float)frame++ / 25.f;
            win2.draw(rect);
            win2.display();
        }
    }
}