
#include <iostream>
#include "wsensor.h"

wSensor::wSensor(double x, double y, uint32_t id):
TRX((float)x, (float)y, 1.0, 1.0, id)
{
    set_carrier(2.4); // f_c = 2.4 GHz
//     m_bkhl = Backhaul::wireless;
    m_route_SNR = 0.; 
    m_route_SINR = 0.;
}

wSensor::~wSensor()
{

}

void wSensor::reset()
{
    set_hop_count(-1);
    set_parent(def_Nothing);
    set_SNR(0.);
    set_SINR(0.);
}
