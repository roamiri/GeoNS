

#include "trx.h"


TRX::TRX(float x, float y, float width, float height, uint32_t id, typ tt)
:node(x,y,width,height,id, tt), the_thread()
{
}

TRX::~TRX()
{

}

void TRX::setColor(std::size_t color)
{
    m_color = color;
	std::size_t red = (color & 0xff0000) >> 16; m_rgb_color[0] = red;
	std::size_t green =(color & 0x00ff00) >> 8; m_rgb_color[1] = green;
	std::size_t blue = (color & 0x0000ff);  	m_rgb_color[2] = blue;
}

void TRX::set_transmit_power(double ptx)
{
    m_TxP_dBm = ptx;
    double p = ptx - 30;
    m_TxP = pow(10.,p/10.0);
}
