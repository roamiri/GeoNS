
#ifndef WSENSOR_H
#define WSENSOR_H

#include "node.h"
#include "trx.h"
#include "common.h"
#include <boost/shared_ptr.hpp>

/**
 * @todo write docs
 */
class wSensor :  public TRX
{
public:
    /**
     * Default constructor
     */
    wSensor(double x, double y, uint32_t id);

    /**
     * Destructor
     */
    ~wSensor();

    void reset();
    
    
private:

    std::vector<boost::shared_ptr<wSensor>> m_load_BS;
    
};

typedef boost::shared_ptr<wSensor> ws_ptr;

#endif // WSENSOR_H
