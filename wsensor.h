
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
    wSensor(double x, double y, uint32_t id, Status st=idle)
    :TRX((float)x, (float)y, 1.0, 1.0, id)
    {
        set_carrier(2.4); // f_c = 2.4 GHz
        m_status = st;
    //     m_bkhl = Backhaul::wireless;
        m_route_SNR = 0.; 
        m_route_SINR = 0.;
    }

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
	void declare_as_cluster_head()
    {
        cluster_head_msg message(getX(), getY(), getID(), m_color);
        clusterHead.emit(message);
    }
    
    
    
private:

    std::vector<boost::shared_ptr<wSensor>> m_load_BS;
    uint32_t cluster_id = def_Nothing;
	Status m_status;
    
    void ThreadMain()
    {
        while(!stop_thread)
        {
            // Time interval to avoid overwhelming the thread!
    //         srand(time(NULL));
            double p = ((double)rand()/(double)(RAND_MAX));
            counter(p * 10.0);
            if(is_route_found())
                stop_thread = true;
            // Clustering
            if(m_status == Status::idle)
            {
                srand(time(NULL));
                double p = ((double)rand()/(double)(RAND_MAX));
    // 			std::cout << "pp = " << p << std::endl;
                counter(p * 10.0);
                if(m_status == Status::idle)
                {
                    candidacy_msg message(getX(), getY(), getID());
                    candidacy.emit(message);
                }
            }
        }
    }
    
public:
    void Start()
    {
        the_thread = std::thread(&wSensor::ThreadMain,this);
        char thread_name [20];
        sprintf(thread_name, "wSensor_%d",getID());
        prctl(PR_SET_NAME,thread_name,0,0,0);
        
        char thread_name_buffer[20];
        prctl(PR_GET_NAME, thread_name_buffer, 0L, 0L, 0L);
    }
    
};

typedef boost::shared_ptr<wSensor> ws_ptr;

#endif // WSENSOR_H
