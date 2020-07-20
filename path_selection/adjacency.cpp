
#include <iostream>
#include <chrono>
#include <boost/progress.hpp>
#include <boost/program_options.hpp>

#include "iabn.h"

namespace po = boost::program_options;
using namespace std;

int main(int argc, char** argv)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    std::string svg_name = "network.svg";
    double area_coeff = 1.0;
    bool b_tree = true;
    
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Construct your network based on the following inputs.")
        ("area", po::value<double>() ,"Area coefficient, Area=coeff*1.e6")
        ("tree", po::value<bool>() ,"Use R-tree for spatial indexing (1) or array (0)")
        ;
        
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }
    
    if(vm.count("area"))
        area_coeff = vm["area"].as<double>();
    
    if(vm.count("tree"))
        b_tree = vm["tree"].as<bool>();
    
    IABN iabn;
    m_nextId = 0; //TODO fix the id generator 
    iabn.create_svg(svg_name);
    
    iabn.generate_nodes(area_coeff, b_tree);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> loading_time = finish - start;
    std::cout << "load = " << loading_time.count() << "," << std::endl;
    
//     if(b_tree)
//         iabn.check_SINR_tree();
//     else
//         iabn.check_SINR_array();
    
    finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "search = "<< elapsed.count() - loading_time.count() << "," << std::endl;
    
    std::cout << "computation = "<< elapsed.count() << "," << std::endl;
    
    std::cout << "Get out of my Audio, Adios!!" << std::endl;

    
//     std::cout << "ratio = " << loading_time.count()/elapsed.count() << "," << std::endl;
    
    return 0;
}
