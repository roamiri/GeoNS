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

#include "iabn.h"
#include "plotter.h"


// using namespace boost::numeric::ublas;

namespace po = boost::program_options;
using namespace std;

int main(int argc, char** argv)
{
    auto start = std::chrono::high_resolution_clock::now();
    //Fixed wired Base stations
    bool fixed = false;
    int fixed_count = -1;
    Path_Policy policy = Path_Policy::WF;
    double node_density = def_lambda_SBS;
    double wired_fractoin = def_Wired_fraction;
    bool b_verbose = false;
    int Total_iter = 100;
    bool bPlot = false;
    std::string svg_name = "network.svg";
    bool b_draw = false;
    
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Construct your network based on the following inputs.")
        ("dens", po::value<double>() ,"Density of the base stations, preference=1e-4 BS/1km2")
        ("fixed", po::value<int>(), "Set wired BS implementation type (fixed(1) or variable(0))")
        ("count", po::value<int>(), "Number of fixed wired base stations")
        ("prob", po::value<double>(), "Wired nodes probability")
        ("policy", po::value<int>(), "Path Selection Policy, options = HQF, WF, PA, MLR, HQIF")
        ("if", po::value<string>(), "Input file name")
        ("of", po::value<string>(), "Output file name")
        ("svg", po::value<string>(), "Topology SVG file name")
        ("iter", po::value<int>(), "Number of iterations")
        ("verbose", po::value<bool>(), "verbose")
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
        node_density = vm["dens"].as<double>();
    
    if(vm.count("prob"))
        wired_fractoin = vm["prob"].as<double>();
        
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
            case(2):
                policy = Path_Policy::PA;
                break;
            case(3):
                policy = Path_Policy::MLR;
                break;
            case(4):
                policy = Path_Policy::HQIF;
                break;
        }
    }
    
    if(vm.count("verbose"))
        b_verbose = vm["verbose"].as<bool>();
        
    
    std::string plot_name = "Hop_Count";
    if(vm.count("of"))
    {
        plot_name = vm["of"].as<string>();
        bPlot = true;
    }
    
    std::string input;
    bool b_input = false;
    if(vm.count("if"))
    {
        input = vm["if"].as<string>();
        b_input = true;
    }
    
    if(vm.count("svg"))
    {
        svg_name = "SVG_"+vm["svg"].as<string>()+".svg";
        b_draw = true;
    }
    
//     std::shared_ptr<IDGenerator> _idGenerator = ;
    
    IABN iabn;
    m_nextId = 0; //TODO fix the id generator 
    iabn.create_svg(svg_name);
//     std::shared_ptr<Painter> _painter = std::make_shared<Painter>(manager.m_items);
//     _painter.get()->Start();
   
//     // Generate data on a disk with radius r with poisson point process    
//     double area = def_Area*node_density;
//     double r = sqrt(1/M_PI)*sqrt(area); // radius of the disk
//     double xx0=r; double yy0=r;    // center of the disk
//     iabn.set_center(xx0, yy0, r);
    
    if(b_input)
        iabn.load_nodes(input, fixed, fixed_count, wired_fractoin);
    else
        iabn.generate_nodes(node_density, fixed, fixed_count, wired_fractoin);
    
    int Total_fail = 0;
    
//     int CDF_Hop_vec[10] = {0};
    boost::numeric::ublas::vector<double> CDF_Hop_vec(15);
    for(int i=0;i<15;i++) CDF_Hop_vec(i)=0;
    
    std::vector<double> vec_SNR_bottleneck;
    std::vector<double> vec_SINR_bottleneck;
    
    int dm = iabn.get_wired_count() + iabn.get_IAB_count(); 
    std::cout << "Number of nodes = " << dm << std::endl;
    
    boost::progress_display show_progress(Total_iter);
    for(int iter=0;iter<Total_iter;iter++)
    {       
       if(b_verbose)
       { 
            std::cout << "Number of Wired nodes = " << iabn.get_wired_count() << std::endl;
            std::cout << "Number of IAB nodes = " << iabn.get_IAB_count() << std::endl;
       }

        // Path Selection
        switch(policy)
        {
            case(Path_Policy::HQF):
                iabn.path_selection_HQF();
                if(b_verbose) std::cout << "Selecting Parents with HQF." << std::endl;
                break;
            
            case(Path_Policy::WF):
                iabn.path_selection_WF();
                if(b_verbose) std::cout << "Selecting Parents with WF." << std::endl;
                break;
                
            case(Path_Policy::PA):
                iabn.path_selection_PA();
                if(b_verbose) std::cout << "Selecting Parents with PA." << std::endl;
                break;
                
            case(Path_Policy::MLR):
                iabn.path_selection_MLR();
                if(b_verbose) std::cout << "Selecting Parents with MLR." << std::endl;
                break;
            case(Path_Policy::HQIF):
                iabn.path_selection_HQF_Interf();
                if(b_verbose) std::cout << "Selecting Parents with HQIF." << std::endl;
                break;
        }
        
        iabn.spread_hop_count();
        
        vec_SNR_bottleneck.push_back(iabn.find_SNR_bottleneck());
        vec_SINR_bottleneck.push_back(iabn.find_SINR_bottleneck());
//         manager.set_hop_counts();
//         std::cout << __FUNCTION__<< __LINE__ << std::endl;

//         int hop_vec[15] = {0};
        int failed = 0;
        int max_hop = 0;
        std::vector<int> hop_vec = iabn.count_hops(max_hop, failed);
//         std::cout << __FUNCTION__<< __LINE__ << std::endl;
        iabn.draw_svg(b_draw);
        
//         int total_hops = 0;
        if(CDF_Hop_vec.size() < max_hop+1)
            CDF_Hop_vec.resize(max_hop+1);
        
        for(int i=0;i<max_hop+1;i++)
        {
            CDF_Hop_vec(i)+=hop_vec[i];
        }
        Total_fail+= failed;
        
//         std::cout << __FUNCTION__<< __LINE__ << std::endl;
        //TODO 
        iabn.update_locations(fixed, wired_fractoin);
//         std::cout << CDF_Hop_vec << std::endl;        
        ++show_progress;
    }
    
    
    std::cout << CDF_Hop_vec << std::endl;
    CDF_Hop_vec(0)=0;
    CDF_Hop_vec =  (1./Total_iter)*CDF_Hop_vec;
//     std::cout << CDF_Hop_vec << std::endl;
    for(int i= 1; i< CDF_Hop_vec.size(); ++i)
    {
        CDF_Hop_vec[i]+=CDF_Hop_vec[i-1];
    }
    std::cout << "Failed = " << ((double)Total_fail/Total_iter) << std::endl;
    std::cout << "Succeeded = " << CDF_Hop_vec[CDF_Hop_vec.size()-1] << std::endl;
    double num_nodes = CDF_Hop_vec[CDF_Hop_vec.size()-1] + ((double)Total_fail/Total_iter);
    CDF_Hop_vec = (1./(num_nodes)) * CDF_Hop_vec;
    std::cout << CDF_Hop_vec << std::endl;
    std::string name = plot_name;
    name = name + ".txt";
    save1DArrayasText(CDF_Hop_vec, CDF_Hop_vec.size(), name);
    save1DArrayasText(vec_SNR_bottleneck, vec_SNR_bottleneck.size(), name, "SNR");
    save1DArrayasText(vec_SINR_bottleneck, vec_SINR_bottleneck.size(), name, "SINR");
//     save1DArrayasText(vec_SNR_bottleneck, vec_SNR_bottleneck.size(), name);
    
    if(bPlot)
    {
        plotter* plot = new plotter();
        std::string legend; 
        std::string plot1 = "Hop_"+plot_name+".eps";
        std::string plot2 = "SNR_"+plot_name+".eps";
//         std::string plot2 = "SINR_"+plot_name;
        switch(policy)
        {
            case(Path_Policy::HQF):
                legend = "High Quality First";
                break;
            case(Path_Policy::WF):
                legend = "Wired First";
                break;
            case(Path_Policy::PA):
                legend = "Position Aware";
                break;
            case(Path_Policy::MLR):
                legend = "Maximum Local Rate";
                break;
            case(Path_Policy::HQIF):
                legend = "High SINR First";
                break;
        }
        
        plot->plot1DArray(boostVtoStdV(CDF_Hop_vec), plot1, legend, std::string("Number of hops"), std::string("CDF"));
//         plot->closePlot();
        std::vector<std::vector<double>> cdfs; 
        std::vector<std::string> legends = {"SNR", "SINR"};
        std::vector<std::string> colors = {"b", "r"};
        cdfs.push_back(vec_SNR_bottleneck); cdfs.push_back(vec_SINR_bottleneck);
        
//        plot->plot_CDF_Array(cdfs, plot2, legends, std::string("SNR"), std::string("CDF"), colors);
//         plot->plot_CDF_Array(vec_SINR_bottleneck, plot2, "SINR", std::string("SINR"), std::string("CDF"), "r");
//         plot->closePlot();
    }
    
    iabn.reset_pointers();
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    

    std::cout << "time = "<< elapsed.count() <<"Get out of my Audio, Adios!!"<< std::endl;
    
    return 0;
}
