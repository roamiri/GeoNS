#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include <stdint.h>

class IDGenerator {
   public:
	static IDGenerator * instance ();
      uint32_t next () { return _id++; }

   private:
      IDGenerator () : _id(0) {}
      static IDGenerator * only_copy;
      uint32_t _id;
};

IDGenerator* IDGenerator::only_copy = 0;

IDGenerator* IDGenerator::instance () 
{
   if (!only_copy) {
      only_copy = new IDGenerator();
   }
   return only_copy;
}
#endif // IDGENERATOR_H
