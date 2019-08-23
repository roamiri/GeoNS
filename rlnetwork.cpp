#include "rlnetwork.h"

using stateSpace = rl::problem::RC::Degree;

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
            BS->neighbors.connect_member(this, &RLNetwork::neighbor_handler);
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
            BS->neighbors.connect_member(this, &RLNetwork::neighbor_handler);
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
    synchronous_learning(NB_EPISODE);
}

void RLNetwork::neighbor_handler(const neighborhood_msg& msg)
{
    uint32_t id = msg.id;
    std::cout << "received from = " << id << "\n";
    float x = msg.x;
    float y = msg.y;
    double range = msg.range;
    int num = num_neighbors(x,y,range);
    set_state(id,num);
}

void RLNetwork::set_state(uint32_t id, int num)
{
    itt it;
    for(it=m_items.begin(); it!=m_items.end();++it)
    {
        rl_ptr mb = (*it);
        if(mb->getID()==id)
        {
            if(num>stateSpace::Kmax) num=stateSpace::Kmax;
            stateSpace::DEG dd = static_cast<stateSpace::DEG>(num);
            mb->setPhase(dd);
        }
    }
}


/** 
 * Multi-Agent learning. 
 * All agents select their actions, receive the reward and the new states, and learn synchronously controlled by the RLNetwork.
 */
void RLNetwork::synchronous_learning(int num_episodes)
{
    for(m_global_episode=0;m_global_episode<num_episodes;++m_global_episode)
    {
        std::cout << "running episode " << std::setw(6) << m_global_episode+1 << "/" << num_episodes << "   \r" << std::flush;
        
        itt it;
        for(it=m_items.begin(); it!=m_items.end();++it)
        {
            rl_ptr agent = (*it);
            agent->initRL();
        }
        
        for(it=m_items.begin(); it!=m_items.end();++it)
        {
            rl_ptr agent = (*it);
            agent->takeAction();
        }
        
        for(it=m_items.begin(); it!=m_items.end();++it)
        {
            rl_ptr agent = (*it);
            // find next state
            double max_range = 200.;
            double range = agent->get_trans_range();
            int num = num_neighbors(agent->getX(),agent->getY(),range);
            int KMAX = stateSpace::Kmax;
            int KGOAL = static_cast<int>(stateSpace::goal);
            if(num>KMAX) num=KMAX;
            stateSpace::DEG dd = static_cast<stateSpace::DEG>(num);
            //calculate reward
            double r=0;
            if(dd==KGOAL)
                r = 1-range/max_range;
            else
                r = 1-range/max_range - dd/KGOAL;
            
            agent->setSR(dd, r);
        }
        
        for(it=m_items.begin(); it!=m_items.end();++it)
        {
            rl_ptr agent = (*it);
            agent->episodic_learn();
        }
    }
}
