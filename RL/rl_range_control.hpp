
#include <iostream>
#include <sstream>
#include "rlAlgo.hpp"
#include "rlException.hpp"
#include <gsl/gsl_vector.h>


namespace rl
{
    namespace problem
    {
        namespace RC // RC stands for range control
        {
            // Action Space //TODO make it template based or variable size!
            enum class Action
            {
                action_00 = 0,
                action_50 = 1,
                action_100 = 2,
                action_150 = 3,
                action_200 = 4
            };
            
            constexpr int actionSize = 5;
            
            // check action and state consistency
            
            class BadAction : public rl::exception::Any
            {
            public:
                BadAction(std::string comment):Any(std::string("Bad Action performed: ")+comment) {}
            };
            
            class BadState : public rl::exception::Any
            {
            public:
                BadState(std::string comment):Any(std::string("Bad state found : ")+comment){}
            };
            
            // State space
//             template<int Kmax>
            class Degree  // Degree of node connectivity
            {
            public:
            const static int Kmax = 10;
            enum DEG
            {
                stateSize = Kmax+1,  // the number of states 
                zero = 0,            // the starting state 
                goal = 3   // the goal state
            };
                
                typedef int phase_type;
            };
            
            class Param 
            {
            public:
                double goalReward() const {return 1.;}
                double stepReward() const {return 0.;}
                double zeroReward() const {return -1.;}
            };
            
//             template<typename DEG, typename PARAM>
//             class Simulator
//             {
//             private:
//                 const PARAM& param;
//             public:
//                 typedef typename DEG::phase_type phase_type;
//                 typedef phase_type               observation_type;
//                 typedef Action                   action_type;
//                 typedef double                   reward_type;
//                 
//             private:
//                 phase_type current_state;
//                 double     r;
//             
//             public:
//                 void restart()
//                 {
//                     setPhase(DEG::zero);
//                 }
//                 
//                 void setPhase(const phase_type& s)
//                 {
//                     current_state = s;
//                     if(s < DEG::zero || s > DEG::goal)
//                     {
//                         std::ostringstream ostr;
//                         ostr << __FUNCTION__ << "( "<<  s << " )";
//                         BadState(ostr.str());
//                     }
//                 }
//                 
//                 const observation_type& sense() const 
//                 {return current_state;}
//                 
//                 void timeStep(const action_type& a)
//                 {
//                     switch(current_state)
//                     {
//                         case(DEG::goal):
//                             stepGoal();
//                             break;
//                         case(DEG::zero):
//                             stepZero(a);
//                             break;
//                         default:
//                             step(a);
//                             break;
//                     }
//                 }
//                 
//             private:
//                 void stepGoal()
//                 {
//                     r = param.goalReward();
//                     throw rl::exception::Terminal("Goal State");
//                 }
//                 
//                 void stepZero(const action_type a)
//                 {
//                     current_state; // TODO
//                     r = param.zeroReward();
//                 }
//                 
//                 void step(const action_type a)
//                 {
//                     current_state; // TODO
//                     r = param.stepReward();
//                 }
//                 
//             public:
//                 reward_type reward() const {return r;}
//                 Simulator(const PARAM&p): param(p), current_state(DEG::zero){}
//                 Simulator(const Simulator& copy): param(copy.param), current_state(copy.sense()){}
//                 ~Simulator(){}
//                 
//             };
//             
            
            
        }
    }
}
