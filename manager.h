
#ifndef MANAGER_H
#define MANAGER_H

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>

#include "mmwavebs.h"
#include "common.h"
// #include "painter.h"

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
    
    void path_selection_WF();
    void path_selection_HQF();
    void set_hop_counts();
	
	std::vector<std::shared_ptr<mmWaveBS>> m_vector_BSs;
		
private:
	std::mutex m_mutex;
// 	std::shared_ptr<Painter> m_painter;
// 	bool stop_thread = false;
// 	std::thread m_draw_thread;
};

#endif // MANAGER_H
