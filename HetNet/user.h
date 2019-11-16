
#ifndef USER_H
#define USER_H

#include <iostream>
#include <mutex>
#include <vector>
#include <map>

#include "common.h"
#include "trx.h"

#include "boost/shared_ptr.hpp"

/**
 * @todo User Equipments
 */
class User : public TRX
{
public:
    /**
     * Default constructor
     */
    User(double x, double y, uint32_t id, Status st=idle, typ ss=typ::UE);

    /**
     * Destructor
     */
    ~User();

    void reset();
    
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
};

#endif // USER_H
