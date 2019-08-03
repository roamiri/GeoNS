
#ifndef MANAGER_H
#define MANAGER_H

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

// #include "idgenerator.h"

typedef std::pair<point, boost::shared_ptr<node>> value;

// typedef boost::shared_ptr<mmWaveBS> bs_ptr;


class Manager
{
    public:
        
    Manager(std::string svg_name);
	~Manager();
	
	void listen_For_Candidacy(candidacy_msg const &message);
	void listen_For_ClusterHead(cluster_head_msg const &message);
	void listen_For_Conflict(std::string const &message);
    void listen_For_parent_update(update_parent_msg const &message);
	void joinCluster(uint32_t id, Status st, uint32_t cluster_id, std::size_t color);
	void makeCluster(uint32_t id);
    
    void set_center(double x, double y, double r);
    void generate_nodes(bool fixed, int fixed_count, double wired_density);
    void load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_density);
    void generate_fixed_nodes(int count);
//     void update_locations();
    void update_locations(bool fixed, double wired_density);
    void path_selection_WF();
    void path_selection_HQF();
    void path_selection_PA();
    void path_selection_MLR();
    
    std::vector<int> count_hops(int &max_hop, int &failed);
    void set_hop_counts();
    void spread_hop_count();
    bool check_neighbors(double x, double y);
    int tree_size(double r);
    
    int get_IAB_count();
    int get_wired_count();
    
    void reset(bool fixed);
    void draw_svg(bool b);
    
    void reset_pointers();
    
    point find_closest_wired(point loc);
    
	
// 	std::vector<bs_ptr> m_vector_BSs;
    std::vector<bs_ptr> m_vector_BSs;
//     static std::shared_ptr<IDGenerator> m_idGenerator;
		
    
private:
    
    void Add_load_BS(uint32_t parent, bs_ptr bs);

    // create the rtree using default constructor
    //TODO figure out the 16 number
    bgi::rtree< value, bgi::quadratic<16> > m_tree;
   
	std::mutex m_mutex;
    Painter* m_painter;
    
    double center_x;
    double center_y;
    double radius;
    
    std::random_device rd;
    std::mt19937 gen_wired;
    std::mt19937 gen_IAB;
    
//     std::vector<int> m_arr_hops[15];
    int m_failed;
    int m_max_hop;
// 	std::shared_ptr<Painter> m_painter;
// 	bool stop_thread = false;
// 	std::thread m_draw_thread;
};

// std::shared_ptr<IDGenerator> Manager::m_idGenerator(IDGenerator::instance());

#endif // MANAGER_H
