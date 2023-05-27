#include "lynx/app.hpp"
#include <iostream>

int main()
{
    lynx::app app;
    app.run();

    std::cout << __FILE__ << "\n";
}