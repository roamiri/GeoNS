

#include "node.h"
node::node(point p1, point p2, uint32_t i)
{
    m_p1=p1;
    m_p2=p2;
    m_box = box(p1,p2);
    m_id = i;
}


node::~node()
{

}
