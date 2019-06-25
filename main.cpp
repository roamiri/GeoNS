#include "Signal.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <sys/prctl.h>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>

#include "manager.h"
#include "mmwavebs.h"
#include "idgenerator.h"
// #include "painter.h"
#include "ppunfix5.h"


int main()
{
  bool create_output = true;
  IDGenerator* _idGenerator = IDGenerator::instance();
  Manager manager;
  
  
  
    std::cout << "Hello!" << std::endl;
    return 0;
}
