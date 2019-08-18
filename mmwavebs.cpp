#include "mmwavebs.h"
#include <random>

mmWaveBS::mmWaveBS(double x, double y, uint32_t id, Backhaul tt, Status st)
: TRX((float)x,(float)y,1.,1.,id)
{
	m_bkhl= tt;
    m_phi_m = def_Phi_m;
    m_route_SNR = 0.; m_route_SINR = 0.;
    if(tt == Backhaul::wired)
    {
        set_hop_count(0);
        route_found(true);
    }
}

// mmWaveBS::mmWaveBS(uint32_t id, double ptx)
// : the_thread()
// {
//     setID(id); 
//     m_TxP = ptx;
//     set_transmit_power(ptx);
// }

void mmWaveBS::reset()
{
    set_hop_count(-1);
    set_parent(def_Nothing);
    set_SNR(0.);
    set_SINR(0.);
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
    sprintf(thread_name, "SBS_%d",getID());
    prctl(PR_SET_NAME,thread_name,0,0,0);
    
    char thread_name_buffer[20];
    prctl(PR_GET_NAME, thread_name_buffer, 0L, 0L, 0L);
//     std::cout << "Class " << thread_name_buffer << " has started!" << std::endl;
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

// void mmWaveBS::setColor(std::size_t color)
// {
// 	m_color = color;
// 	std::size_t red = (color & 0xff0000) >> 16; m_rgb_color[0] = red;
// 	std::size_t green =(color & 0x00ff00) >> 8; m_rgb_color[1] = green;
// 	std::size_t blue = (color & 0x0000ff);  	m_rgb_color[2] = blue;
// }


double mmWaveBS::calculate_SNR_of_link(double x, double y)
{
    // SNR = S/N
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(0., sqrt(def_backhaul_zeta_los));
    
    double dist = euclidean_dist(x, y, (double) getX(), (double) getY());
    double zeta = dis(gen);
    double path_loss_db = def_beta + 10.* def_backhaul_alpha_los * log10(dist) + zeta; //TODO implement zeta better with randn, PL = alpha + 10 beta log10(||distance||) + zeta, 
    double plg = m_TxP_dBm + def_G_max + def_G_max - path_loss_db; // S = P_transmit * G_max * G_max / pathloss
    double noise_dBm = -174 + 10*log10(def_BW) + def_NoiseFigure; // N = -174 dBm/Hz + 10log10(BW) + noise figure //TODO def_BW should be derives based on resource allocation
    double snr = dBm_to_watt(plg)/dBm_to_watt(noise_dBm);
    return snr;
}

/**
 * interf should be based on Watt not Decibel
 */
double mmWaveBS::calculate_SINR_of_link(double x, double y, double interf)
{
    // SNR = S/N
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(0., sqrt(def_backhaul_zeta_los));
    
    double dist = euclidean_dist(x, y, (double) getX(), (double) getY());
    double zeta = dis(gen);
    double path_loss_db = def_beta + 10.* def_backhaul_alpha_los * log10(dist) + zeta; //TODO implement zeta better with randn, PL = alpha + 10 beta log10(||distance||) + zeta, 
    double plg = m_TxP_dBm + def_G_max + def_G_max - path_loss_db; // S = P_transmit * G_max * G_max / pathloss
    double noise_dBm = -174 + 10*log10(def_BW) + def_NoiseFigure; // N = -174 dBm/Hz + 10log10(BW) + noise figure //TODO def_BW should be derives based on resource allocation
    double sinr = dBm_to_watt(plg)/(dBm_to_watt(noise_dBm)+interf);
    return sinr;
}

double mmWaveBS::calculate_Interf_of_link(double x, double y)
{
    // SNR = S/N
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(0., sqrt(def_backhaul_zeta_los));
    
    double dist = euclidean_dist(x, y, (double) getX(), (double) getY());
    double zeta = dis(gen);
    double path_loss_db = def_beta + 10.* def_backhaul_alpha_los * log10(dist) + zeta; //TODO implement zeta better with randn, PL = alpha + 10 beta log10(||distance||) + zeta, 
    double plg = m_TxP_dBm + def_G_max + def_G_max - path_loss_db; // S = P_transmit * G_max * G_max / pathloss
    
    return dBm_to_watt(plg);
}

double mmWaveBS::calculate_Rate_of_link(double x, double y)
{
    double snr = calculate_SNR_of_link(x, y);
    double rate = def_BW * log2(1+snr);
    return rate;
}

double mmWaveBS::calculate_distance_of_link(double x, double y)
{
    double dist = euclidean_dist(x, y, getX(), getY());
    return dist;
}

void mmWaveBS::emit_update_parent()
{
    update_parent_msg msg(getID(), m_hop_cnt, getX(), getY());
//     if(m_hop_cnt==-1)
//         std::cout << "here";
    update_parent.emit(msg);
}

void mmWaveBS::set_backhaul_Type(Backhaul st)
{
    m_bkhl = st;
//     if (st==Backhaul::wired)
//         set_hop_count(0);
}

void mmWaveBS::add_to_load_BS(boost::shared_ptr<mmWaveBS> bs/*uint32_t id, point p*/)
{
    bool found = false;//= m_load_BS.find(id)!=m_load_BS.end();
    
    for(std::vector<boost::shared_ptr<mmWaveBS>>::iterator it=m_load_BS.begin(); it!=m_load_BS.end();++it)
    {
        boost::shared_ptr<mmWaveBS> mmB = (*it);
        if(mmB->getID()==bs->getID())
        {found = true; break;}
    }
    
    if(!found)
    {
        m_load_BS.push_back(bs);
    }
    
}

void mmWaveBS::remove_from_load_BS(uint32_t id)
{
//     bool found = m_load_BS.find(id)!=m_load_BS.end();
//     
//     if(found)
//         m_load_BS.erase(id);
//     else
//         std::cout << "BS not found to be deleted!" << std::endl;
}

int mmWaveBS::get_load_BS_count()
{
    int s = m_load_BS.size(); 
    return s;
}

void mmWaveBS::update_load_hops()
{
    int hop = get_hop_count()+1;
    for(std::vector<boost::shared_ptr<mmWaveBS>>::iterator it=m_load_BS.begin(); it!=m_load_BS.end();++it)
    {
        boost::shared_ptr<mmWaveBS> mmB = (*it);
//         if(mmB->get_parent()!=getID())
//             std::cout << "parent=" << getID() << ", IAB_parent=" << mmB->get_parent() << "my id=" << mmB->getID()<< "\n";
        if(mmB->get_hop_count()==-1)
        {
            mmB->set_hop_count(hop);
            mmB->route_found(true);
        }
//         mmB->update_load_hops(mmB->get_hop_count());
    }
}

void mmWaveBS::reset_load()
{
    int n = m_load_BS.size();
    for(int i=0;i<n;++i)
        m_load_BS.pop_back();
}
