
#ifndef WSN_H
#define WSN_H

#include <iostream>
#include <mutex>

#include "container.h"
#include "wsensor.h"
#include "common.h"

#include <boost/shared_ptr.hpp>
#include <boost/pointer_cast.hpp>

/**
 * @def Wireless sensor network
 */
class WSN : public Container<wSensor>
{
public:
    typedef std::vector<boost::shared_ptr<wSensor>>::iterator itt;
    
    
    /**
     * Default constructor
     */
    WSN();

    /**
     * Destructor
     */
    ~WSN();
    
    void listen_For_Candidacy(candidacy_msg const &message);
	void listen_For_ClusterHead(cluster_head_msg const &message);
	void listen_For_Conflict(std::string const &message);
    void joinCluster(uint32_t id, Status st, uint32_t cluster_id, std::size_t color);
	void makeCluster(uint32_t id);
    
    void generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin);
    
    void load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin);
    
    void save(std::string f_name);
    
    void draw_clusters(bool b);
private:
    std::mutex m_mutex;

};

#endif // WSN_H
