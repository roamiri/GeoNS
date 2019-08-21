
#include "rlnetwork.h"

RLNetwork::RLNetwork(std::string svg_name)
:Container(svg_name)
{

}

RLNetwork::~RLNetwork()
{

}


void RLNetwork::generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin)
{
//     if(fixed) generate_fixed_nodes(fixed_count);
    
    //   Create the nodes
    std::uniform_real_distribution<> dis(0, 1);
    std::poisson_distribution<int> pd(1e6*node_density);
    int num_nodes=pd(m_generator); // Poisson number of points
    //     int num_nodes = 100; // Poisson number of points
    
    for(int i =0;i<num_nodes;i++)
    {
        double theta=2*M_PI*(dis(m_generator));   // angular coordinates
        double rho=radius*sqrt(dis(m_generator));      // radial coordinates
        
        double x = center_x + rho * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + rho * sin(theta);
        
        bool vicinity = check_neighbors(x,y);
        
        if(vicinity)
        {
            rl_ptr BS;
            BS = boost::shared_ptr<RLAgent>(new RLAgent(x,y,get_nextID()));
            BS->set_transmit_power(def_P_tx);
            if(!BS) std::cerr << __FUNCTION__ << std::endl;
            BS.get()->setColor(0);
            m_tree.insert(std::make_pair(BS->get_loc(), BS)); //TODO maybe here!
            m_items.push_back(BS);
//             BS.get()->update_parent.connect_member(this, &IABN::listen_For_parent_update);
//             BS.get()->Start();
// 			BS.get()->candidacy.connect_member(&manager, &IABN::listen_For_Candidacy);
// 			BS.get()->clusterHead.connect_member(&manager, &IABN::listen_For_ClusterHead);
// 			BS.get()->conflict.connect_member(&manager, &IABN::listen_For_Conflict);
        }
    }
}

void RLNetwork::load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin)
{
    int num_nodes =0;
// 	double data[num_nodes][2];
    std::vector<std::array<double,2>> vec_data;
	std::fstream _file(f_name);

	if(_file.fail())
    {
		std::cerr << "input data file does not exis!" << std::endl;
    }
    std::string line;
	while(std::getline(_file, line) && num_nodes<200)
	{
		std::istringstream is(line);
		double num;
        std::array<double,2> arr;
        is >> arr[0];
        is>>arr[1];
        vec_data.push_back(arr);
        num_nodes++;
	}
	
	
// 	if(fixed) generate_fixed_nodes(fixed_count);
    
    //   Create the nodes
    
    num_nodes= vec_data.size(); // number of points
    
    for(int i =0;i<num_nodes;i++)
    {
        double x = center_x + 1.5*radius* vec_data[i][0];  // Convert from polar to Cartesian coordinates
        double y = center_y + 1.5*radius* vec_data[i][1];
        
        bool vicinity = check_neighbors(x,y);
        
        if(vicinity)
        {
            rl_ptr BS;
            BS = boost::shared_ptr<RLAgent>(new RLAgent(x,y, get_nextID()));
            BS->set_transmit_power(def_P_tx);
            if(!BS) std::cerr << __FUNCTION__ << " at " << __LINE__ <<std::endl;
            BS.get()->setColor(0);
            m_tree.insert(std::make_pair(BS->get_loc(), BS)); //TODO maybe here!
            m_items.push_back(BS);
//          BS.get()->Start();
        }
    }
}

int RLNetwork::node_count()
{
    int cnt = 0;
    itt it;
    for(it=m_items.begin(); it!=m_items.end();++it)
    {
        cnt++;
    }
    return cnt;
}

void RLNetwork::train()
{
    itt it;
    for(it=m_items.begin();it!=m_items.end();++it)
    {
        rl_ptr agent = (*it);
        std::cout << "updating \n";
        agent->UpdateQFunction();
    }
}
