

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
}


// episodic learning with a goal state
void RLAgent::takeAction()
{
    auto q = std::bind(q_parametrized, m_theta, _1, _2);
    auto learning_policy = rl::policy::epsilon_greedy(q,m_epsilon,m_A_start, m_A_End, m_generator);
//     auto s  = this->sense();
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


void RLAgent::setSR(phase_type s, reward_type r)
{
    m_next_state = s;
    m_r = r;
}

void RLAgent::episodic_learn()
{
    auto q = std::bind(q_parametrized, m_theta, _1, _2);
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
    dum *= 50;
    return dum;
}


void RLAgent::step(const action_type a)
{
    double dum = static_cast<double>(a);
    dum *= 50;
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
