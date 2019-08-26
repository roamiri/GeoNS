
#ifndef RLNETWORK_H
#define RLNETWORK_H


#include "container.h"
#include "common.h"
#include "rlagent.h"

#include <boost/shared_ptr.hpp>
#include <boost/pointer_cast.hpp>
/**
 * @todo Nework consisting of RL agents
 */

// using Simulator = rl::problem::RC::Simulator<RLRC, Param>;

class RLNetwork : public Container<RLAgent>
{
public:
    typedef std::vector<boost::shared_ptr<RLAgent>>::iterator itt;
    
    /**
     * Default constructor
     */
    RLNetwork():Container()
    {
        m_global_episode=0;
    }

    /**
     * Destructor
     */
    ~RLNetwork(){}
     
    
void generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin);

void load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin);

int node_count();

void train(int nb_episode);

void neighbor_handler(const neighborhood_msg& msg);

void set_state(uint32_t id, int num);

void synchronous_learning(int num_episodes);

void k_connect(int round);

void set_neighbors(rl_ptr agent, double range);
void draw_neighbors(bool bdraw);

private:
    int m_global_episode;

// public:
//     static RLNetwork* instance();
    
// private:
//     static RLNetwork* only_copy;
};

// RLNetwork* RLNetwork::only_copy = 0;
    
// RLNetwork* RLNetwork::instance() 
// {
//     if (only_copy == NULL) only_copy = new RLNetwork();
//     return only_copy;
// }
// 
// RLNetwork* _getNet = RLNetwork::instance();
// extern RLNetwork* global_ENV;

#endif // RLNETWORK_H
