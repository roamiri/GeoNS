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
            initiRL.connect_member(BS.get(), &RLAgent::receive_init_RL);
            newSR.connect_member(BS.get(), &RLAgent::receive_SR);
            BS->neighbors.connect_member(this, &RLNetwork::neighbor_handler);
            BS->finished.connect_member(this, &RLNetwork::finish_training);
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

void RLNetwork::train(int nb_episode)
{
    Asynchronous_learning(nb_episode);
}

void RLNetwork::neighbor_handler(const neighborhood_msg& msg)
{
    uint32_t id = msg.id;
//     std::cout << "received from = " << id << "\n";
    float x = msg.x;
    float y = msg.y;
    double range = msg.range;
    int num = num_neighbors(x,y,range);
    if(num>stateSpace::Kmax) num=stateSpace::Kmax;
    stateSpace::DEG dd = static_cast<stateSpace::DEG>(num);
    //calculate reward
    int KGOAL = static_cast<int>(stateSpace::goal);
    double r=0;
    if(dd==stateSpace::zero)
        r = 0;//-range/max_range;
    else
        r = exp(-pow(num-KGOAL,2));//1-range/max_range - dd/KGOAL;
    SR_msg message(id, dd, r);
    newSR.emit(message);
//     set_state(id,num);
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
    itt it;
    // Initialize RL agents
    for(it=m_items.begin(); it!=m_items.end();++it)
    {
        rl_ptr agent = (*it);
        agent->initRL();
    }

    for(m_global_episode=0;m_global_episode<num_episodes;++m_global_episode)
    {
        std::cout << "running episode " << std::setw(6) << m_global_episode+1 << "/" << num_episodes << "   \r" << std::flush;
        //Action selection
        for(it=m_items.begin(); it!=m_items.end();++it)
        {
            rl_ptr agent = (*it);
            agent->takeAction(false);
        }
        // Receive reward and new state
        double max_range = MAX_RANGE;
        for(it=m_items.begin(); it!=m_items.end();++it)
        {
            rl_ptr agent = (*it);
            // find next state
            double range = agent->get_trans_range();
            int num = num_neighbors(agent->getX(),agent->getY(),range);
            int KMAX = stateSpace::Kmax;
            int KGOAL = static_cast<int>(stateSpace::goal);
            if(num>KMAX) num=KMAX;
            stateSpace::DEG dd = static_cast<stateSpace::DEG>(num);
            //calculate reward
            double r=0;
            if(dd==stateSpace::zero)
                r = 0;//-range/max_range;
            else
                r = exp(-pow(num-KGOAL,2));//1-range/max_range - dd/KGOAL;
            
            agent->setSR(dd, r);
        }
        // learning
        for(it=m_items.begin(); it!=m_items.end();++it)
        {
            rl_ptr agent = (*it);
            agent->episodic_learn();
        }
    }
}

void RLNetwork::synchronous_learning_1_Agent(int num_episodes)
{
    itt it;
    for(it=m_items.begin(); it!=m_items.end();++it)
    {
        rl_ptr agent = (*it);
        agent->initRL();
    }

    it = m_items.begin();
    rl_ptr agent = (*it);
    for(m_global_episode=0;m_global_episode<num_episodes;++m_global_episode)
    {
        std::cout << "running episode " << std::setw(6) << m_global_episode+1 << "/" << num_episodes << "   \r" << std::flush;

        agent->takeAction(false);
        
        double max_range = MAX_RANGE;
        // find next state
        double range = agent->get_trans_range();
        int num = num_neighbors(agent->getX(),agent->getY(),range);
        int KMAX = stateSpace::Kmax;
        int KGOAL = static_cast<int>(stateSpace::goal);
        if(num>KMAX) num=KMAX;
        stateSpace::DEG dd = static_cast<stateSpace::DEG>(num);
        //calculate reward
        double r=0;
        if(dd==stateSpace::zero)
            r = 0;//-range/max_range;
        else
            r = exp(-pow(num-KGOAL,2));//1-range/max_range - dd/KGOAL;
        
        agent->setSR(dd, r);
        agent->episodic_learn();
    }
    agent->print_policy(O_POLICY::softmax);
}

void RLNetwork::Asynchronous_learning(int num_episodes)
{
    itt it;
    for(it=m_items.begin(); it!=m_items.end();++it)
    {
        rl_ptr bs = (*it);
        bs->Start();
    }
    initiRL.emit(num_episodes);
}

/**
 * Runs Q-learning for #round to obtain connectivity for each node
 */
void RLNetwork::k_connect(int round)
{
    itt it;
    double max_range = MAX_RANGE;
    int KGOAL = static_cast<int>(stateSpace::DEG::goal);
    for(it=m_items.begin(); it!=m_items.end();++it)
    {
        rl_ptr agent = (*it);
        // Figure out state of agent for maximum transmission range
        int num = num_neighbors(agent->getX(),agent->getY(),max_range);
        int KMAX = stateSpace::Kmax;
        if(num>KMAX) num=KMAX;
        stateSpace::DEG dd = static_cast<stateSpace::DEG>(num);
        agent->setPhase(dd);
    }
    
    for(it=m_items.begin();it!=m_items.end();++it)
    {
        rl_ptr agent = (*it);
        while(round>0)
        {
            agent->takeGreedyAction();
            double range = agent->get_trans_range();
            int num = num_neighbors(agent->getX(),agent->getY(),range);
            int KMAX = stateSpace::Kmax;
            if(num>KMAX) num=KMAX;
            stateSpace::DEG dd = static_cast<stateSpace::DEG>(num);
            //calculate reward
            double r=0;
            if(dd==stateSpace::zero)
                r = 0;//-range/max_range;
            else
                r = exp(-pow(num-KGOAL,2));//1-range/max_range - dd/KGOAL;
            
            agent->setSR(dd, r);
            agent->UpdateQFunction();
            if(dd==stateSpace::DEG::goal)
                break;
            --round;
        }
        set_neighbors(agent,agent->get_trans_range());
    }
    
}


void RLNetwork::set_neighbors(rl_ptr agent, double range)
{
    // search for nearest neighbours
    std::vector<value> results;
    float xx = (float) agent->getX();
    float yy = (float) agent->getY();
    point sought(xx,yy);
    m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < range;}),std::back_inserter(results));
    
    BOOST_FOREACH(value const&v, results)
    {
        rl_ptr bs = boost::dynamic_pointer_cast<RLAgent>(v.second);
        if(agent->getID()!=bs->getID())
            agent->add_to_neighbors(bs);
    }
}

void RLNetwork::draw_neighbors(bool bdraw)
{
    if(bdraw)
    {
        m_painter->draw_neighbors<RLAgent>(m_items);
    }
}

void RLNetwork::print_Q_function(uint32_t id, O_POLICY op)
{
    m_items[id-1]->print_policy(op);
}

void RLNetwork::finish_training(const uint32_t id)
{
    std::cout << "agent " << id << "finished training!\n";
    ++m_finished_agents;
    if(m_finished_agents==m_items.size())
        b_ready = true;
}
