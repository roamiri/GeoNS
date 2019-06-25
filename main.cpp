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
  
  int num_nodes = 3;
  
    // Read a file from matlab to simulate poisson point process
	double data[num_nodes][2];
	std::fstream _file("../data/3Nodes.txt");
	int i =0;
	if(_file.fail())
    {
		std::cerr << "input data file does not exis!" << std::endl;
    }
	while(_file)
	{
		int j=0;
		std::string line;
		std::getline(_file, line);
		std::istringstream is(line);
		double num;
		while(is >> num)
		{
			data[i][j] = num;
			j++;
		}
		i++;
	}
  
  //   Create the nodes
	for(int i =0;i<3;i++)
	{
// 		if(data[i][0])
		{
			std::shared_ptr<mmWaveBS> BS = std::make_shared<mmWaveBS>(10.0 *data[i][0], 10.0 *data[i][1], _idGenerator->next(), def_P_tx);
			BS.get()->setColor(0);
			manager.m_vector_BSs.push_back(BS);
		}
	}
	
	manager.m_vector_BSs[0]->setStatus(Status::clusterHead);
    
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=manager.m_vector_BSs.begin(); it!=manager.m_vector_BSs.end();++it)
    {
        std::shared_ptr<mmWaveBS> mmB = (*it);
        uint32_t cid = mmB.get()->getID();
        if(mmB.get()->getStatus()==Status::clusterHead)
            continue;
        
        double x = mmB.get()->getX();
        double y = mmB.get()->getY();
        for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it2=manager.m_vector_BSs.begin(); it2!=manager.m_vector_BSs.end();++it2)
        {
            std::shared_ptr<mmWaveBS> mmB2 = (*it2);
            if(mmB2.get()->getID() != cid)
            { 
                double snr = mmB->calculate_SNR_of_link(mmB2->getX(), mmB2->getY());
                double rate = mmB->calculate_Rate_of_link(mmB2->getX(), mmB2->getY());
//                 std::cout << "SBS= "<< mmB.get()->getID() << "SBS= "<< mmB2.get()->getID() << ", dist = " <<euclidean_dist(x,y, mmB2->getX(), mmB2->getY()) <<std::endl;
                std::cout << "SBS= "<< mmB.get()->getID() << "SBS= "<< mmB2.get()->getID() << " SNR= " << snr<< " Rate= " << rate/1.e9 << "Gbps" << std::endl;
            }
        }
    }
  
    std::cout << "Hello!" << std::endl;
    return 0;
}
