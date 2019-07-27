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
#include <random>

#include "manager.h"
#include "mmwavebs.h"
#include "idgenerator.h"
#include "painter.h"
// #include "ppunfix5.h"


int main()
{
  bool create_output = true;
  IDGenerator* _idGenerator = IDGenerator::instance();
  Manager manager;
   std::shared_ptr<Painter> _painter = std::make_shared<Painter>(manager.m_vector_BSs);
  _painter.get()->Start();
  int num_nodes = -1;
  
#ifdef Generate_Poisson
    // Generate data on a disk with radius r with poisson point process
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    
    double r = sqrt(1/M_PI)*1000.;                  // radius of disk
    double xx0=r; double yy0=r;   // centre of disk
    double areaTotal = M_PI*r*r;    // area of disk
    double lambda=50.;              //intensity (ie mean density) of the Poisson process
    // Simulate Poisson point process
    std::poisson_distribution<int> pd(areaTotal*lambda_SBS);
    num_nodes=pd(gen); // Poisson number of points
//     std::cout << numb_points << std::endl;
    
    double data[num_nodes][2];
    srand(time(NULL));
    
    for(int i=0;i<num_nodes;i++)
    {
        double theta=2*M_PI*(dis(gen));   // angular coordinates
        double rho=r*sqrt(dis(gen));      //radial coordinates
        
        data[i][0] = xx0 + rho * cos(theta);  // Convert from polar to Cartesian coordinates
        data[i][1] = yy0 + rho * sin(theta);
    }
    
//     {
//         ofstream test;
//         test.open("test_data.txt");
//         for(int i=0;i<num_nodes;i++)
//         {
//             test << std::to_string(data[i][0]);
//             test << " ";
//             test << std::to_string(data[i][1]);
//             test << " ";
//             test << sqrt(pow(data[i][0],2) + pow(data[i][1],2));
//             test << "\n";
//         }
//         test.close();
//     }
  
#else
    // Read a file from matlab to simulate poisson point process
    num_nodes = 100;
	double data[num_nodes][2];
	std::fstream _file("../data/exptable_148.txt");
	int i =0;
	if(_file.fail())
    {
		std::cerr << "input data file does not exis!" << std::endl;
    }
	while(i<num_nodes)
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
#endif
  //   Create the nodes
	for(int i =0;i<num_nodes;i++)
	{
        bool prob = (rand() % 100) < 100*def_prob_Wired;
        std::shared_ptr<mmWaveBS> BS;
        double x,y;
#ifdef Generate_Poisson
        x = data[i][0]; 
        y = data[i][1];
#else
        x = 1000.0 *data[i][0]; 
        y = 1000.0 *data[i][1];
#endif
        
        if(prob)
                BS = std::make_shared<mmWaveBS>(x,y, _idGenerator->next(),  def_P_tx, Backhaul::wired);
        else
                BS = std::make_shared<mmWaveBS>(x,y, _idGenerator->next(),  def_P_tx, Backhaul::IAB);
        
        BS.get()->setColor(0);
        manager.m_vector_BSs.push_back(BS);
//             BS.get()->Start();
        BS.get()->update_parent.connect_member(&manager, &Manager::listen_For_parent_update);
    }
	
	// Fidning the parents of nodes
// 	manager.m_vector_BSs[0]->setStatus(Status::clusterHead);
//     manager.m_vector_BSs[0]->set_hop_count(0);
    
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=manager.m_vector_BSs.begin(); it!=manager.m_vector_BSs.end();++it)
    {
        std::shared_ptr<mmWaveBS> mmB = (*it);
        uint32_t cid = mmB.get()->getID();
        
        if(mmB.get()->get_backhaul_Type()==Backhaul::wired)
            continue;
        
        double x = mmB.get()->getX();
        double y = mmB.get()->getY();
        double max_snr = -1.0;
        uint32_t parent = -1;
        for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it2=manager.m_vector_BSs.begin(); it2!=manager.m_vector_BSs.end();++it2)
        {
            std::shared_ptr<mmWaveBS> mmB2 = (*it2);
            if(mmB2.get()->getID() != cid)
            { 
                double dist = mmB->calculate_distance_of_link(mmB2->getX(), mmB2->getY());
                double snr = mmB->calculate_SNR_of_link(mmB2->getX(), mmB2->getY());
                // Rules
                bool b_snr = snr>max_snr;
                bool b_parent = mmB2.get()->get_IAB_parent()!=mmB.get()->getID();
                bool b_wired = mmB2->get_backhaul_Type()==Backhaul::wired;
                bool b_dist = dist<def_MAX_MMWAVE_RANGE;

                if(b_wired && b_dist)
                {
                    parent = mmB2.get()->getID();
                    break;
                }
                
                if(b_snr && b_parent)
                {
                    max_snr = snr;
                    parent = mmB2.get()->getID();
                }
//                 double rate = mmB->calculate_Rate_of_link(mmB2->getX(), mmB2->getY());
//                 std::cout << "SBS= "<< mmB.get()->getID() << "SBS= "<< mmB2.get()->getID() << ", dist = " <<euclidean_dist(x,y, mmB2->getX(), mmB2->getY()) <<std::endl;
            }
        }
        mmB->set_IAB_parent(parent);
        std::cout << "SBS= "<< mmB.get()->getID() << " parent= "<< parent << std::endl;
    }
    
    // set hop count of the nodes
//     manager.m_vector_BSs[0]->emit_update_parent();
    bool finish = false;
    int counter = 0;
    while((!finish) && (counter<10))
    {
        bool all_found = true;
        for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=manager.m_vector_BSs.begin(); it!=manager.m_vector_BSs.end(); ++it)
        {
            std::shared_ptr<mmWaveBS> mmB = (*it);
            if(mmB->get_hop_count()!=-1)
                mmB->emit_update_parent();
            else
                all_found = false;
        }
        
        if(all_found)
            finish = true;
        counter++;
    }
    
    
    int hop_vec[10] = {0};
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=manager.m_vector_BSs.begin(); it!=manager.m_vector_BSs.end(); ++it)
    {
        std::shared_ptr<mmWaveBS> mmB = (*it);
        int hcnt = mmB->get_hop_count();
        if(hcnt != -1)
            hop_vec[hcnt]++;
//         std::cout << "SBS " << mmB->getID() << " hop count = " << mmB->get_hop_count() << std::endl;
    }
    
    std::this_thread::sleep_for( std::chrono::seconds(2) );
    _painter.get()->Enable();
    
    
    std::cout << "Done!" << std::endl;
    return 0;
}
