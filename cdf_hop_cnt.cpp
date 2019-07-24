#include "Signal.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <sys/prctl.h>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/progress.hpp>
#include <boost/program_options.hpp>

#include "manager.h"
#include "mmwavebs.h"
#include "idgenerator.h"
#include "painter.h"
#include "ppunfix5.h"
#include "plotter.h"

// using namespace boost::numeric::ublas;

namespace po = boost::program_options;
using namespace std;

int main(int argc, char** argv)
{
    //Fixed wired Base stations
    bool fixed = false;
    int fixed_count = -1;
    Path_Policy policy = Path_Policy::WF;
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Input  1 for fixed locations of the fiber base stations or 0 for variable")
        ("fixed", po::value<int>(), "Set wired BS implementation type (fixed or variable)")
        ("count", po::value<int>(), "Number of fixed wired base stations")
        ("policy", po::value<int>(), "Path Selection Policy, options = HQF, WF")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) 
    {
        std::cout << desc << "\n";
        return 1;
    }

    if (vm.count("fixed")) 
    {
        cout << "Fixed wired Base Stations is set to "  << vm["fixed"].as<int>() << ".\n";
        int dd = vm["fixed"].as<int>();
        if(dd==1)
            fixed = true;
        else
            fixed = false;
        
    } else {
        cerr << "Input Arg Error: set the type of wired base station implementation.\n";
        return 1;
    }
    if(vm.count("count"))
        fixed_count = vm["count"].as<int>();
    else
        fixed_count = 4;
    
    if(vm.count("policy"))
    {
        int pp = vm["policy"].as<int>();
        switch(pp)
        {
            case(0):
                policy = Path_Policy::HQF;
                break;
            case(1):
                policy = Path_Policy::WF;
                break;
        }
    }
        
    
    IDGenerator* _idGenerator = IDGenerator::instance();
    Manager manager;
    
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    
    std::mt19937 gen_IAB(rd());
    std::uniform_real_distribution<> dis1(0, 1);
    
    // Generate data on a disk with radius r with poisson point process    
    double r = sqrt(1/M_PI)*1000.; // radius of disk
    double xx0=r; double yy0=r;    // centre of disk
        
    //   Create the nodes
    std::poisson_distribution<int> pd(1e6*lambda_SBS);
    int num_nodes=pd(gen_IAB); // Poisson number of points
    std::cout << "Number of IAB nodes = " << num_nodes << std::endl;
//     int num_nodes = 100; // Poisson number of points
	for(int i =0;i<num_nodes;i++)
	{
        std::shared_ptr<mmWaveBS> BS;
        BS = std::make_shared<mmWaveBS>(_idGenerator->next(),  def_P_tx);
        BS.get()->setColor(0);
        manager.m_vector_BSs.push_back(BS);
        BS.get()->update_parent.connect_member(&manager, &Manager::listen_For_parent_update);
    }
    
    int Total_iter = 10;
    int Total_fail = 0;
    
//     int CDF_Hop_vec[10] = {0};
    boost::numeric::ublas::vector<double> CDF_Hop_vec(10);
    
    boost::progress_display show_progress(Total_iter);
    for(int iter=0;iter<Total_iter;iter++)
    {        
//         double data[num_nodes][2];
//         
//         for(int i=0;i<num_nodes;i++)
//         {
//             double theta=2*M_PI*(dis(gen));   // angular coordinates
//             double rho=r*sqrt(dis(gen));      // radial coordinates
//             
//             data[i][0] = xx0 + rho * cos(theta);  // Convert from polar to Cartesian coordinates
//             data[i][1] = yy0 + rho * sin(theta);
//         }
//          
//         int cnt = 0;
        for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=manager.m_vector_BSs.begin(); it!=manager.m_vector_BSs.end();++it)
        {
            std::shared_ptr<mmWaveBS> mmB = (*it);
            
            if(fixed)
            {
                double theta=2*M_PI*(dis1(gen_IAB));   // angular coordinates
                double rho=r*sqrt(dis1(gen_IAB));      // radial coordinates
                
                double x = xx0 + rho * cos(theta);  // Convert from polar to Cartesian coordinates
                double y = yy0 + rho * sin(theta);
                mmB->setX(x);
                mmB->setY(y);
                mmB->set_backhaul_Type(Backhaul::IAB);
            }
            else
            {
                bool prob = (rand() % 100) < 100*def_prob_Wired;
                if(prob)
                {
                    double theta=2*M_PI*(dis(gen));   // angular coordinates
                    double rho=r*sqrt(dis(gen));      // radial coordinates
                    
                    double x = xx0 + rho * cos(theta);  // Convert from polar to Cartesian coordinates
                    double y = yy0 + rho * sin(theta);
                    mmB->setX(x);
                    mmB->setY(y);
                    mmB->set_backhaul_Type(Backhaul::wired);
                }
                else
                {
                    double theta=2*M_PI*(dis1(gen_IAB));   // angular coordinates
                    double rho=r*sqrt(dis1(gen_IAB));      // radial coordinates
                    
                    double x = xx0 + rho * cos(theta);  // Convert from polar to Cartesian coordinates
                    double y = yy0 + rho * sin(theta);
                    mmB->setX(x);
                    mmB->setY(y);
                    mmB->set_backhaul_Type(Backhaul::IAB);
                }
                
            }   
                    
        }
        

        // Adding Fixed Wired BSs
        if(fixed)
        {
            double delta_teta = 2*M_PI/fixed_count;
            for(int i =0;i<fixed_count;i++)
            {
                std::shared_ptr<mmWaveBS> BS;
                BS = std::make_shared<mmWaveBS>(_idGenerator->next(),  def_P_tx);
                BS.get()->setColor(0);
                double theta = i*delta_teta;
                double r2 = r/2.;
                double x = xx0 + r2 * cos(theta);  // Convert from polar to Cartesian coordinates
                double y = yy0 + r2 * sin(theta);
                BS->setX(x);
                BS->setY(y);
                BS->set_backhaul_Type(Backhaul::wired);
                manager.m_vector_BSs.push_back(BS);
                BS.get()->update_parent.connect_member(&manager, &Manager::listen_For_parent_update);
            }
        }
        
        
        // Path Selection
        switch(policy)
        {
            case(Path_Policy::HQF):
                manager.path_selection_HQF();
                break;
            
            case(Path_Policy::WF):
                manager.path_selection_WF();
                break;
        }
    
        manager.set_hop_counts();
        
        int hop_vec[10] = {0};
        int failed = 0;
        for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=manager.m_vector_BSs.begin(); it!=manager.m_vector_BSs.end(); ++it)
        {
            std::shared_ptr<mmWaveBS> mmB = (*it);
            int hcnt = mmB->get_hop_count();
            if(hcnt != -1)
                hop_vec[hcnt]++;
            else
                failed++;
    //         std::cout << "SBS " << mmB->getID() << " hop count = " << mmB->get_hop_count() << std::endl;
        }
        
//         int total_hops = 0;
        for(int i=0;i<10;i++)
        {
//             total_hops+=i*hop_vec[i];
            CDF_Hop_vec(i)+=hop_vec[i];
        }
        Total_fail+= failed;
    //     std::cout << "Total hops = " << total_hops << ", number fails = " << failed << std::endl;
        
        for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=manager.m_vector_BSs.begin(); it!=manager.m_vector_BSs.end(); ++it)
        {
            std::shared_ptr<mmWaveBS> mmB = (*it);
            mmB->reset();
        }
        
        ++show_progress;
    }
    
    CDF_Hop_vec =  (1./Total_iter)*CDF_Hop_vec;
    for(int i= 1; i< CDF_Hop_vec.size(); ++i)
    {
        CDF_Hop_vec[i]+=CDF_Hop_vec[i-1];
    }
    double tt = (double)Total_fail/Total_iter;
    CDF_Hop_vec = (1./(num_nodes)) * CDF_Hop_vec;
    std::cout << CDF_Hop_vec << std::endl;
    std::string name = "CDF_Hop.txt";
    save1DArrayasText(CDF_Hop_vec, 10, name);
    plotter* plot = new plotter();
    plot->plot1DArray(boostVtoStdV(CDF_Hop_vec), std::string("CDF_Hop.jpg"), std::string("Wired First"), std::string("Number of hops"), std::string("CDF"));
    
    
//     std::cout << "Total hops = " << CDF_Hop_vec << ", number fails = " << Total_fail << std::endl;
    
    return 0;
}
