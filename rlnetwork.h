
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
class RLNetwork : public Container<RLAgent>
{
public:
    typedef std::vector<boost::shared_ptr<RLAgent>>::iterator itt;
    /**
     * Default constructor
     */
    RLNetwork(std::string svg_name);

    /**
     * Destructor
     */
    ~RLNetwork();
    
    void generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin) override;
    
    void load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin) override;
    
    int node_count();
    
    void train();


};

#endif // RLNETWORK_H
