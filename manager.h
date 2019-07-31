
#ifndef MANAGER_H
#define MANAGER_H

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>

#include <boost/geometry/index/rtree.hpp>

#include "mmwavebs.h"
#include "common.h"
#include "node.h"
#include "painter.h"

// #include "idgenerator.h"

typedef std::pair<point, std::shared_ptr<node>> value;

class Manager
{
    public:
        
    Manager();
	~Manager();
	
	void listen_For_Candidacy(candidacy_msg const &message);
	void listen_For_ClusterHead(cluster_head_msg const &message);
	void listen_For_Conflict(std::string const &message);
    void listen_For_parent_update(update_parent_msg const &message);
	void joinCluster(uint32_t id, Status st, uint32_t cluster_id, std::size_t color);
	void makeCluster(uint32_t id);
    
    void set_center(double x, double y, double r);
    void generate_nodes(bool fixed, double wired_density);
    void generate_fixed_nodes(int count);
    void update_locations();
    void update_locations(bool fixed, double wired_density);
    void path_selection_WF();
    void path_selection_HQF();
    void set_hop_counts();
    bool check_neighbors(double x, double y);
    int tree_size(double r);
    
    int get_IAB_count();
    int get_wired_count();
    
    void reset(bool fixed);
    
	
	std::vector<std::shared_ptr<mmWaveBS>> m_vector_BSs;
//     static std::shared_ptr<IDGenerator> m_idGenerator;
		
    
private:
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
    
// 	std::shared_ptr<Painter> m_painter;
// 	bool stop_thread = false;
// 	std::thread m_draw_thread;
};

// std::shared_ptr<IDGenerator> Manager::m_idGenerator(IDGenerator::instance());

#endif // MANAGER_H
