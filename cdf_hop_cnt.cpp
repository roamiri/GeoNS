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
#include "ppunfix5.h"


int main(int argc, char** argv)
{
    IDGenerator* _idGenerator = IDGenerator::instance();
    Manager manager;
    
    //   Create the nodes
    int num_nodes = 100; // Poisson number of points
	for(int i =0;i<num_nodes;i++)
	{
        std::shared_ptr<mmWaveBS> BS;
        BS = std::make_shared<mmWaveBS>(_idGenerator->next(),  def_P_tx);
        BS.get()->setColor(0);
        manager.m_vector_BSs.push_back(BS);
        BS.get()->update_parent.connect_member(&manager, &Manager::listen_For_parent_update);
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    
    int Total_iter = 10;
    int Total_fail = 0;
    int CDF_Hop_vec[10] = {0};
    for(int iter=0;iter<Total_iter;iter++)
    {
        // Generate data on a disk with radius r with poisson point process    
        double r = sqrt(1/M_PI)*1000.;                  // radius of disk
        double xx0=r; double yy0=r;   // centre of disk
        
        double data[num_nodes][2];
        
        for(int i=0;i<num_nodes;i++)
        {
            double theta=2*M_PI*(dis(gen));   // angular coordinates
            double rho=r*sqrt(dis(gen));      //radial coordinates
            
            data[i][0] = xx0 + rho * cos(theta);  // Convert from polar to Cartesian coordinates
            data[i][1] = yy0 + rho * sin(theta);
        }
         
        int cnt = 0;
        for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=manager.m_vector_BSs.begin(); it!=manager.m_vector_BSs.end();++it)
        {
            std::shared_ptr<mmWaveBS> mmB = (*it);
            mmB->setX(data[cnt][0]);
            mmB->setY(data[cnt][1]);
            
            bool prob = (rand() % 100) < 100*def_prob_Wired;
            if(prob)
                mmB->set_backhaul_Type(Backhaul::wired);
            else
                mmB->set_backhaul_Type(Backhaul::IAB);
            
            cnt++;
        }
        
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
//         std::cout << "SBS= "<< mmB.get()->getID() << " parent= "<< parent << std::endl;
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
        int failed = 0;
        for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=manager.m_vector_BSs.begin(); it!=manager.m_vector_BSs.end(); ++it)
        {
            std::shared_ptr<mmWaveBS> mmB = (*it);
            int hcnt = mmB->get_hop_count();
            if(hcnt != -1)
                hop_vec[hcnt]++;
            else
                failed++;
    //         std::cout << "SBS " << mmB->getID() << " hop count = " << mmB->get_hop_count() << std::endl;
        }
        
        int total_hops = 0;
        for(int i=0;i<10;i++)
        {
            total_hops+=i*hop_vec[i];
            CDF_Hop_vec[i]+=hop_vec[i];
        }
        Total_fail+= failed;
    //     std::cout << "Total hops = " << total_hops << ", number fails = " << failed << std::endl;
        
        for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=manager.m_vector_BSs.begin(); it!=manager.m_vector_BSs.end(); ++it)
        {
            std::shared_ptr<mmWaveBS> mmB = (*it);
            mmB->reset();
        }
    }
    
    std::cout << "Total hops = " << CDF_Hop_vec << ", number fails = " << Total_fail << std::endl;
    
    return 0;
}
