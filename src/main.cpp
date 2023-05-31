#include "lynx/app.hpp"
#include <iostream>

int main()
{
    DBG_SET_LEVEL(info)
    lynx::app app;
    app.run();
}