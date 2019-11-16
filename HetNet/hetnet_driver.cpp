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

// #include "plotter.h"

#include "hetnet.h"

namespace po = boost::program_options;
using namespace std;

int main(int argc, char** argv)
{
    bool fixed = false;
    int fixed_count = -1;
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
    
    HetNet network;
    m_nextId = 0;
    
    // Generate data on a disk with radius r with poisson point process    
    double r = sqrt(1/M_PI)*sqrt(def_Area); // radius of the disk
    double xx0=r; double yy0=r;    // center of the disk
    network.set_center(xx0, yy0, r);
    
    if(false)
        network.load_nodes(input, fixed, fixed_count, wired_fractoin);
    else
        network.generate_nodes(node_density, fixed, fixed_count, wired_fractoin);
    
    cout << "HELLO!!" << endl;
    return 0;
}
