#include "mmwavebs.h"

mmWaveBS::mmWaveBS(double x, double y, uint32_t id, double ptx, Backhaul tt, Status st)
: the_thread()
{
	m_xx =x; m_yy =y; m_id =id; m_bkhl= tt; m_status = st;
    m_TxP_dBm = ptx;
    set_transmit_power(ptx);
    if(tt == Backhaul::wired)
        set_hop_count(0);
}

mmWaveBS::~mmWaveBS()
{
	stop_thread = true;
	if(the_thread.joinable()) 
		the_thread.join();
// 	std::cout << "Deconstructor " << __FILE__ << std::endl;
}


void mmWaveBS::Start()
{
    // This will start the thread. Notice move semantics!
    the_thread = std::thread(&mmWaveBS::ThreadMain,this);
    
    char thread_name [20];
    sprintf(thread_name, "SBS_%d",m_id);
    prctl(PR_SET_NAME,thread_name,0,0,0);
    
    char thread_name_buffer[20];
    prctl(PR_GET_NAME, thread_name_buffer, 0L, 0L, 0L);
//     std::cout << "Class " << thread_name_buffer << " has started!" << std::endl;
}

void mmWaveBS::listen(const std::string& message)
{
    std::cout << " received: " << message << std::endl;
}

void mmWaveBS::ThreadMain()
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
// 		if(m_status == Status::idle)
// 		{
// 			srand(time(NULL));
// 			double p = ((double)rand()/(double)(RAND_MAX));
// // 			std::cout << "pp = " << p << std::endl;
// 			counter(p * 10.0);
// 			if(m_status == Status::idle)
// 			{
// 				candidacy_msg message(m_xx, m_yy, m_id);
// 				candidacy.emit(message);
// 			}
// 		}
    }
}

void mmWaveBS::setColor(std::size_t color)
{
	m_color = color;
	std::size_t red = (color & 0xff0000) >> 16; m_rgb_color[0] = red;
	std::size_t green =(color & 0x00ff00) >> 8; m_rgb_color[1] = green;
	std::size_t blue = (color & 0x0000ff);  	m_rgb_color[2] = blue;
}

void mmWaveBS::declare_as_cluster_head()
{
	cluster_head_msg message(m_xx, m_yy, m_id, m_color);
	clusterHead.emit(message);
}

void mmWaveBS::set_transmit_power(double ptx)
{
    double p = ptx - 30;
    m_TxP = pow(10.,p/10.0);
}

double mmWaveBS::calculate_SNR_of_link(double x, double y)
{
    // SNR = S/N
    double dist = euclidean_dist(x, y, m_xx, m_yy);
    double path_loss_db = def_beta + 10.* def_backhaul_alpha_los * log10(dist) + def_backhaul_zeta_los; //TODO implement zeta better with randn, PL = alpha + 10 beta log10(||distance||) + zeta, 
    double plg = m_TxP_dBm + def_G_max + def_G_max - path_loss_db; // S = P_transmit * G_max * G_max / pathloss
    double noise_dBm = -174 + 10*log10(def_BW) + def_NoiseFigure; // N = -174 dBm/Hz + 10log10(BW) + noise figure
    double snr = dBm_to_watt(plg)/dBm_to_watt(noise_dBm);
    return snr;
}

double mmWaveBS::calculate_Rate_of_link(double x, double y)
{
    double snr = calculate_SNR_of_link(x, y);
    double rate = def_BW * log2(1+snr);
    return rate;
}

double mmWaveBS::calculate_distance_of_link(double x, double y)
{
    double dist = euclidean_dist(x, y, m_xx, m_yy);
    return dist;
}

void mmWaveBS::emit_update_parent()
{
    update_parent_msg msg(m_id, m_hop_cnt);
    update_parent.emit(msg);
}
