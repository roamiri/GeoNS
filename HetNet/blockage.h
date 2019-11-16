
#ifndef BLOCKAGE_H
#define BLOCKAGE_H

#include <thread>
#include <chrono>
#include <sys/prctl.h>

#include "node.h"
#include "common.h"

/**
 * @todo write docs
 */
class Blockage : public node
{
public:
    /**
     * Default constructor
     */
    Blockage(float x, float y, float width, float height, uint32_t id, typ tt=typ::BL)
    :node(x,y,width,height,id, tt)
    {}

    /**
     * Destructor
     */
    ~Blockage(){}
    
    Backhaul get_backhaul_Type(){}

};

typedef boost::shared_ptr<Blockage> bl_ptr;
#endif // BLOCKAGE_H
