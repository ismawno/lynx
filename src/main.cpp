#include "lynx/app.hpp"
#include "lynx/camera.hpp"
#include "lynx/input.hpp"
#include <iostream>

class example_app2D : public lynx::app2D
{
    void on_update() override
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
    void on_update() override
    {
        if (lynx::input::key_pressed(lynx::input::key_code::A))
            cube.transform.position.x -= 0.05f;
        if (lynx::input::key_pressed(lynx::input::key_code::D))
            cube.transform.position.x += 0.05f;
        if (lynx::input::key_pressed(lynx::input::key_code::W))
            cube.transform.position.y -= 0.05f;
        if (lynx::input::key_pressed(lynx::input::key_code::S))
            cube.transform.position.y += 0.05f;
        if (lynx::input::key_pressed(lynx::input::key_code::Q))
            cube.transform.rotation.z += 0.05f;
        if (lynx::input::key_pressed(lynx::input::key_code::E))
            cube.transform.rotation.z -= 0.05f;

        cam->point_to(cube.transform.position);

        m_window.draw(cube);
    }
    lynx::perspective3D *cam;
    lynx::cube3D cube;
};

int main()
{
    DBG_SET_LEVEL(info)
    example_app3D app;
    app.run();

    // example_app2D app2;
    // example_app3D app3;
    // app2.start();
    // app3.start();
    // while (true)
    // {
    //     const bool done2 = !app2.next_frame();
    //     const bool done3 = !app3.next_frame();
    //     if (done2 && done3)
    //         break;
    // }
    // app2.shutdown();
    // app3.shutdown();
}