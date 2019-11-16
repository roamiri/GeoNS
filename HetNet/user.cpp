
#include "user.h"

User::User(double x, double y, uint32_t id, Status st, typ ss)
: TRX((float)x,(float)y,1.,1.,id, ss)
{
    m_phi_m = def_Phi_m;
    m_route_SNR = 0.; 
    m_route_SINR = 0.;
}

User::~User()
{
    stop_thread = true;
	if(the_thread.joinable()) 
		the_thread.join();
}

void User::reset()
{
    set_SNR(0.);
    set_SINR(0.);
}

void User::Start()
{
    // This will start the thread. Notice move semantics!
    the_thread = std::thread(&User::ThreadMain,this);
    
    char thread_name [20];
    sprintf(thread_name, "UE_%d",getID());
    prctl(PR_SET_NAME,thread_name,0,0,0);
    
    char thread_name_buffer[20];
    prctl(PR_GET_NAME, thread_name_buffer, 0L, 0L, 0L);
}


void User::ThreadMain()
{
    while(!stop_thread)
    {
        // Time interval to avoid overwhelming the thread!
//         srand(time(NULL));
        double p = ((double)rand()/(double)(RAND_MAX));
    }
}


double User::calculate_SNR_of_link(double x, double y)
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
double User::calculate_SINR_of_link(double x, double y, double interf)
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

double User::calculate_Interf_of_link(double x, double y)
{
    // SINR = S/(I+N)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(0., sqrt(def_backhaul_zeta_los));
    
    std::default_random_engine generator;
    std::uniform_real_distribution<double> dist_uniform(0.0, 360.0);
    
    double dist = euclidean_dist(x, y, (double) getX(), (double) getY());
    double zeta = dis(gen);
    double path_loss_db = def_beta + 10.* def_backhaul_alpha_los * log10(dist) + zeta; //TODO implement zeta better with randn, PL = alpha + 10 beta log10(||distance||) + zeta,
    double dum = dist_uniform(generator);
    double plg;
    if(dum<=m_phi_m)
        plg = m_TxP_dBm + def_G_max + def_G_max - path_loss_db; // S = P_transmit * G_max * G_max / pathloss
    else
        plg = m_TxP_dBm + def_G_max + def_G_min - path_loss_db; // S = P_transmit * G_max * G_min / pathloss
    return dBm_to_watt(plg);
}

double User::calculate_Rate_of_link(double x, double y)
{
    double snr = calculate_SNR_of_link(x, y);
    double rate = def_BW * log2(1+snr);
    return rate;
}

double User::calculate_distance_of_link(double x, double y)
{
    double dist = euclidean_dist(x, y, getX(), getY());
    return dist;
}
