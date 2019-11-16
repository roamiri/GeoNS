
#ifndef TRX_H
#define TRX_H

#include "Signal.h"
#include <thread>
#include <chrono>
#include <sys/prctl.h>

#include "node.h"
#include "common.h"

/**
 * @todo write docs
 */
class TRX :  public node
{
public:

    /**
     * Constructor
     */
    TRX(float x, float y, float width, float height, uint32_t id, typ tt);
    
    /**
     * Destructor
     */
    ~TRX();

//     Backhaul get_backhaul_Type(){return m_bkhl;}
//     void set_backhaul_Type(Backhaul st){m_bkhl = st;}
    
    virtual Backhaul get_backhaul_Type()=0;
    
    void setColor(std::size_t color);
    std::size_t getColor(){return m_color;}
    std::size_t* get_rgb_Color(){return m_rgb_color;}
    
    void set_hop_count(int i) {m_hop_cnt=i;}
    int get_hop_count(){return m_hop_cnt;}
    
    void set_parent(uint32_t i){m_parent=i;}
    int get_parent(){return m_parent;}
    
    void set_SNR(double snr){m_route_SNR = snr;}
    double get_SNR(){return m_route_SNR;}
    
    void set_SINR(double sinr){m_route_SINR = sinr;}
    double get_SINR(){return m_route_SINR;}
    
    void set_carrier(double fc){m_fc = fc;};  // carreir frequency based on GHz
    double get_fc(){return m_fc;}
    
    void route_found(bool bb){m_found_Route = bb;}
    bool is_route_found(){return m_found_Route;}
    
    void set_transmit_power(double ptx);
    
    // For running the thread
    virtual void Start() = 0;
    
protected:
    
    std::size_t m_rgb_color[3];
	std::size_t m_color;
    
    // RF parameters
    double m_fc;
    double m_range;
    double m_TxP;
    double m_TxP_dBm;
    double m_RxPower;
    double m_Antenna_Gain;
    double m_phi_m;
    
    // Routing parameters
//     Backhaul m_bkhl;
    int m_hop_cnt = -1;
    uint32_t m_parent = def_Nothing;
    bool m_found_Route = false;
    double m_route_SNR;
    double m_route_SINR;
    
    // If you want your TRX to be a separate thread
    std::thread the_thread;
	bool stop_thread = false;
    


};

#endif // TRX_H
