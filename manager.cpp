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
#include "common.h"

Manager::Manager()
{
// 	m_painter = p;
    gen_wired = std::mt19937(rd());
    gen_IAB = std::mt19937(rd());  //TODO is it independent from the above?
    std::cout << "Manager started!\n";
}

Manager::~Manager()
{
// 	stop_thread = true;
// 	if(m_draw_thread.joinable()) m_draw_thread.join();
	std::cout << "Deconstruct " << __FILE__ << std::endl;
}

void Manager::set_center(double x, double y, double r)
{
    center_x = x;
    center_y = y;
    radius = r;
}


void Manager::generate_nodes(bool fixed, double wired_density)
{
    //   Create the nodes
    std::uniform_real_distribution<> dis(0, 1);
    std::poisson_distribution<int> pd(1e6*lambda_SBS);
    int num_nodes=pd(gen_IAB); // Poisson number of points
    //     int num_nodes = 100; // Poisson number of points
    
    for(int i =0;i<num_nodes;i++)
    {
        double theta=2*M_PI*(dis(gen_IAB));   // angular coordinates
        double rho=radius*sqrt(dis(gen_IAB));      // radial coordinates
        
        double x = center_x + rho * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + rho * sin(theta);
        
        std::shared_ptr<mmWaveBS> BS;
        BS = std::make_shared<mmWaveBS>(x,y, get_nextID(),  def_P_tx);
        BS.get()->setColor(0);
        m_tree.insert(std::make_pair(BS->get_loc(), BS));
        m_vector_BSs.push_back(BS);
        BS.get()->update_parent.connect_member(this, &Manager::listen_For_parent_update);
        if(fixed)
            BS->set_backhaul_Type(Backhaul::IAB);
        else
        {
            bool prob = (rand() % 100) < 100*wired_density;
            if(prob)
                BS->set_backhaul_Type(Backhaul::wired);
            else
                BS->set_backhaul_Type(Backhaul::wired);
        }
    }
}

void Manager::generate_fixed_nodes(int count)
{
    // Adding Fixed Wired BSs
    double delta_teta = 2*M_PI/count;
    for(int i =0;i<count;i++)
    {
        std::shared_ptr<mmWaveBS> BS;
        double theta = i*delta_teta;
        double r2 = radius/2.;
        double x = center_x + r2 * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + r2 * sin(theta);
        BS = std::make_shared<mmWaveBS>(x, y, get_nextID(),  def_P_tx);
        BS.get()->setColor(0);
        BS->set_backhaul_Type(Backhaul::wired);
        m_tree.insert(std::make_pair(BS->get_loc(), BS));
        m_vector_BSs.push_back(BS);
        BS.get()->update_parent.connect_member(this, &Manager::listen_For_parent_update);
    }
}

/**
 * For scenario of fixed wired nodes
 */
void Manager::update_locations()
{
    std::uniform_real_distribution<> dis(0, 1);
    
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
    {
        std::shared_ptr<mmWaveBS> mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::wired)
            continue;
        
        //TODO it might not work!
        m_tree.remove(std::make_pair(mmB->get_loc(), mmB));
        
        double theta=2*M_PI*(dis(gen_IAB));   // angular coordinates
        double rho=radius*sqrt(dis(gen_IAB));      // radial coordinates
        
        double x = center_x + rho * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + rho * sin(theta);
        mmB->set_loc((float)x,(float)y);
        
        m_tree.insert(std::make_pair(mmB->get_loc(), mmB));
    }
}

/*
 * For scenario of variable location wired nodes
 */
void Manager::update_locations(bool fixed, double wired_density)
{
    std::uniform_real_distribution<> dis(0, 1);
    
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
    {
        std::shared_ptr<mmWaveBS> mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::wired && fixed)
            continue;
        
        //TODO it might not work!
        m_tree.remove(std::make_pair(mmB->get_loc(), mmB));
        
        //TODO for now IAB and wired have the same distro but different density
        double theta=2*M_PI*(dis(gen_IAB));   // angular coordinates
        double rho=radius*sqrt(dis(gen_IAB));      // radial coordinates
        
        double x = center_x + rho * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + rho * sin(theta);
        mmB->set_loc((float)x, (float)y);
        
        bool prob = (rand() % 100) < 100*wired_density;
        if(prob)
            mmB->set_backhaul_Type(Backhaul::wired);
        else
            mmB->set_backhaul_Type(Backhaul::IAB);
        
        m_tree.insert(std::make_pair(mmB->get_loc(), mmB));
    }
    
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
            
//             double x = mmB.get()->getX();
//             double y = mmB.get()->getY();
            double max_snr = -1.0;
            uint32_t parent = def_Nothing;
            
            // search for nearest neighbours
            std::vector<value> results;
            point sought = mmB->get_loc();
            m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < def_MAX_MMWAVE_RANGE;}),
                        std::back_inserter(results));
            
            BOOST_FOREACH(value const&v, results)
            {
                std::shared_ptr<mmWaveBS> mmB2 = std::dynamic_pointer_cast<mmWaveBS>(v.second);
                if(mmB2.get()->getID() != cid)
                { 
//                     double dist = mmB->calculate_distance_of_link(mmB2->getX(), mmB2->getY());
                    double snr = mmB->calculate_SNR_of_link(mmB2->getX(), mmB2->getY());
                    // Rules
                    bool b_snr = snr>max_snr;
                    bool b_parent = mmB2.get()->get_IAB_parent()!=mmB.get()->getID();
                    bool b_wired = mmB2->get_backhaul_Type()==Backhaul::wired;
//                     bool b_dist = dist<def_MAX_MMWAVE_RANGE;

                    if(b_wired)
                    {
                        parent = mmB2.get()->getID();
                        break;
                    }
                    
                    if(b_snr && b_parent)
                    {
                        max_snr = snr;
                        parent = mmB2.get()->getID();
                    }
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
            
//             double x = mmB.get()->getX();
//             double y = mmB.get()->getY();
            double max_snr = -1.0;
            uint32_t parent = def_Nothing;
            
            // search for nearest neighbours
            std::vector<value> results;
            point sought = mmB->get_loc();
            m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < def_MAX_MMWAVE_RANGE;}),
                        std::back_inserter(results));
            
            BOOST_FOREACH(value const&v, results)
            {
                std::shared_ptr<mmWaveBS> mmB2 = std::dynamic_pointer_cast<mmWaveBS>(v.second);
                if(mmB2.get()->getID() != cid)
                { 
//                     double dist = mmB->calculate_distance_of_link(mmB2->getX(), mmB2->getY());
                    double x = mmB2->getX(); double y = mmB2->getY();
                    double snr = mmB->calculate_SNR_of_link(x,y);
                    // Rules
                    bool b_snr = snr>max_snr;
                    bool b_parent = mmB2.get()->get_IAB_parent()!=mmB.get()->getID();
//                     bool b_wired = mmB2->get_backhaul_Type()==Backhaul::wired;
//                     bool b_dist = dist<def_MAX_MMWAVE_RANGE;
                    
                    if(b_snr && b_parent)
                    {
                        max_snr = snr;
                        parent = mmB2.get()->getID();
                    }
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
            {
//                 std::cout << mmB->get_hop_count() << std::endl;
                mmB->emit_update_parent();
            }
            else
                all_found = false;
        }
        
        if(all_found)
            finish = true;
        counter++;
    }
}

int Manager::get_IAB_count()
{
    int cc = 0;
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        std::shared_ptr<mmWaveBS> mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::IAB)
            cc++;
    }
    return cc;
}

int Manager::get_wired_count()
{
    int cc = 0;
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        std::shared_ptr<mmWaveBS> mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::wired)
            cc++;
    }
    return cc;
};

void Manager::reset(bool fixed)
{
    for(std::vector<std::shared_ptr<mmWaveBS>>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        std::shared_ptr<mmWaveBS> mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::IAB && fixed)
            mmB->reset();
        else
            mmB->reset();
    }
}
