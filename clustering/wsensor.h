
#ifndef WSENSOR_H
#define WSENSOR_H

#include "trx.h"
#include "common.h"
#include <boost/shared_ptr.hpp>
/**
 * @todo write docs
 */
class wSensor :  public TRX
{
public:
    /**
     * Default constructor
     */
    wSensor(double x, double y, uint32_t id, Status st=idle);

    /**
     * Destructor
     */
    ~wSensor(){}

    void reset()
    {
        set_hop_count(-1);
        set_parent(def_Nothing);
        set_SNR(0.);
        set_SINR(0.);
    }
    
    void setClusterID(int id) {cluster_id = id;}
    uint32_t getClusterID() {return cluster_id;}
    
    Status getStatus(){return m_status;}
    void setStatus(Status st){m_status = st;}
    
    Backhaul get_backhaul_Type(){return Backhaul::wireless;}
    
    Signal<candidacy_msg const &> candidacy;
    Signal<cluster_head_msg const &> clusterHead;
    Signal<std::string const &> conflict;
    
    void listen(std::string const &message);
	void declare_as_cluster_head();    
    
private:

    std::vector<boost::shared_ptr<wSensor>> m_load_BS;
    uint32_t cluster_id = def_Nothing;
	Status m_status;
    
    void ThreadMain();
    
public:
    void Start();
    
};

typedef boost::shared_ptr<wSensor> ws_ptr;

#endif // WSENSOR_H
