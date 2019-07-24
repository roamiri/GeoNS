/*
 * Copyright 2018 <copyright holder> <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "manager.h"
#include <math.h>
#include <limits>
#include <random>

Manager::Manager()
// :m_draw_thread()
{
// 	m_painter = p;
    std::cout << "Manager started!\n";
}

Manager::~Manager()
{
// 	stop_thread = true;
// 	if(m_draw_thread.joinable()) m_draw_thread.join();
	std::cout << "Deconstruct " << __FILE__ << std::endl;
}

void Manager::listen_For_Candidacy(const candidacy_msg& message)
{
	std::cout << " Candidate received: x=" << message.x << ", y=" << message.y << " from id=" << message.id << std::endl;
	uint32_t candidate_id = message.id;
	double x = message.x;
	double y = message.y;
	bool ib_found = false;
	for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it = m_vector_BSs.begin(); it != m_vector_BSs.end(); ++it) 
	{
		std::shared_ptr<mmWaveBS> mmB = (*it);
		if(euclidean_dist2(x, y, mmB->getX(), mmB->getY()) <=  pow(in_bound, 2))
		{
			if(mmB->getStatus()==Status::clusterHead)
			{
				ib_found = true;
				joinCluster(candidate_id, Status::inBound, mmB->getClusterID(), mmB->getColor());
				break;
			}
			else if((mmB->getStatus()==Status::inBound))
			{
				ib_found = true;
				joinCluster(candidate_id, Status::outBound, mmB->getClusterID(), mmB->getColor());
			}
		}
	}
	if(!ib_found)
		makeCluster(message.id);
}

void Manager::listen_For_ClusterHead(const cluster_head_msg& message)
{
	std::cout << " Cluster Head received: " << message.id << std::endl;
	uint32_t new_cluster_id = message.id;
	double x = message.x;
	double y = message.y;
	std::size_t new_cluster_color = message.color;
	
// 	std::lock_guard<std::mutex> guard(m_mutex);
	for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it != m_vector_BSs.end(); ++it)
	{
		std::shared_ptr<mmWaveBS> mmB = (*it);
		double dist2 = euclidean_dist2(x, y, mmB->getX(), mmB->getY());
		if(dist2>0) // dist2=0 means the same node
		{
			if(dist2 <=  pow(out_bound, 2))
			{
				if(dist2 <=  pow(in_bound, 2))
				{
	// 				if(mmB->getStatus()==Status::idle)
	// 				{
						mmB->setClusterID(new_cluster_id);
						mmB->setStatus(Status::inBound);
						mmB->setColor(new_cluster_color);
	// 				}
				}
				else
				{
					if(mmB->getStatus()==Status::idle)
					{
						mmB->setClusterID(new_cluster_id);
						mmB->setStatus(Status::outBound);
						mmB->setColor(new_cluster_color);
					}
				}
			}
		}
	}
}

void Manager::listen_For_Conflict(const std::string& message)
{
	std::cout << " Conflict received: " << message << std::endl;
}

void Manager::joinCluster(uint32_t id, Status st, uint32_t cluster_id, std::size_t color)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it = m_vector_BSs.begin(); it != m_vector_BSs.end(); ++it)
	{
		if((*it)->getID()==id)
		{
			(*it)->setClusterID(cluster_id);
			(*it)->setStatus(st);
			(*it)->setColor(color);
// 			std::shared_ptr<draw_object> new_node = std::make_shared<draw_object>((*it)->getX(), (*it)->getY(), cluster_id);
// 			m_painter.get()->add_to_draw_queue(new_node);
// 			m_painter.get()->Enable();
			std::cout << "The BS_" << id << " joined cluster: " << cluster_id << " as " << st << std::endl;
			break;
		}
	}
}

void Manager::makeCluster(uint32_t id)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
		if((*it)->getID()==id)
		{
			(*it)->setClusterID(id);
			(*it)->setStatus(Status::clusterHead);
			(*it)->setColor(generateColor());
			(*it).get()->declare_as_cluster_head();
// 			std::shared_ptr<draw_object> new_node = std::make_shared<draw_object>((*it)->getX(), (*it)->getY(), (*it)->getID());
// 			m_painter.get()->add_to_draw_queue(new_node);
// 			m_painter.get()->Enable();
			std::cout << "The BS_" << id << " is Cluster Head!!" << std::endl;
			break;
		}
}

void Manager::listen_For_parent_update(const update_parent_msg& msg)
{
    uint32_t id = msg.id;
    int hop_cnt = msg.hop_cnt;
//     std::cout << "Parent update from SBS= " << id << std::endl;
    std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
    {
        std::shared_ptr<mmWaveBS> mmB = (*it);
        if(mmB->get_IAB_parent()==id)
        {
            mmB->set_hop_count(hop_cnt+1);
            mmB->route_found(true);
        }
    }
}

void Manager::path_selection_WF()
{
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
        {
            std::shared_ptr<mmWaveBS> mmB = (*it);
            uint32_t cid = mmB.get()->getID();
            
            if(mmB.get()->get_backhaul_Type()==Backhaul::wired)
                continue;
            
            double x = mmB.get()->getX();
            double y = mmB.get()->getY();
            double max_snr = -1.0;
            uint32_t parent = def_Nothing;
            for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it2=m_vector_BSs.begin(); it2!=m_vector_BSs.end();++it2)
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
                    
                    if(b_snr && b_parent && b_dist)
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
}

void Manager::path_selection_HQF()
{
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
        {
            std::shared_ptr<mmWaveBS> mmB = (*it);
            uint32_t cid = mmB.get()->getID();
            
            if(mmB.get()->get_backhaul_Type()==Backhaul::wired)
                continue;
            
            double x = mmB.get()->getX();
            double y = mmB.get()->getY();
            double max_snr = -1.0;
            uint32_t parent = def_Nothing;
            for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it2=m_vector_BSs.begin(); it2!=m_vector_BSs.end();++it2)
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
                    
                    if(b_snr && b_parent && b_dist)
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
}



void Manager::set_hop_counts()
{
    // set hop count of the nodes
    bool finish = false;
    int counter = 0;
    while((!finish) && (counter<10))
    {
        bool all_found = true;
        for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
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
}
