

#ifndef NODE_H
#define NODE_H

// just for output
#include <iostream>

// to store queries results
#include <vector>

#include <boost/foreach.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;

/**
 * @todo write docs
 */
class node
{
public:
    /**
     * Default constructor
     */
    node(point p1, point p2, uint32_t i);
    
    /**
     * Destructor
     */
    ~node();
    
    int get_id(){return m_id;}
    
private:
    point m_p1;
    point m_p2;
    box m_box;
    uint32_t m_id;



};

#endif // NODE_H
