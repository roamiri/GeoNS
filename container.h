

#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>

#include <boost/geometry/index/rtree.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "mmwavebs.h"
#include "common.h"
#include "node.h"
#include "painter.h"


typedef std::pair<point, boost::shared_ptr<node>> value;

/**
 * @def Manager class that contains the object of type T
 */
template <class T>
class Container
{
public:
    /**
     * Default constructor
     */
    Container(std::string svg_name);

    /**
     * Destructor
     */
    ~Container();
    
    void set_center(double x, double y, double r);
    
    void generate_fixed_nodes(int count);
    void generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin);
    void load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin);
    bool check_neighbors(double x, double y);
    
    
    void draw_svg(bool b);

private:
    std::vector<boost::shared_ptr<T>> m_items;
    
    bgi::rtree< value, bgi::quadratic<16> > m_tree;
    
    double center_x;
    double center_y;
    double radius;
    
    // Random generators
    std::random_device m_rd;
    std::mt19937 m_generator;
    
    Painter* m_painter;
    
};

#endif // TEST_H
