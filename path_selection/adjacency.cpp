
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
    bool b_draw = false;
    double area_coeff = 1.0;
    bool b_tree = true;
    double wired_fractoin = def_Wired_fraction;
    //Fixed wired Base stations
    bool fixed = false;
    int fixed_count = -1;
    double node_density = def_lambda_SBS;
    
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Construct your network based on the following inputs.")
        ("area", po::value<double>() ,"Area coefficient, Area=coeff*1.e6")
        ("tree", po::value<bool>() ,"Use R-tree for spatial indexing (1) or array (0)")
        ("if", po::value<string>(), "Input file name")
        ("fixed", po::value<int>(), "Set wired BS implementation type (fixed(1) or variable(0))")
        ("count", po::value<int>(), "Number of fixed wired base stations")
        ("svg", po::value<string>(), "Topology SVG file name")
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
    
    std::string input;
    bool b_input = false;
    if(vm.count("if"))
    {
        input = vm["if"].as<string>();
        b_input = true;
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
    
    if(vm.count("svg"))
    {
        svg_name = "SVG_"+vm["svg"].as<string>()+".svg";
        b_draw = true;
    }
    
    IABN iabn;
    m_nextId = 0; //TODO fix the id generator 
    iabn.create_svg(svg_name);
    
// Generate data on a disk with radius r with poisson point process    
    double area = def_Area*node_density;
    double r = 100*sqrt(1/M_PI)*sqrt(area); // radius of the disk
    double xx0=r; double yy0=r;    // center of the disk
    iabn.set_center(xx0, yy0, r);
    
    if(b_input)
        iabn.load_nodes(input, fixed, fixed_count, wired_fractoin);
    else
        iabn.generate_nodes(node_density, fixed, fixed_count, wired_fractoin);
    
    std::cout << "IAB node count = " << iabn.get_IAB_count() << std::endl;
    std::cout << "Wired node count = " << iabn.get_wired_count() << std::endl;
    matrix<double> matrix_SINR = iabn.Generate_Matrix_SINR();
    
//     std::cout << matrix_SINR << std::endl;
    
    
    iabn.draw_svg(b_draw);
//     if(b_tree)
//         iabn.check_SINR_tree();
//     else
//         iabn.check_SINR_array();
    
//     finish = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double> elapsed = finish - start;
//     std::cout << "search = "<< elapsed.count() - loading_time.count() << "," << std::endl;
//     
//     std::cout << "computation = "<< elapsed.count() << "," << std::endl;
    
    std::cout << "Get out of my Audio, Adios!!" << std::endl;

    
//     std::cout << "ratio = " << loading_time.count()/elapsed.count() << "," << std::endl;
    
    return 0;
}
