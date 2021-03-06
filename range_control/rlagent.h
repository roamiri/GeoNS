
#ifndef RLAGENT_H
#define RLAGENT_H

#include <boost/shared_ptr.hpp>

#include "trx.h"
#include "common.h"
#include "rl.hpp"
#include <random>

// class RLNetwork;
// 
// extern RLNetwork* _getNet;
enum O_POLICY
{
    score,
    softmax
};

using RLRC = rl::problem::RC::Degree;
using Param = rl::problem::RC::Param;
// using Simulator = RLNetwork<RLRC>;
// using Simulator = rl::problem::RC::Simulator<RLRC, Param>;

typedef typename RLRC::phase_type     phase_type;
typedef phase_type                   observation_type;
typedef rl::problem::RC::Action      action_type;
typedef double                       reward_type;

typedef reward_type Reward;
typedef observation_type S;
typedef action_type A;

#define S_CARDINALITY           rl::problem::RC::Degree::stateSize
#define A_CARDINALITY           rl::problem::RC::actionSize
#define TABULAR_Q_CARDINALITY   S_CARDINALITY*A_CARDINALITY
#define TABULAR_Q_RANK(s,a)     (static_cast<int>(a)*S_CARDINALITY+s)

static double q_parametrized(const gsl_vector* theta, S s, A a)
{
    return gsl_vector_get(theta, TABULAR_Q_RANK(s,a));
}

static double grad_q_parametrized(const gsl_vector* theta, gsl_vector* grad_theta_sa, S s, A a)
{
    return gsl_vector_set_basis(grad_theta_sa, TABULAR_Q_RANK(s,a));
}

template<typename AITER, typename SCORES>
double normalized_score(const S&s, const A& a, 
                        AITER action_begin, AITER action_end,
                        const SCORES& scores)
{
    double score = exp(scores(s,a));
    double Z = 0.0;
    for(auto ai=action_begin; ai!=action_end; ++ai)
        Z += exp(scores(s,*ai));
    return score/Z;
}

template<typename AITER, typename SCORES, typename OutPolicy>
void print_greedy_policy(AITER action_begin, AITER action_end,
			 const SCORES& scores, OutPolicy&op) {
  std::cout << "The greedy policy is depicted below. For each state, the greedy action\n     is displayed with a normalized score : exp(Q(s,a_greedy)) / sum_a exp(Q(s, a))\n\n";

  auto policy = rl::policy::greedy(scores, action_begin, action_end);
  int asize = rl::problem::RC::actionSize;
  int ssize = RLRC::stateSize;
  
  if(op==O_POLICY::softmax)
    for(int s = 0 ; s<ssize ; ++s) 
    {
        std::cout << s << " ";
        for(auto ai=action_begin; ai!=action_end; ++ai)
        {
            std::cout << " " << std::setfill(' ') << std::setw(5) << std::setprecision(3) <<
            normalized_score(s, (*ai), action_begin, action_end, scores) << " ";
        }
        std::cout << std::endl;
    }
  else if(op==O_POLICY::score)
    for(int s = 0 ; s<ssize ; ++s) 
    {
        std::cout << s << " ";
      for(auto ai=action_begin; ai!=action_end; ++ai)
      {
        std::cout << " " << std::setfill(' ') << std::setw(5) << std::setprecision(3) <<
        scores(s,*ai) << " ";
      }  
      std::cout << std::endl;
   
    }
}



#define paramGAMA       .99
#define paramALPHA      .2
#define paramEPSILON    .2

#define NB_EPISODE              1000
#define MAX_EPISODE_DURATION    100
#define FRAME_PERIOD            25
#define MIN_V                   -50

#define MAX_RANGE 200.

struct SR_msg
{
    uint32_t id;
    phase_type s;
    double rw;
    SR_msg(uint32_t iidd, phase_type ss, double rr){id=iidd; s=ss; rw=rr;}
};

struct neighborhood_msg
{
    uint32_t id;
    float x;
    float y;
    double range;
    neighborhood_msg(uint32_t iidd, float xx, float yy, double r){id=iidd;x=xx; y=yy; range=r;}
};

using namespace std::placeholders;
/**
 * @todo Reinforcement Learning agent
 */

class RLAgent : public TRX
{
public:
    /**
     * Default constructor
     */
    RLAgent(double x, double y, uint32_t id)
    :TRX((float)x, (float)y, 1.0, 1.0, id)
    {
        set_carrier(28.0);
        m_route_SNR = 0.; 
        m_route_SINR = 0.;
        m_theta_size = TABULAR_Q_CARDINALITY;
    }

    /**
     * Destructor
     */
    ~RLAgent()
    {
        gsl_vector_free(m_theta);
    }

    void reset()
    {
        set_hop_count(-1);
        set_parent(def_Nothing);
        set_SNR(0.);
        set_SINR(0.);
    }
    
    //TODO change it to multiple (wired and wireless) in main project
    Backhaul get_backhaul_Type(){return Backhaul::wireless;}
    
    gsl_vector* get_theta(){return m_theta;}
    
    Signal<neighborhood_msg const &> neighbors;
    Signal<uint32_t const> finished;
    
    void add_to_neighbors(boost::shared_ptr<RLAgent> agent);
    std::vector<uint32_t> get_neighborsID();
    int get_num_neighbors();
    
private:
    std::vector<boost::shared_ptr<RLAgent>> m_neighbors;
    
    //Q-learning related parameters
    gsl_vector* m_theta;
    int m_theta_size;
    phase_type m_current_state;
    phase_type m_next_state;
    Reward m_r;
    rl::enumerator<A> m_A_start;
    rl::enumerator<A> m_A_End;
    std::random_device m_rd;
    std::mt19937 m_generator;
    
    double m_epsilon;
    double m_alpha;
    double m_gamma;
    
    int m_max_episode;
    int m_episode;
    int m_length;
    int m_frame;
    int m_max_length;
    
    // selected action iterator
    action_type m_a;
    
    bool b_initialize;
    bool b_receivedSR;
    bool b_takeAction;
    bool b_updateQ ;
    
//     auto m_critic;
    
public:
    
    void Start();
    void ThreadMain();

    const observation_type& sense() const {return m_current_state;}
    const action_type& get_action() const {return m_a;}
    double get_trans_range();
    
    void restart()
    {
        setPhase(RLRC::zero);
    }
                
    void setPhase(const phase_type& s);
    
    reward_type reward() const {return m_r;}
    
    void timeStep(const action_type& a)
    {
        switch(m_current_state)
        {
            case(RLRC::goal):
            stepGoal();
            break;
            
//             case(RLRC::zero):
//             stepZero(a);
//             break;
            
            default:
            step(a);
            break;
                
        }
    }
                
    void initRL();
    void takeAction(bool sync, bool episodic);
    void takeGreedyAction();
    void setSR(phase_type s, reward_type r);
    void episodic_learn();
    void UpdateQFunction();
    void print_policy(O_POLICY p);
//     void received_SR();
    
    // asynchronous learning signal/slots
    void receive_init_RL(const int num_episodes);
    void receive_take_action();
    void receive_SR(const SR_msg &msg);
    
private:
    void stepGoal()
    {
        m_r = 1;
        throw rl::exception::Terminal("Goal State");
    }
    
    void stepZero(const action_type a)
    {
        m_current_state;
        m_r;
    }
    
    void step(const action_type a);
};

typedef boost::shared_ptr<RLAgent> rl_ptr;

#endif // RLAGENT_H
