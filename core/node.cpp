

#include "node.h"
node::node(float x, float y, float width, float height, uint32_t id, typ tt)
{
    m_xx = x; m_yy=y;
    m_width = width;
    m_height = height;
    float w = width/2.;
    float h = height/2.;
    m_location = point(x,y);
    m_pmin = point(x-w, y-h);
    m_pmax = point(x+w, y+h);
    m_box = box(m_pmin, m_pmax);
    m_id = id;
    m_type = tt;
}


node::~node()
{
    
}

float node::getX()
{
    return m_xx;
}

float node::getY()
{
    return m_yy;
}

void node::set_loc(float x, float y)
{
    m_location = point(x,y);
    float w = m_width/2.;
    float h = m_height/2.;
    m_pmin = point(x-w, y-h);
    m_pmax = point(x+w, y+h);
    m_box = box(m_pmin, m_pmax);
}
