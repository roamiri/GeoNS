
#include "Signal.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>
#include <string>
#include <random>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/progress.hpp>
#include <boost/program_options.hpp>


#include "plotter.h"
#include "painter.h"
#include "container.cpp"
#include "wsensor.h"


namespace po = boost::program_options;
using namespace std;

int main(int argc, char** argv)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    std::string svg_name = "WSN.svg";
    bool b_input = false;
    std::string input;
    double node_density = def_lambda_SBS;
    double wired_fractoin = 0.0;
    bool b_verbose = false;
    int Total_iter = 100;
    bool bPlot = false;
    bool b_draw = false;
    
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "Construct your network based on the following inputs.")
    ("dens", po::value<double>() ,"Density of the base stations, preference=1e-4 BS/1km2")
    ("if", po::value<string>(), "Input file name")
    ("of", po::value<string>(), "Output file name")
    ("svg", po::value<string>(), "Topology SVG file name")
    ("iter", po::value<int>(), "Number of iterations")
    ("verbose", po::value<bool>(), "verbose")
    ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if(vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }
    
    if(vm.count("iter"))
        Total_iter = vm["iter"].as<int>();
    
    if(vm.count("dens"))
        node_density = vm["dens"].as<double>();
    
    if(vm.count("verbose"))
        b_verbose = vm["verbose"].as<bool>();
        
    
    std::string plot_name = "Hop_Count";
    if(vm.count("of"))
    {
        plot_name = vm["of"].as<string>();
        bPlot = true;
    }
    
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
    
    
    Container<wSensor> WSN(svg_name);
    
    // Generate data on a disk with radius r with poisson point process    
    double r = sqrt(1/M_PI)*sqrt(def_Area); // radius of the disk
    double xx0=r; double yy0=r;    // center of the disk
    WSN.set_center(xx0, yy0, r);
    
    if(b_input)
        WSN.load_nodes(input, false, 0, wired_fractoin);
    else
        WSN.generate_nodes(node_density, false, 0, wired_fractoin);
    
    WSN.draw_svg(b_draw);
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "time = "<< elapsed.count() <<"Get out of my Audio, Adios!!"<< std::endl;
    
    return 0;
}
