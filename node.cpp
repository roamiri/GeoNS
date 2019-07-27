

#include "node.h"
node::node(float x, float y, float width, float height, uint32_t id)
{
    m_width = width;
    m_height = height;
    float w = width/2.;
    float h = height/2.;
    m_location = point(x,y);
    m_pmin = point(x-w, y-h);
    m_pmax = point(x+w, y+h);
    m_box = box(m_pmin, m_pmax);
    m_id = id;
    
    
}


node::~node()
{

}
