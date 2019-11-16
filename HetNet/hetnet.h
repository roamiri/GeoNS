#ifndef HETNET_H
#define HETNET_H

#include <iostream>
#include <thread>
#include <memory>
#include <mutex>

#include <boost/geometry/index/rtree.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "smallbs.h"
#include "common.h"
#include "painter.h"
#include "container.h"

/**
 * @todo write docs
 */
class HetNet : public Container
{
public:
    /**
     * Default constructor
     */
    HetNet();

    /**
     * Destructor
     */
    ~HetNet();

    void generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin);
    void load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin);
    
    void generate_MBS(double node_density, bool fixed, int fixed_count);
    void generate_SBS(double node_density, bool fixed, int fixed_count);
    void generate_UEs(double node_density, bool fixed, int fixed_count);
    void generate_Blockage();

    void load_MBS(double node_density, bool fixed, int fixed_count);
    void load_SBS(double node_density, bool fixed, int fixed_count);
    void load_UEs(double node_density, bool fixed, int fixed_count);
    void load_Blockage();
};

#endif // HETNET_H
