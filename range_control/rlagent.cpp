

#include "rlagent.h"
// #include "rlnetwork.h"

using stateSpace = rl::problem::RC::Degree;

void RLAgent::initRL()
{
    m_theta = gsl_vector_alloc(TABULAR_Q_CARDINALITY);
    gsl_vector_set_zero(m_theta);
    m_A_start = rl::enumerator<A>(rl::problem::RC::Action::action_00);
    m_A_End = m_A_start + rl::problem::RC::actionSize;
    m_generator = std::mt19937(m_rd());
    m_epsilon = paramEPSILON;
    m_alpha = paramALPHA;
    m_gamma = paramGAMA;
    m_episode = 0;
    m_frame = 0;
    m_length = 0;
    m_max_length = MAX_EPISODE_DURATION;
    restart();
    
    // asynchronous learning params
    b_initialize=false;
    b_receivedSR=false;
    b_takeAction=false;
    b_updateQ = false;
}

void RLAgent::Start()
{
    the_thread = std::thread(&RLAgent::ThreadMain, this);
    char thread_name [20];
    sprintf(thread_name, "RLAgent_%d", getID());
    prctl(PR_SET_NAME, thread_name, 0,0,0);
}

void RLAgent::ThreadMain()
{
    while(!stop_thread)
    {
        if(b_initialize)
        {
            initRL();
            b_initialize = false;
            b_takeAction = true;
        }
        
        if(b_takeAction)
        {
            takeAction(false);
            b_takeAction = false;
        }
        
        if(b_receivedSR)
        {
            b_receivedSR = false;
            b_updateQ = true;
        }
        if(b_updateQ)
        {
            episodic_learn();
            b_updateQ = false;
            b_takeAction = true;
        }
    }
}

    
    
void RLAgent::setPhase(const phase_type& s)
{
    m_current_state = s;
    if(s < RLRC::zero || s > RLRC::Kmax)
    {
        std::ostringstream ostr;
        ostr << __FUNCTION__ << "( "<<  s << " )";
        rl::problem::RC::BadState(ostr.str());
    }
}

// episodic learning with a goal state
void RLAgent::takeAction(bool episodic)
{
    auto q = std::bind(q_parametrized, m_theta, _1, _2);
    auto learning_policy = rl::policy::epsilon_greedy(q,m_epsilon,m_A_start, m_A_End, m_generator);
//     auto s  = this->sense();
    if(episodic)
    {
        if(m_length<m_max_length)
        {
            m_a = learning_policy(m_current_state);
            ++m_length;
        }
        else
        {
            m_length=0;
            ++m_episode;
            restart();
            m_a = learning_policy(m_current_state);
        }
    }
    else
    {
        m_a = learning_policy(m_current_state);
        ++m_episode;
    }
    if(m_episode<m_max_episode)
        step(m_a);
    else
    {
        finished.emit(getID());
        stop_thread = true;
    }
}

void RLAgent::takeGreedyAction()
{
    auto q = std::bind(q_parametrized, m_theta, _1, _2);
    auto policy = rl::policy::greedy(q, m_A_start, m_A_End);
    m_a = policy(m_current_state);
}


void RLAgent::setSR(phase_type s, reward_type r)
{
    m_next_state = s;
    m_r = r;
}

void RLAgent::episodic_learn()
{
    auto critic = rl::gsl::q_learning<S,A>(m_theta,
            m_gamma, m_alpha,
            m_A_start, m_A_End,
            q_parametrized,grad_q_parametrized);
    
    if(m_next_state==stateSpace::DEG::goal)
    {
        critic.learn(m_current_state,m_a,reward());
        restart();
        m_length=0;
        ++m_episode;
    }
    else
    {
        critic.learn(m_current_state,m_a,this->reward(),m_next_state);
        setPhase(m_next_state);
    }   
}

void RLAgent::UpdateQFunction()
{
    auto q = std::bind(q_parametrized, m_theta, _1, _2);
    auto critic = rl::gsl::q_learning<S,A>(m_theta, m_gamma, m_alpha, m_A_start, m_A_End, q_parametrized, grad_q_parametrized);
    if(m_next_state==stateSpace::DEG::goal)
        critic.learn(m_current_state, m_a, reward());
    else
    {
        critic.learn(m_current_state, m_a, reward(), m_next_state);
    }
    setPhase(m_next_state); //TODO not sure, check it.
}

// void RLAgent::UpdateQFunction()
// {
//         
// //         std::random_device rd;
// //         std::mt19937 gen(rd());
// //         auto action_begin = rl::enumerator<A>(rl::problem::RC::Action::action_00);
// //         auto action_end = action_begin + rl::problem::RC::actionSize;
//         
//         auto q = std::bind(q_parametrized, m_theta, _1, _2);
//         auto critic = rl::gsl::q_learning<S,A>(m_theta,
//             m_gamma, m_alpha,
//             action_begin, action_end,
//             q_parametrized,
//             grad_q_parametrized);
//         
//         //TODO experiment
// //         make_experiment(critic, q, gen);
//         Param param;
// //         Simulator simulator(param);
//         double   epsilon         = paramEPSILON;
// //         auto     learning_policy = rl::policy::epsilon_greedy(q,epsilon,action_begin, action_end, gen);
//         auto     test_policy     = rl::policy::greedy(q,action_begin, action_end);
//         int       episode, frame;
//         for(episode=0, frame =0; episode<NB_EPISODE; ++episode)
//         {
// //             std::cout << "running episode " << std::setw(6) << episode+1 
// //             << "/" << NB_EPISODE << "   \r" << std::flush;
//             
//             this->restart();
//             unsigned int length=0;
//             auto s  = this->sense();
//             auto sa = std::make_pair(s,learning_policy(s));
//             try {
//                 do {
//                     ++length;
//                     try {
//                         timeStep(sa.second);
//                         auto next = this->sense();
//                         auto res = std::make_pair(next,learning_policy(next));
//                         critic.learn(s,sa.second,reward(),next);
// //                         return res;
//                     }
//                     catch(rl::exception::Terminal& e) { 
//                         critic.learn(s,sa.second,reward());
// //                         throw e;
//                     }
//                 } while(length != MAX_EPISODE_DURATION);
//             }
//             catch(rl::exception::Terminal& e) {}
//         }
//         
// }


double RLAgent::get_trans_range()
{
    double dum = static_cast<double>(m_a);
    int steps = A_CARDINALITY-1;
    double coef = MAX_RANGE/(steps);
    dum *= coef;
    return dum;
}


void RLAgent::step(const action_type a)
{
    double dum = static_cast<double>(a);
    int steps = A_CARDINALITY -1;
    double coef = MAX_RANGE/steps;
    dum *= coef;
    neighborhood_msg msg(getID(),getX(),getY(), dum);
    neighbors.emit(msg);
//             int num = _getNet->instance()->num_neighbors(getX(),getY(), dum);
//     m_current_state;
//     auto action_begin = rl::enumerator<A>(rl::problem::RC::Action::action_00);
//     auto action_end = action_begin + rl::problem::RC::actionSize;
//     auto it = action_begin;
//     while(it!=action_end)
//     {
//         double dum = static_cast<double>(*it);
//         dum *= 50;
//             neighborhood_msg msg(getID(),getX(),getY(), dum);
// //             int num = _getNet->instance()->num_neighbors(getX(),getY(), dum);
//             neighbors.emit(msg);
//         it=it.operator++();
//     }
// 
//     m_r;
}

void RLAgent::add_to_neighbors(boost::shared_ptr<RLAgent> agent)
{
    bool found = false;
    std::vector<boost::shared_ptr<RLAgent>>::iterator it;
    for(it=m_neighbors.begin(); it!=m_neighbors.end();++it)
    {
        rl_ptr bs = (*it);
        if(bs->getID()==agent->getID())
        {found = true; break;}
    }
    
    if(!found)
    {
        m_neighbors.push_back(agent);
    }
}

std::vector<uint32_t> RLAgent::get_neighborsID()
{
    std::vector<uint32_t> result;
    std::vector<boost::shared_ptr<RLAgent>>::iterator it;
    for(it=m_neighbors.begin(); it!=m_neighbors.end();++it)
    {
        rl_ptr bs = (*it);
        result.push_back(bs->getID());
    }
    return result;
}

int RLAgent::get_num_neighbors()
{
    int ss = m_neighbors.size();
    return ss;
}

void RLAgent::print_policy(O_POLICY p)
{
    auto q = std::bind(q_parametrized, m_theta, _1, _2);
    print_greedy_policy(m_A_start, m_A_End, q, p);
}

void RLAgent::receive_init_RL(const int num_episodes)
{
    m_max_episode = num_episodes;
    b_initialize = true;
}

void RLAgent::receive_SR(const SR_msg &msg)
{
    uint32_t id = msg.id;
    phase_type s = msg.s;
    double r = msg.rw;
    if(id==getID())
    {
        setSR(s,r);
        b_updateQ = true;
    }
}

void RLAgent::receive_take_action()
{
}

