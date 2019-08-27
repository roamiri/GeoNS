
#include <iostream>
#include "wsensor.h"

void wSensor::listen(const std::string& message)
{
    std::cout << " received: " << message << std::endl;
}
