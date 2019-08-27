
#include <iostream>
#include "wsensor.h"

wSensor::wSensor(double x, double y, uint32_t id, Status st)
:TRX((float)x, (float)y, 1.0, 1.0, id)
{
    set_carrier(2.4); // f_c = 2.4 GHz
    m_status = st;
//     m_bkhl = Backhaul::wireless;
    m_route_SNR = 0.; 
    m_route_SINR = 0.;
}
    
void wSensor::listen(const std::string& message)
{
    std::cout << " received: " << message << std::endl;
}

void wSensor::ThreadMain()
{
    while(!stop_thread)
    {
        // Time interval to avoid overwhelming the thread!
//         srand(time(NULL));
//         double p = ((double)rand()/(double)(RAND_MAX));
//         counter(p * 10.0);
//         if(is_route_found())
//             stop_thread = true;
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

void wSensor::Start()
{
    the_thread = std::thread(&wSensor::ThreadMain,this);
    char thread_name [20];
    sprintf(thread_name, "wSensor_%d",getID());
    prctl(PR_SET_NAME,thread_name,0,0,0);
    
    char thread_name_buffer[20];
    prctl(PR_GET_NAME, thread_name_buffer, 0L, 0L, 0L);
}

void wSensor::declare_as_cluster_head()
{
    cluster_head_msg message(getX(), getY(), getID(), m_color);
    clusterHead.emit(message);
}
