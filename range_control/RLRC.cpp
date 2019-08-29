
// Range control in wireless communication with reinforcement learning

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <chrono>


#include "rlnetwork.h"
#include "common.h"


namespace po = boost::program_options;
using namespace std;


int main(int argc, char** argv)
{
    
    string svg_name = "test";
    bool bdraw = false;
    bool b_input = false;
    string i_file;
    double node_desnity = def_lambda_SBS;
    int nb_episode = 100;
    uint32_t Q_id = 1;
    bool bQ = false;
    
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Use the following flags to input the required parameters.")
        ("if", po::value<string>(), "input file")
        ("svg", po::value<string>(), "Topology SVG output file name")
        ("episode", po::value<double>(), "Number of training episodes")
        ("Q", po::value<uint32_t>(), "node id to print Q-function in softmax")
        ;
        
        
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if(vm.count("help"))
    {
        cout << desc << endl;
        return 1;
    }
    
    if(vm.count("svg"))
    {
        svg_name = vm["svg"].as<string>()+".svg";
        bdraw = true;
    }
    
    if(vm.count("if"))
    {
        b_input = true;
        i_file = vm["if"].as<string>();
    }
    
    if(vm.count("episode"))
    {
        nb_episode = vm["episode"].as<double>();
    } 
    
    if(vm.count("Q"))
    {
        Q_id = vm["Q"].as<uint32_t>();
        bQ = true;
    }
    
    auto start = chrono::high_resolution_clock::now();
    
    RLNetwork* global_ENV = new RLNetwork();
    m_nextId = 0;
    global_ENV->create_svg(svg_name);
    double r = sqrt(def_Area/M_PI);
    global_ENV->set_center(r,r,r);
    
    if(b_input)
        global_ENV->load_nodes(i_file, false, 0, 0.);
    else
        global_ENV->generate_nodes(node_desnity, false, 0, 0.);
    
    global_ENV->train(nb_episode);
    
    while(!global_ENV->isNetworkReady())
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds(10));
    }
    global_ENV->k_connect(10);
    
    global_ENV->draw_neighbors(bdraw);
    
    if(bQ) global_ENV->print_Q_function(Q_id, O_POLICY::softmax);
    
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish-start;
    
    cout << "Duration time = " << elapsed.count() << " seconds for " << global_ENV->node_count() << " nodes!" << endl;
    
    //TODO not sure about this function!
    global_ENV->reset_pointers();
    return 0;
}
