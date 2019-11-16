#ifndef SMALLBS_H
#define SMALLBS_H

#include <iostream>
#include <mutex>
#include <vector>
#include <map>

#include "common.h"
#include "trx.h"

#include "boost/shared_ptr.hpp"

class smallBS : public TRX
{
public:
    /* Explicitly using the default constructor to
     * underline the fact that it does get called */
    smallBS(double x, double y, uint32_t id, Backhaul tt=wireless, Status st=idle, typ ss=typ::SBS);
//     mmWaveBS(uint32_t id, double ptx);
    ~smallBS();
	
    void reset();
    
    Backhaul get_backhaul_Type(){return m_bkhl;}
    void set_backhaul_Type(Backhaul st);

    double calculate_SNR_of_link(double x, double y);
    double calculate_SINR_of_link(double x, double y, double interf);
    double calculate_Rate_of_link(double x, double y);
    double calculate_distance_of_link(double x, double y);
    double calculate_Interf_of_link(double x, double y);
    
    // Return Phi_m in radian
    double get_phi_m(){return m_phi_m*(M_PI/180.);}
    
    void Start();
    
private:
    
    std::mutex m_mutex;
    void ThreadMain();
	timer_t T;

    // Routing parameters
    Backhaul m_bkhl;
//     int m_hop_cnt = -1;
//     uint32_t m_parent = def_Nothing;
//     bool m_found_Route = false;
//     double m_route_SNR;
//     double m_route_SINR;
    
    //TODO mybe is should use another data structure or I should hold just to IDs.
//     std::vector<bs_ptr> m_load_BS; // vector containing pointers of the connected BSs as IAB nodes
//     std::vector<boost::shared_ptr<mmWaveBS>> m_load_BS;
    
};

typedef boost::shared_ptr<smallBS> sbs_ptr;

#endif // SMALLBS_H
