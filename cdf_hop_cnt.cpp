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
#include "painter.h"
// #include "ppunfix5.h"
#include "plotter.h"
// #include "idgenerator.h"

// using namespace boost::numeric::ublas;

namespace po = boost::program_options;
using namespace std;

int main(int argc, char** argv)
{
    //Fixed wired Base stations
    bool fixed = false;
    int fixed_count = -1;
    Path_Policy policy = Path_Policy::WF;
    double wired_density = def_prob_Wired;
    bool debg = false;
    int Total_iter = 100;
    bool bPlot = true;
    
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Input  1 for fixed locations of the fiber base stations or 0 for variable")
        ("iter", po::value<int>(), "Number of iterations")
        ("fixed", po::value<int>(), "Set wired BS implementation type (fixed or variable)")
        ("count", po::value<int>(), "Number of fixed wired base stations")
        ("dens", po::value<double>(), "Wired nodes density")
        ("policy", po::value<int>(), "Path Selection Policy, options = HQF, WF")
        ("verbose", po::value<bool>(), "verbose")
        ("plot", po::value<bool>(), "Plot the CDF of the policy")
        ("file_name", po::value<string>(), "file name")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) 
    {
        std::cout << desc << "\n";
        return 1;
    }
    
    if(vm.count("iter"))
        Total_iter = vm["iter"].as<int>();

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
    
    if(vm.count("dens"))
        wired_density = vm["dens"].as<double>();
        
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
    
    if(vm.count("verbose"))
        debg = vm["verbose"].as<bool>();
        
    if(vm.count("plot"))
        bPlot = vm["plot"].as<bool>();
    
    std::string plot_name = "Hop_Count";
    if(vm.count("file_name"))
        plot_name = vm["file_name"].as<string>();
    
//     std::shared_ptr<IDGenerator> _idGenerator = ;
    
    Manager manager;
    m_nextId = 0; //TODO fix the id generator 
    
//     std::shared_ptr<Painter> _painter = std::make_shared<Painter>(manager.m_vector_BSs);
//     _painter.get()->Start();
    
    // Generate data on a disk with radius r with poisson point process    
    double r = sqrt(1/M_PI)*sqrt(def_Area); // radius of disk
    double xx0=r; double yy0=r;    // centre of disk
    manager.set_center(xx0, yy0, r);
    
    manager.generate_nodes(fixed, fixed_count, wired_density);
    
    
    int Total_fail = 0;
    
//     int CDF_Hop_vec[10] = {0};
    boost::numeric::ublas::vector<double> CDF_Hop_vec(10);
    for(int i=0;i<10;i++) CDF_Hop_vec(i)=0;
    
    
    boost::progress_display show_progress(Total_iter);
    for(int iter=0;iter<Total_iter;iter++)
    {

       
       if(debg){ 
            std::cout << "Number of Wired nodes = " << manager.get_wired_count() << std::endl;
            std::cout << "Number of IAB nodes = " << manager.get_IAB_count() << std::endl;
       }
        // Path Selection
        switch(policy)
        {
            case(Path_Policy::HQF):
                manager.path_selection_HQF();
                if(debg) std::cout << "Selecting Parents with HQF." << std::endl;
                break;
            
            case(Path_Policy::WF):
                manager.path_selection_WF();
                if(debg) std::cout << "Selecting Parents with WF." << std::endl;
                break;
        }
        
        manager.set_hop_counts();
//         _painter->Enable();
        
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
        
        //TODO 
        manager.update_locations(fixed, wired_density);
        
//         std::cout << "tree size = " << manager.tree_size(1000) << std::endl;
        ++show_progress;
//         std::cout << CDF_Hop_vec << std::endl;
    }
    CDF_Hop_vec(0)=0;
    CDF_Hop_vec =  (1./Total_iter)*CDF_Hop_vec;
    for(int i= 1; i< CDF_Hop_vec.size(); ++i)
    {
        CDF_Hop_vec[i]+=CDF_Hop_vec[i-1];
    }
//     double tt = (double)Total_fail/Total_iter;
//     std::cout << CDF_Hop_vec << std::endl;
    int num_nodes = manager.get_IAB_count();
    CDF_Hop_vec = (1./(num_nodes)) * CDF_Hop_vec;
    std::cout << CDF_Hop_vec << std::endl;
    std::string name = plot_name;
    name = name + ".txt";
    save1DArrayasText(CDF_Hop_vec, 10, name);
    
    if(bPlot)
    {
        plotter* plot = new plotter();
        std::string fig_name;
        if(policy==Path_Policy::HQF)
            fig_name = "High Quality First";
        else if(policy==Path_Policy::WF)
            fig_name = "Wired First";
        
        plot->plot1DArray(boostVtoStdV(CDF_Hop_vec), plot_name, fig_name, std::string("Number of hops"), std::string("CDF"));
    }
    
//     std::cout << "Total hops = " << CDF_Hop_vec << ", number fails = " << Total_fail << std::endl;
    
    return 0;
}
