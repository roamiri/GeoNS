

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
    
    virtual void generate_fixed_nodes(int count)
    {
        // Adding Fixed Wired BSs
        double delta_teta = 2*M_PI/count;
        for(int i =0;i<count;i++)
        {
            boost::shared_ptr<T> BS;
            double theta = i*delta_teta;
            double r2 = radius/2.;
            double x = center_x + r2 * cos(theta);  // Convert from polar to Cartesian coordinates
            double y = center_y + r2 * sin(theta);
            BS = boost::shared_ptr<T>(new T(x, y, get_nextID()));
            BS.get()->setColor(0);
    //         BS->set_backhaul_Type(Backhaul::wired);
            BS->set_hop_count(0);
            BS->route_found(true);
            m_tree.insert(std::make_pair(BS->get_loc(), BS));
            m_items.push_back(BS);
    //         BS.get()->update_parent.connect_member(this, &Manager::listen_For_parent_update);
        }
    }
    
    virtual void generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin)
    {
        if(fixed) generate_fixed_nodes(fixed_count);
    
        //   Create the nodes
        std::uniform_real_distribution<> dis(0, 1);
        std::poisson_distribution<int> pd(1e6*node_density);
        int num_nodes=pd(m_generator); // Poisson number of points
        //     int num_nodes = 100; // Poisson number of points
        
        for(int i =0;i<num_nodes;i++)
        {
            double theta=2*M_PI*(dis(m_generator));   // angular coordinates
            double rho=radius*sqrt(dis(m_generator));      // radial coordinates
            
            double x = center_x + rho * cos(theta);  // Convert from polar to Cartesian coordinates
            double y = center_y + rho * sin(theta);
            
            bool vicinity = check_neighbors(x,y);
            
            if(vicinity)
            {
                boost::shared_ptr<T> BS;
                BS = boost::shared_ptr<T>(new T(x,y, get_nextID()));
                if(!BS) std::cerr << __FUNCTION__ << std::endl;
                BS.get()->setColor(0);
                m_tree.insert(std::make_pair(BS->get_loc(), BS)); //TODO maybe here!
                m_items.push_back(BS);
    //             BS.get()->update_parent.connect_member(this, &Manager::listen_For_parent_update);
                if(fixed)
                {
    //                 BS->set_backhaul_Type(Backhaul::wireless);
                }
                else
                {
                    bool prob = (rand() % 100) < 100*wired_fractoin;
                    if(prob)
                    {
    //                     BS->set_backhaul_Type(Backhaul::wired);
                        BS->set_hop_count(0); BS->route_found(true);
                    }
    //                 else
    //                     BS->set_backhaul_Type(Backhaul::wireless);
                }
            }
        }
    }
    
    virtual void load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin)
    {
        int num_nodes =0;
    // 	double data[num_nodes][2];
        std::vector<std::array<double,2>> vec_data;
        std::fstream _file(f_name);

        if(_file.fail())
        {
            std::cerr << "input data file does not exis!" << std::endl;
        }
        std::string line;
        while(std::getline(_file, line) && num_nodes<200)
        {
            std::istringstream is(line);
            double num;
            std::array<double,2> arr;
            is >> arr[0];
            is>>arr[1];
            vec_data.push_back(arr);
            num_nodes++;
        }
        
        
        if(fixed) generate_fixed_nodes(fixed_count);
        
        //   Create the nodes
        
        num_nodes= vec_data.size(); // number of points
        
        for(int i =0;i<num_nodes;i++)
        {
            double x = center_x + 1.5*radius* vec_data[i][0];  // Convert from polar to Cartesian coordinates
            double y = center_y + 1.5*radius* vec_data[i][1];
            
            bool vicinity = check_neighbors(x,y);
            
            if(vicinity)
            {
                boost::shared_ptr<T> BS;
                BS = boost::shared_ptr<T>(new T(x,y, get_nextID()));
                if(!BS) std::cerr << __FUNCTION__ << " at " << __LINE__ <<std::endl;
                BS.get()->setColor(0);
                m_tree.insert(std::make_pair(BS->get_loc(), BS)); //TODO maybe here!
                m_items.push_back(BS);
    //             BS.get()->update_parent.connect_member(this, &Manager::listen_For_parent_update);
                if(fixed)
                {
    //                 BS->set_backhaul_Type(Backhaul::wireless);
                }
                else
                {
                    bool prob = (rand() % 100) < 100*wired_fractoin;
                    if(prob)
                    {
    //                     BS->set_backhaul_Type(Backhaul::wired);
                        BS->set_hop_count(0); BS->route_found(true);
                    }
    //                 else
    //                     BS->set_backhaul_Type(Backhaul::wireless);
                }
            }
        }
    }
};

#endif // TEST_H
