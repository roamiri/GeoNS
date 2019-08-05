#ifndef MMWAVEBS_H
#define MMWAVEBS_H

#include "Signal.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <sys/prctl.h>
#include "common.h"
#include "node.h"
#include <mutex>

#include "boost/shared_ptr.hpp"

class mmWaveBS : public node
{
public:
    /* Explicitly using the default constructor to
     * underline the fact that it does get called */
    mmWaveBS(double x, double y, uint32_t id, double ptx, Backhaul tt=IAB, Status st=idle);
//     mmWaveBS(uint32_t id, double ptx);
    ~mmWaveBS();
    void Start();
	
    void reset();
    void setClusterID(int id) {cluster_id = id;}
    uint32_t getClusterID() {return cluster_id;}
    
//     uint32_t getID(){return m_id;}
    
//     void setX(double x){m_xx=x;}
//     void setY(double y){m_yy=y;}
//     
//     double getX(){return m_xx;}
//     double getY(){return m_yy;}
    
    Status getStatus(){return m_status;}
    void setStatus(Status st){m_status = st;}
    
    Backhaul get_backhaul_Type(){return m_bkhl;}
    void set_backhaul_Type(Backhaul st);
    
    Signal<candidacy_msg const &> candidacy;
    Signal<cluster_head_msg const &> clusterHead;
    Signal<std::string const &> conflict;
    Signal<update_parent_msg const &> update_parent;
    

    void listen(std::string const &message);
	void setColor(std::size_t color);
	void declare_as_cluster_head();
	
	std::size_t getColor(){return m_color;}
    std::size_t* get_rgb_Color(){return m_rgb_color;}
    
    void set_transmit_power(double ptx);
    double calculate_SNR_of_link(double x, double y);
    double calculate_SINR_of_link(double x, double y, double interf);
    double calculate_Rate_of_link(double x, double y);
    double calculate_distance_of_link(double x, double y);
    double calculate_Interf_of_link(double x, double y);
    
    void set_hop_count(int i)
    {
        m_hop_cnt=i;
    }
    int get_hop_count(){return m_hop_cnt;}
    
    void set_IAB_parent(uint32_t i){m_IAB_parent=i;}
    int get_IAB_parent(){return m_IAB_parent;}
    
    void route_found(bool bb){m_found_Route = bb;}
    bool is_route_found(){return m_found_Route;}
    
    void emit_update_parent();
    
    void add_to_load_BS(boost::shared_ptr<mmWaveBS> bs/*uint32_t id, point p*/);
    void remove_from_load_BS(uint32_t id);
    void update_load_hops();
    int get_load_BS_count();
    void reset_load();
    
    // Return Phi_m in radian
    double get_phi_m(){return m_phi_m*(M_PI/180.);}
    
private:
    std::thread the_thread;
	bool stop_thread = false;
    std::mutex m_mutex;
    
    void ThreadMain();
    
// 	uint32_t m_id;
    uint32_t cluster_id = def_Nothing;
//     double m_xx;
//     double m_yy;
	Status m_status;
	
	timer_t T;
	std::size_t m_rgb_color[3];
	std::size_t m_color;
    
    // RF parameters
    double m_TxP;
    double m_TxP_dBm;
    double m_RxPower;
    double m_Antenna_Gain;
    double m_phi_m;
    
    // Routing parameters
    Backhaul m_bkhl;
    int m_hop_cnt = -1;
    uint32_t m_IAB_parent = def_Nothing;
    bool m_found_Route = false;
    
    //TODO mybe is should use another data structure or I should hold just to IDs.
//     std::vector<bs_ptr> m_load_BS; // vector containing pointers of the connected BSs as IAB nodes
    std::vector<boost::shared_ptr<mmWaveBS>> m_load_BS;
    
};

typedef boost::shared_ptr<mmWaveBS> bs_ptr;

#endif // MMWAVEBS_H
