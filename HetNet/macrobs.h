
#ifndef MACROBS_H
#define MACROBS_H

#include <iostream>
#include <mutex>
#include <vector>
#include <map>

#include "common.h"
#include "trx.h"

#include "boost/shared_ptr.hpp"

/**
 * @todo write docs
 */
class MacroBS: public TRX
{
public:
    /**
     * Default constructor
     */
    MacroBS(double x, double y, uint32_t id, Backhaul tt=wired, Status st=idle, typ ss=typ::MBS);

    /**
     * Destructor
     */
    ~MacroBS();
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

};

typedef boost::shared_ptr<MacroBS> mbs_ptr;

#endif // MACROBS_H
