
#ifndef RLAGENT_H
#define RLAGENT_H

#include <boost/shared_ptr.hpp>

#include "trx.h"
#include "common.h"
#include "RL/rl.hpp"
#include <random>

using RLRC = rl::problem::RC::Degree<3>;
using Param = rl::problem::RC::Param;
using Simulator = rl::problem::RC::Simulator<RLRC, Param>;

typedef Simulator::reward_type Reward;
typedef Simulator::observation_type S;
typedef Simulator::action_type A;

#define S_CARDINALITY           RLRC::stateSize
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

#define paramGAMA       .99
#define paramALPHA      .05
#define paramEPSILON    .55

#include "RL/rl_experiment.hpp"

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
    }

    /**
     * Destructor
     */
    ~RLAgent(){}

    void reset()
    {
        set_hop_count(-1);
        set_parent(def_Nothing);
        set_SNR(0.);
        set_SINR(0.);
    }
    
    //TODO change it to multiple (wired and wireless) in main project
    Backhaul get_backhaul_Type(){return Backhaul::wireless;}

    
private:
    std::vector<boost::shared_ptr<RLAgent>> m_load_BS;
    
    gsl_vector* m_theta;
    
    void ThreadMain()
    {
        ;
    }
    
    
    
public:
    
    void Start()
    {
        the_thread = std::thread(&RLAgent::ThreadMain, this);
        char thread_name [20];
        sprintf(thread_name, "RLAgent_%d", getID());
        prctl(PR_SET_NAME, thread_name, 0,0,0);
    }
    
    void UpdateQFunction()
    {
        m_theta = gsl_vector_alloc(TABULAR_Q_CARDINALITY);
        std::random_device rd;
        std::mt19937 gen(rd());
        auto action_begin = rl::enumerator<A>(rl::problem::RC::Action::action_50);
        auto action_end = action_begin + rl::problem::RC::actionSize;
        
        auto q = std::bind(q_parametrized, m_theta, _1, _2);
        auto critic = rl::gsl::q_learning<S,A>(m_theta,
            paramGAMA, paramALPHA,
            action_begin, action_end,
            q_parametrized,
            grad_q_parametrized);
        gsl_vector_set_zero(m_theta);
        //TODO experiment
        make_experiment(critic, q, gen);
        gsl_vector_free(m_theta);
    }
};

typedef boost::shared_ptr<RLAgent> rl_ptr;
#endif // RLAGENT_H
