

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
protected:
    std::vector<boost::shared_ptr<T>> m_items;
    
    bgi::rtree< value, bgi::quadratic<16> > m_tree;
    
    double center_x;
    double center_y;
    double radius;
    
    // Random generators
    std::random_device m_rd;
    std::mt19937 m_generator;
    
    Painter* m_painter;
    
public:
    /**
     * Default constructor
     */
    Container(std::string svg_name)
    {
        m_generator = std::mt19937(m_rd());
        m_painter = new Painter(svg_name);
    //     m_painter->Start();
    }

    /**
     * Destructor
     */
    ~Container(){ delete m_painter;}
    
    void set_center(double x, double y, double r)
    {
        center_x = x;
        center_y = y;
        radius = r;
    }
    
    bool check_neighbors(double x, double y)
    {
        bool ans = true;
    
        // search for nearest neighbours
        std::vector<value> results;
        float xx = (float) x;
        float yy = (float) y;
        point sought(xx,yy);
        m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < def_min_dis;}),
                    std::back_inserter(results));
        if(results.size()>0) ans = false;
        
        return ans;
    }
    
    
    void draw_svg(bool b)
    {
        if(b) m_painter->update<T>(m_items);
    }
    
    int tree_size(double r)
    {
        std::vector<value> results;
        float xx = (float) center_x;
        float yy = (float) center_y;
        point sought(xx,yy);
        m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < r;}),
                    std::back_inserter(results));
        
        return results.size();
    }
    
    void reset_pointers()
    {
        typename std::vector<boost::shared_ptr<T>>::iterator it;
        for(it=m_items.begin(); it!=m_items.end(); ++it)
        {
            boost::shared_ptr<T> mmB = (*it);
            if(mmB)
                mmB.reset();
        }
    }
    
    virtual void generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin) = 0;
    
    
    virtual void load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin) = 0;
    
};

#endif // TEST_H
