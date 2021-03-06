#ifndef MMWAVEBS_H
#define MMWAVEBS_H

#include <iostream>
#include <mutex>
#include <vector>
#include <map>

#include "common.h"
#include "trx.h"

#include "boost/shared_ptr.hpp"

class mmWaveBS : public TRX
{
public:
    /* Explicitly using the default constructor to
     * underline the fact that it does get called */
    mmWaveBS(double x, double y, uint32_t id, Backhaul tt=wireless, Status st=idle);
//     mmWaveBS(uint32_t id, double ptx);
    ~mmWaveBS();
	
    void reset();
    
    
//     uint32_t getID(){return m_id;}
    
//     void setX(double x){m_xx=x;}
//     void setY(double y){m_yy=y;}
//     
//     double getX(){return m_xx;}
//     double getY(){return m_yy;}
    
    
    
    Backhaul get_backhaul_Type(){return m_bkhl;}
    void set_backhaul_Type(Backhaul st);
    
    Signal<update_parent_msg const &> update_parent;

	
// 	void setColor(std::size_t color);
// 	std::size_t getColor(){return m_color;}
//     std::size_t* get_rgb_Color(){return m_rgb_color;}
    
    double calculate_SNR_of_link(double x, double y, double dist);
    double calculate_SINR_of_link(double x, double y, double dist, double interf);
    double calculate_Rate_of_link(double x, double y, double dist);
    double calculate_distance_of_link(double x, double y);
    double calculate_Interf_of_link(double x, double y, double dist);
    
//     void set_hop_count(int i) {m_hop_cnt=i;}
//     int get_hop_count(){return m_hop_cnt;}
    
//     void set_parent(uint32_t i){m_parent=i;}
//     int get_parent(){return m_parent;}
    
//     void route_found(bool bb){m_found_Route = bb;}
//     bool is_route_found(){return m_found_Route;}
    
    void emit_update_parent();
    
    void add_to_load_BS(boost::shared_ptr<mmWaveBS> bs/*uint32_t id, point p*/);
    void remove_from_load_BS(uint32_t id);
    void update_load_hops();
    int get_load_BS_count();
    void reset_load();
    
    // Return Phi_m in radian
    double get_phi_m(){return m_phi_m*(M_PI/180.);}
    
//     void set_SNR(double snr){m_route_SNR = snr;}
//     double get_SNR(){return m_route_SNR;}
    
//     void set_SINR(double sinr){m_route_SINR = sinr;}
//     double get_SINR(){return m_route_SINR;}

    void Start();
    
private:
    
    std::mutex m_mutex;
    void ThreadMain();
    
    
// 	uint32_t m_id;
    
//     double m_xx;
//     double m_yy;
	
	timer_t T;
// 	std::size_t m_rgb_color[3];
// 	std::size_t m_color;
    
    // RF parameters
//     double m_TxP;
//     double m_TxP_dBm;
//     double m_RxPower;
//     double m_Antenna_Gain;
//     double m_phi_m;
    
    // Routing parameters
    Backhaul m_bkhl;
//     int m_hop_cnt = -1;
//     uint32_t m_parent = def_Nothing;
//     bool m_found_Route = false;
//     double m_route_SNR;
//     double m_route_SINR;
    
    //TODO mybe is should use another data structure or I should hold just to IDs.
//     std::vector<bs_ptr> m_load_BS; // vector containing pointers of the connected BSs as IAB nodes
    std::vector<boost::shared_ptr<mmWaveBS>> m_load_BS;
    
};

typedef boost::shared_ptr<mmWaveBS> bs_ptr;

#endif // MMWAVEBS_H
