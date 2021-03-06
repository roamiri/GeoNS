

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

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef bg::model::polygon<point> polygon2D;

/**
 * @todo write docs
 */
class node
{
public:
    /**
     * Default constructor
     */
    node(float x, float y, float width, float height, uint32_t id);
    
    /**
     * Destructor
     */
    ~node();
    
    //TODO chanage the virtual function, now just to get the downcast working
    virtual void setID(uint32_t id){m_id=id;}
    uint32_t getID(){return m_id;}
    /**
     * Returns a geometry point of the location.
     */
    point get_loc(){return m_location;}
    box get_box(){return m_box;}
    
    void set_loc(float x, float y);
    
    float getX();
    float getY();
    
private:
    float m_xx;
    float m_yy;
    float m_width;
    float m_height;
    point m_location;
    point m_pmin;  // lower left point
    point m_pmax;  // upper right point
    box m_box;
    uint32_t m_id;



};

#endif // NODE_H
