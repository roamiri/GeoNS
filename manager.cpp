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
#include "boost/math/special_functions/log1p.hpp"
#include <algorithm>
#include <iterator>
#include <bits/stdc++.h> 
#include <fstream>
#include <sstream>


namespace bm = boost::math;

Manager::Manager(std::string svg_name)
{
    gen_wired = std::mt19937(rd());
    gen_IAB = std::mt19937(rd());  //TODO is it independent from the above?
    
    m_painter = new Painter(svg_name);
//     m_painter->Start();
    
    std::cout << "Manager started!\n";
}

Manager::~Manager()
{
// 	stop_thread = true;
// 	if(m_draw_thread.joinable()) m_draw_thread.join();
//     for(int i=0;i<m_vector_BSs.size();++i)
//         std::cout << i << "="<< m_vector_BSs[i].use_count() << ", ";
//     std::cout << "\n";
    
    delete m_painter;
// 	std::cout << "Deconstruct " << __FILE__ << std::endl;
}

void Manager::set_center(double x, double y, double r)
{
    center_x = x;
    center_y = y;
    radius = r;
}

bool Manager::check_neighbors(double x, double y)
{
    bool ans = true;
    
    // search for nearest neighbours
    std::vector<value> results;
    float xx = (float) x;
    float yy = (float) y;
    point sought(xx,yy);
    m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < def_min_dis;}),
                std::back_inserter(results));
    if(results.size()>0) ans = false;
    
    return ans;
}


int Manager::tree_size(double r)
{
    std::vector<value> results;
    float xx = (float) center_x;
    float yy = (float) center_y;
    point sought(xx,yy);
    m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < r;}),
                std::back_inserter(results));
    
    return results.size();
}


void Manager::generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin)
{
    if(fixed) generate_fixed_nodes(fixed_count);
    
    //   Create the nodes
    std::uniform_real_distribution<> dis(0, 1);
    std::poisson_distribution<int> pd(1e6*node_density);
    int num_nodes=pd(gen_IAB); // Poisson number of points
    //     int num_nodes = 100; // Poisson number of points
    
    for(int i =0;i<num_nodes;i++)
    {
        double theta=2*M_PI*(dis(gen_IAB));   // angular coordinates
        double rho=radius*sqrt(dis(gen_IAB));      // radial coordinates
        
        double x = center_x + rho * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + rho * sin(theta);
        
        bool vicinity = check_neighbors(x,y);
        
        if(vicinity)
        {
            bs_ptr BS;
            BS = boost::shared_ptr<mmWaveBS>(new mmWaveBS(x,y, get_nextID(),  def_P_tx));
            if(!BS) std::cerr << __FUNCTION__ << std::endl;
            BS.get()->setColor(0);
            m_tree.insert(std::make_pair(BS->get_loc(), BS)); //TODO maybe here!
            m_vector_BSs.push_back(BS);
//             BS.get()->update_parent.connect_member(this, &Manager::listen_For_parent_update);
            if(fixed)
            {
                BS->set_backhaul_Type(Backhaul::IAB);
            }
            else
            {
                bool prob = (rand() % 100) < 100*wired_fractoin;
                if(prob)
                {
                    BS->set_backhaul_Type(Backhaul::wired);
                    BS->set_hop_count(0); BS->route_found(true);
                }
                else
                    BS->set_backhaul_Type(Backhaul::IAB);
            }
        }
    }
}

void Manager::load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin)
{
    int num_nodes =0;
// 	double data[num_nodes][2];
    std::vector<std::array<double,2>> vec_data;
	std::fstream _file(f_name);

	if(_file.fail())
    {
		std::cerr << "input data file does not exis!" << std::endl;
    }
    std::string line;
	while(std::getline(_file, line) && num_nodes<200)
	{
		std::istringstream is(line);
		double num;
        std::array<double,2> arr;
        is >> arr[0];
        is>>arr[1];
        vec_data.push_back(arr);
        num_nodes++;
	}
	
	
	if(fixed) generate_fixed_nodes(fixed_count);
    
    //   Create the nodes
    
    num_nodes= vec_data.size(); // number of points
    
    for(int i =0;i<num_nodes;i++)
    {
        double x = center_x + 1.5*radius* vec_data[i][0];  // Convert from polar to Cartesian coordinates
        double y = center_y + 1.5*radius* vec_data[i][1];
        
        bool vicinity = check_neighbors(x,y);
        
        if(vicinity)
        {
            bs_ptr BS;
            BS = boost::shared_ptr<mmWaveBS>(new mmWaveBS(x,y, get_nextID(),  def_P_tx));
            if(!BS) std::cerr << __FUNCTION__ << " at " << __LINE__ <<std::endl;
            BS.get()->setColor(0);
            m_tree.insert(std::make_pair(BS->get_loc(), BS)); //TODO maybe here!
            m_vector_BSs.push_back(BS);
//             BS.get()->update_parent.connect_member(this, &Manager::listen_For_parent_update);
            if(fixed)
            {
                BS->set_backhaul_Type(Backhaul::IAB);
            }
            else
            {
                bool prob = (rand() % 100) < 100*wired_fractoin;
                if(prob)
                {
                    BS->set_backhaul_Type(Backhaul::wired);
                    BS->set_hop_count(0); BS->route_found(true);
                }
                else
                    BS->set_backhaul_Type(Backhaul::IAB);
            }
        }
    }
}

void Manager::generate_fixed_nodes(int count)
{
    // Adding Fixed Wired BSs
    double delta_teta = 2*M_PI/count;
    for(int i =0;i<count;i++)
    {
        bs_ptr BS;
        double theta = i*delta_teta;
        double r2 = radius/2.;
        double x = center_x + r2 * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + r2 * sin(theta);
        BS = boost::shared_ptr<mmWaveBS>(new mmWaveBS(x, y, get_nextID(),  def_P_tx));
        BS.get()->setColor(0);
        BS->set_backhaul_Type(Backhaul::wired);
        BS->set_hop_count(0);
        BS->route_found(true);
        m_tree.insert(std::make_pair(BS->get_loc(), BS));
        m_vector_BSs.push_back(BS);
//         BS.get()->update_parent.connect_member(this, &Manager::listen_For_parent_update);
    }
}


/**
 * For scenario of fixed wired nodes
 */
// void Manager::update_locations()
// {
//     std::uniform_real_distribution<> dis(0, 1);
//     
//     for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
//     {
//         bs_ptr mmB = (*it);
//         if(mmB->get_backhaul_Type()==Backhaul::wired)
//             continue;
//         
//         
//         //TODO it might not work!
//         m_tree.remove(std::make_pair(mmB->get_loc(), mmB));
// //         std::cout << "Tree size = " << tree_size(1000) << std::endl;
//         double theta=2*M_PI*(dis(gen_IAB));   // angular coordinates
//         double rho=radius*sqrt(dis(gen_IAB));      // radial coordinates
//         
//         double x = center_x + rho * cos(theta);  // Convert from polar to Cartesian coordinates
//         double y = center_y + rho * sin(theta);
//         mmB->set_loc((float)x,(float)y);
//         
//         m_tree.insert(std::make_pair(mmB->get_loc(), mmB));
// //         std::cout << "Tree size = " << tree_size(1000) << std::endl;
//     }
// }

/*
 * For scenario of variable location wired nodes
 */
void Manager::update_locations(bool fixed, double wired_fractoin)
{
    std::uniform_real_distribution<> dis(0, 1);
    
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB) std::cerr << "FUCK!!!" << std::endl;
        mmB->reset_load();
        if(mmB->get_backhaul_Type()==Backhaul::wired && fixed)
            continue;        
        mmB->route_found(false);
//         std::cout << "Tree size = " << tree_size(1000) << std::endl;
        //TODO it might not work!
//         std::cout << "use count before = " << mmB.use_count();
        m_tree.remove(std::make_pair(mmB->get_loc(), mmB));
//         std::cout << "use count after = " << mmB.use_count();
//         std::cout << "Tree size = " << tree_size(1000) << std::endl;
        
        //TODO for now IAB and wired have the same distro but different density
        double theta=2*M_PI*(dis(gen_IAB));   // angular coordinates
        double rho=radius*sqrt(dis(gen_IAB));      // radial coordinates
        
        double x = center_x + rho * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + rho * sin(theta);
        
        mmB->set_loc((float)x, (float)y);
        mmB->reset(); // sets hop count = -1 and no parent
        bool prob = (rand() % 100) < 100*wired_fractoin;
        if(fixed)
            mmB->set_backhaul_Type(Backhaul::IAB);
        else{ 
            if(prob)
            {
                mmB->set_backhaul_Type(Backhaul::wired);
                mmB->set_hop_count(0); mmB->route_found(true);
            }
            else
            {
                mmB->set_backhaul_Type(Backhaul::IAB);
            }
        }
        
        m_tree.insert(std::make_pair(mmB->get_loc(), mmB));
//         std::cout << "Tree size = " << tree_size(1000) << std::endl;
    }
}


void Manager::listen_For_Candidacy(const candidacy_msg& message)
{
	std::cout << " Candidate received: x=" << message.x << ", y=" << message.y << " from id=" << message.id << std::endl;
	uint32_t candidate_id = message.id;
	double x = message.x;
	double y = message.y;
	bool ib_found = false;
	for(std::vector<bs_ptr>::iterator it = m_vector_BSs.begin(); it != m_vector_BSs.end(); ++it) 
	{
		bs_ptr mmB = (*it);
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
	for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it != m_vector_BSs.end(); ++it)
	{
		bs_ptr mmB = (*it);
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
	for(std::vector<bs_ptr>::iterator it = m_vector_BSs.begin(); it != m_vector_BSs.end(); ++it)
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
	for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
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
    //TODO update the for loop
    uint32_t id = msg.id;
    int hop_cnt = msg.hop_cnt;
    point sought = point(msg.xx, msg.yy);
    
    std::vector<value> results;
    m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < 2*def_MAX_MMWAVE_RANGE;}), std::back_inserter(results));

//     std::lock_guard<std::mutex> guard(m_mutex);
    BOOST_FOREACH(value const&v, results)
    {
        bs_ptr mmB = boost::dynamic_pointer_cast<mmWaveBS>(v.second);
        if(mmB->get_IAB_parent()==id)
        {
            mmB->set_hop_count(hop_cnt+1);
//             mmB->route_found(true);
        }
    }
//     for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
//     {
//         bs_ptr mmB = (*it);
//         if(mmB->get_IAB_parent()==id)
//         {
//             mmB->set_hop_count(hop_cnt+1);
//             mmB->route_found(true);
//         }
//     }
}

void Manager::spread_hop_count()
{
    bool all_found = false;
    int counter = 0;
    while(!all_found && counter<10)
    {
        for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
        {
            bs_ptr mmB = (*it);
            if(mmB->get_hop_count()!=-1)
                mmB->update_load_hops();
            
        //         if(mmB->get_backhaul_Type()==Backhaul::wired)
        //             mmB->update_load_hops(0);
        }
        
        bool found = true;
        
        for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
        {
            bs_ptr mmB = (*it);
            if(mmB->get_hop_count()==-1 && mmB->get_IAB_parent()!=def_Nothing)
            {found = false; break;}
        }
        if(found)
            all_found = true;
        counter++;
    }
    
//     count_hops(m_arr_hops, m_failed);
}

void Manager::set_hop_counts()
{
    // set hop count of the nodes
    bool finish = false;
    int counter = 0;
    while((!finish) && (counter<10))
    {
        bool all_found = true;
//         std::lock_guard<std::mutex> guard(m_mutex);
        for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
        {
            bs_ptr mmB = (*it);
            if(!mmB) std::cerr << __func__ << std::endl;
            
            if(mmB->get_hop_count()!=-1)
            {
//                 std::cout << mmB->get_hop_count() << ",";
//                 mmB->emit_update_parent();
                int h = mmB->get_hop_count(); uint32_t id = mmB->getID();
                for(std::vector<bs_ptr>::iterator it2=m_vector_BSs.begin(); it2!=m_vector_BSs.end();++it2)
                {
                    bs_ptr mmB2 = (*it2);
                    if(mmB2->get_IAB_parent()==id)
                    {
                        mmB2->set_hop_count(h+1);
//                         mmB2->route_found(true);
                    }
                }
//                 std::vector<value> results;
//                 point sought = mmB->get_loc();
//                 m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < 2*def_MAX_MMWAVE_RANGE;}), std::back_inserter(results));
// 
//                 BOOST_FOREACH(value const &v, results)
//                 {
//                     bs_ptr mmB2 = boost::dynamic_pointer_cast<mmWaveBS>(v.second);
//                     if(mmB2->get_IAB_parent()==mmB->getID())
//                     {
//                         mmB2->set_hop_count(mmB->get_hop_count()+1);
//                         mmB2->route_found(true);
//                     }
//                 }
            }
            else
                all_found = false;
        }
        
        if(all_found)
            finish = true;
        counter++;
    }

}


void Manager::draw_svg(bool b)
{
    if(b)
    {
//         m_painter->draw_node_ID(m_vector_BSs);
        m_painter->update(m_vector_BSs);
    }
}

/**
 * Select path based on wired first policy
 */
void Manager::path_selection_WF()
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
    {
        bs_ptr mmB = (*it);
//         std::cout << "use count = " << mmB.use_count();
        if(!mmB) std::cerr << __FUNCTION__ << std::endl;
        
        if(mmB.get()->get_backhaul_Type()==Backhaul::wired)
            continue;
        
        uint32_t cid = mmB.get()->getID();
        double max_snr = -1.0;
        double max_sinr = -1.0;
        uint32_t parent = def_Nothing;
        
        // search for nearest neighbours
        std::vector<value> results;
        point sought = mmB->get_loc();
        m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < def_MAX_MMWAVE_RANGE;}),
                    std::back_inserter(results));
        
        point p1 = mmB->get_loc();
        
        BOOST_FOREACH(value const&v, results)
        {
            bs_ptr mmB2 = boost::dynamic_pointer_cast<mmWaveBS>(v.second);
            uint32_t cid2 = mmB2.get()->getID();
            if( cid2 != cid)
            { 
                double x2 = mmB2->getX(); double y2 = mmB2->getY(); point p2 = mmB2->get_loc();
                polygon2D poly = directional_polygon(p1, p2, mmB->get_phi_m());
                std::vector<value> vec_query;
                m_tree.query(bgi::intersects(poly), std::back_inserter(vec_query));
                double interf=0.;
                BOOST_FOREACH(value const&mz, vec_query)
                {
                    bs_ptr mmB3 = boost::dynamic_pointer_cast<mmWaveBS>(mz.second);
                    uint32_t cid3 = mmB3->getID();
                    if(cid3!=cid2 && cid3!=cid)
                        interf+= mmB->calculate_Interf_of_link(mmB3->getX(), mmB3->getY());
                }
                double snr = mmB->calculate_SNR_of_link(x2, y2);
                double sinr = mmB->calculate_SINR_of_link(x2,y2, interf);
                // Rules
                bool b_snr = snr>max_snr;
                bool b_parent = mmB2.get()->get_IAB_parent()!=mmB.get()->getID();
                bool b_wired = mmB2->get_backhaul_Type()==Backhaul::wired;

//TODO What if there are multiple wired nodes in its vicinity????? //FIXME
                if(b_wired)
                {
                    parent = mmB2.get()->getID();
                    max_sinr = sinr;
                    max_snr = snr;
                    break;
                }
                
                if(b_snr && b_parent)
                {
                    max_snr = snr;
                    max_sinr = sinr;
                    parent = mmB2.get()->getID();
                }
            }
        }
        if(parent!=def_Nothing)
        {
            mmB->set_IAB_parent(parent);
            mmB->set_SNR(max_snr);
            mmB->set_SINR(max_sinr);
            Add_load_BS(parent, mmB);
        }
    }
}

void Manager::path_selection_HQF_Interf()
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB) std::cerr << __FUNCTION__ << std::endl;
        
        if(mmB.get()->get_backhaul_Type()==Backhaul::wired)
            continue;
        
        uint32_t cid = mmB.get()->getID();
        double max_sinr = -1.0;
        double max_snr = -1.0;
        uint32_t parent = def_Nothing;
        
        // search for nearest neighbours
        std::vector<value> results;
        point sought = mmB->get_loc();
        m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < def_MAX_MMWAVE_RANGE;}),
                    std::back_inserter(results));
        
        double x1 = mmB->getX(); double y1 = mmB->getY(); point p1 = mmB->get_loc(); 
//         std::cout << "cid1 = " << cid << std::endl;
        BOOST_FOREACH(value const&v, results)
        {
            bs_ptr mmB2 = boost::dynamic_pointer_cast<mmWaveBS>(v.second);
            if(mmB2)
            {
                uint32_t cid2 = mmB2.get()->getID();
//                 std::cout << "cid2 = " << cid2 << std::endl;
                if( cid2!= cid)
                {
                    double x2 = mmB2->getX(); double y2 = mmB2->getY(); point p2 = mmB2->get_loc();
                    polygon2D poly = directional_polygon(p1, p2, mmB->get_phi_m());
                    std::vector<value> vec_query;
                    m_tree.query(bgi::intersects(poly), std::back_inserter(vec_query));
                    double interf=0.;
                    BOOST_FOREACH(value const&mz, vec_query)
                    {
                        bs_ptr mmB3 = boost::dynamic_pointer_cast<mmWaveBS>(mz.second);
                        uint32_t cid3 = mmB3->getID();
//                         std::cout << "cid3 = " << cid3 << std::endl;
                        if(cid3!=cid2 && cid3!=cid)
                            interf+= mmB->calculate_Interf_of_link(mmB3->getX(), mmB3->getY());
                    }
                    
                    double snr = mmB->calculate_SNR_of_link(x2,y2);
                    double sinr = mmB->calculate_SINR_of_link(x2,y2, interf);
//                     // Rules
                    bool b_snr = sinr>max_sinr;
                    bool b_parent = mmB2.get()->get_IAB_parent()!=mmB.get()->getID();
                    
                    if(b_snr && b_parent)
                    {
                        max_sinr = sinr;
                        max_snr = snr;
                        parent = mmB2.get()->getID();
                    }
                }
            }
        }
        if(parent!=def_Nothing)
        {
            mmB->set_IAB_parent(parent);
            mmB->set_SINR(max_sinr);
            mmB->set_SNR(max_snr);
            Add_load_BS(parent, mmB);
        }
    }
}

/**
 * Returns the directional triangle from p1 tp p2 considering angle of main lobe phi_m
 * phi_m should be in radian
 */
polygon2D Manager::directional_polygon(point p1, point p2, double phi_m)
{
    polygon2D poly;
    // first vertice
    poly.outer().push_back(p1);
    
    // Two other vertices
    double phi = atan2(p2.get<1>()-p1.get<1>(), p2.get<0>()-p1.get<0>());
    double t1 = phi - phi_m/2.;
    double t2 = phi + phi_m/2.;
    double r = def_MAX_MMWAVE_RANGE;
    point p3(p1.get<0>()+r*cos(t1), p1.get<1>()+r*sin(t1));
    point p4(p1.get<0>()+r*cos(t2), p1.get<1>()+r*sin(t2));
    poly.outer().push_back(p3);
    poly.outer().push_back(p4);
    
    // last vertice
    poly.outer().push_back(p1);
    return poly;
}

void Manager::path_selection_HQF()
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB) std::cerr << __FUNCTION__ << std::endl;
        
        if(mmB.get()->get_backhaul_Type()==Backhaul::wired)
            continue;
        
        uint32_t cid = mmB.get()->getID();
        double max_snr = -1.0; double max_sinr = -1.0;
        uint32_t parent = def_Nothing;
        
        // search for nearest neighbours
        std::vector<value> results;
        point sought = mmB->get_loc();
        m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < def_MAX_MMWAVE_RANGE;}),
                    std::back_inserter(results));
        
        point p1 = mmB->get_loc();
        BOOST_FOREACH(value const&v, results)
        {
            bs_ptr mmB2 = boost::dynamic_pointer_cast<mmWaveBS>(v.second);
            uint32_t cid2 = mmB2.get()->getID();
            if( cid2 != cid)
            { 
                double x2 = mmB2->getX(); double y2 = mmB2->getY(); point p2 = mmB2->get_loc();
                polygon2D poly = directional_polygon(p1, p2, mmB->get_phi_m());
                std::vector<value> vec_query;
                m_tree.query(bgi::intersects(poly), std::back_inserter(vec_query));
                double interf=0.;
                BOOST_FOREACH(value const&mz, vec_query)
                {
                    bs_ptr mmB3 = boost::dynamic_pointer_cast<mmWaveBS>(mz.second);
                    uint32_t cid3 = mmB3->getID();
                    if(cid3!=cid2 && cid3!=cid)
                        interf+= mmB->calculate_Interf_of_link(mmB3->getX(), mmB3->getY());
                }
                double snr = mmB->calculate_SNR_of_link(x2,y2);
                double sinr = mmB->calculate_SINR_of_link(x2,y2, interf);
                // Rules
                bool b_snr = snr>max_snr;
                bool b_parent = mmB2.get()->get_IAB_parent()!=mmB.get()->getID();
                
                if(b_snr && b_parent)
                {
                    max_snr = snr;
                    max_sinr = sinr;
                    parent = mmB2.get()->getID();
                }
            }
        }
        
        if(parent!=def_Nothing)
        {
            mmB->set_IAB_parent(parent);
            mmB->set_SNR(max_snr);
            mmB->set_SINR(max_sinr);
            Add_load_BS(parent, mmB);
        }
    }
}

/**
 * Select path based on position-aware policy
 */
void Manager::path_selection_PA()
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    //TODO check this method precisely.
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB) std::cerr << __FUNCTION__ << std::endl;
        
        if(mmB.get()->get_backhaul_Type()==Backhaul::wired)
            continue;
        
        uint32_t cid = mmB.get()->getID();
        double max_snr = -1.0;
        double max_sinr = -1.0;
        uint32_t parent = def_Nothing;
        
        point closest_wired = find_closest_wired(mmB->getID(), mmB->get_loc());
        double dist_wired = bg::distance(closest_wired, mmB->get_loc());
        // search for nearest neighbours
        std::vector<value> results;
        point sought = mmB->get_loc();
        m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < def_MAX_MMWAVE_RANGE;}),
                    std::back_inserter(results));
        
        point p1 = mmB->get_loc();
        BOOST_FOREACH(value const&v, results)
        {
            bs_ptr mmB2 = boost::dynamic_pointer_cast<mmWaveBS>(v.second);
            uint32_t cid2 = mmB2.get()->getID();
            if( cid2 != cid)
            {
                double x2 = mmB2->getX(); double y2 = mmB2->getY(); point p2 = mmB2->get_loc();
                polygon2D poly = directional_polygon(p1, p2, mmB->get_phi_m());
                std::vector<value> vec_query;
                m_tree.query(bgi::intersects(poly), std::back_inserter(vec_query));
                double interf=0.;
                BOOST_FOREACH(value const&mz, vec_query)
                {
                    bs_ptr mmB3 = boost::dynamic_pointer_cast<mmWaveBS>(mz.second);
                    uint32_t cid3 = mmB3->getID();
                    if(cid3!=cid2 && cid3!=cid)
                        interf+= mmB->calculate_Interf_of_link(mmB3->getX(), mmB3->getY());
                }
                double snr = mmB->calculate_SNR_of_link(x2, y2);
                double sinr = mmB->calculate_SINR_of_link(x2, y2, interf);
                // Rules
                bool b_snr = snr>max_snr;
                bool b_parent = mmB2.get()->get_IAB_parent()!=mmB.get()->getID();
                bool b_dist = bg::distance(closest_wired, mmB2->get_loc()) <= dist_wired;
                
                if(b_snr && b_parent && b_dist)
                {
                    max_snr = snr;
                    max_sinr = sinr;
                    parent = mmB2.get()->getID();
                }
            }
        }
        
        if(parent!=def_Nothing)
        {
            mmB->set_IAB_parent(parent);
            mmB->set_SNR(max_snr);
            mmB->set_SINR(max_sinr);
            Add_load_BS(parent, mmB);
        }
    }
}


point Manager::find_closest_wired(uint32_t id, point loc)
{
    //TODO check if the answer is not equal the input loc
    point wired;// = loc;
    double search_radius = 10; // 10 meter first search radius
    int max = radius/search_radius;
    
    for(int i=1;i<max;i++)
    {
        search_radius=i*search_radius;
        // search for nearest neighbours
        std::vector<value> results;
        m_tree.query(bgi::satisfies([&](value const& v) {return bg::distance(v.first, loc) < search_radius;}), std::back_inserter(results));
        double dis = 3*radius; uint32_t cid; bool b_found = false;
        BOOST_FOREACH(value const&v, results)
        {
            bs_ptr mmB2 = boost::dynamic_pointer_cast<mmWaveBS>(v.second);
            if(mmB2->getID()!=id)
            {
                if(mmB2->get_backhaul_Type()==Backhaul::wired)
                {
                    if(bg::distance(loc, mmB2->get_loc()) < dis)
                    {
                        wired = mmB2->get_loc();
                        dis = bg::distance(loc, mmB2->get_loc());
                        cid = mmB2->getID(); 
                        b_found = true;
                    }
                }
            }
        }
        if(b_found)
        {
//             std::cout << "closest wired id for " << id  << " is " << cid << std::endl;
            return wired;
        }
        
    }
    
    std::cerr << "No wired found!!" << std::endl;
//     return wired;
}


/**
 * Path selection policy based on Maximum local rate
 */
void Manager::path_selection_MLR()
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    //TODO maybe here!
//     bool b_all_nodes_route = false;
//     int count = 0;
//     while(!b_all_nodes_route && count<10)
//     {
        for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end();++it)
        {
            bs_ptr mmB = (*it);
            if(!mmB) std::cerr << __FUNCTION__ << std::endl;
            
            if(mmB.get()->get_backhaul_Type()==Backhaul::wired)
                continue;
            
            uint32_t cid = mmB.get()->getID();
            double max_rate = -1.0;
            uint32_t parent_id = def_Nothing;
        
            // search for nearest neighbours
            point sought = mmB->get_loc();
            std::vector<value> results;
            m_tree.query(bgi::satisfies([&](value const&v){return bg::distance(v.first, sought)<def_MAX_MMWAVE_RANGE;}), std::back_inserter(results));
            
            bs_ptr parent;
            BOOST_FOREACH(value const&v, results)
            {
                bs_ptr mmB2 = boost::dynamic_pointer_cast<mmWaveBS>(v.second);
//                 if(mmB2->get_backhaul_Type()==Backhaul::wired)
// //                     std::cout << "here\n";
                if(cid != mmB2.get()->getID())
                {
                    double snr = mmB.get()->calculate_SNR_of_link(mmB2.get()->getX(), mmB2.get()->getY());
                    double load = 1. + mmB2.get()->get_load_BS_count();
                    double BW =def_BW;
                    double d = def_BW/load;
                    double rate =d * bm::log1p(1.+snr)/bm::log1p(2.0);
    //                 std::cout << "rate= " << rate << ", ";
                    bool b_rate = rate > max_rate;
                    bool b_parent = mmB2.get()->get_IAB_parent()!=cid;
                    
                    if(b_rate && b_parent)
                    {
                        max_rate = rate;
                        parent_id = mmB2.get()->getID();
                        parent = mmB2;
                    }
                }
            }
            if(parent_id!=def_Nothing)
            {
                mmB->set_IAB_parent(parent_id);
//                 mmB->set_SNR(max_snr);
//             mmB->set_SINR(max_sinr);
                Add_load_BS(parent_id, mmB);
            }
        }
        
//         b_all_nodes_route = check_all_routes();
//         count++;
//     }
//     std::cout << "The route for all nodes was found = " << b_all_nodes_route << " with " << count << " tries!" <<std::endl;
}

bool Manager::check_all_routes()
{
    bool found = true;
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB->is_route_found())
        {found = false; break;}
    }
    return found;
}

void Manager::Add_load_BS(uint32_t parent, bs_ptr bs/*uint32_t member, point loc*/)
{
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->getID()==parent)
        {
            if(mmB->getID()!=bs->get_IAB_parent())
                std::cerr << "WTF!\n";
            mmB->add_to_load_BS(bs);
        }
    }
}


int Manager::get_IAB_count()
{
    int cc = 0;
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::IAB)
            cc++;
    }
    return cc;
}

int Manager::get_wired_count()
{
    int cc = 0;
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::wired)
            cc++;
    }
    return cc;
};

void Manager::reset(bool fixed)
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::IAB && fixed)
            mmB->reset();
        else
            mmB->reset();
    }
}

std::vector<int> Manager::count_hops(int &max_hop, int &failed)
{
    
    m_failed = 0; m_max_hop = -1;
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(m_max_hop < mmB->get_hop_count())
            m_max_hop = mmB->get_hop_count();
    }
//     memset(m_arr_hops, 0, sizeof(m_arr_hops));
    std::vector<int> arr_hops; arr_hops.resize(m_max_hop+1);
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB)
        {
            std::cerr << "NULL Pointer in " << __FUNCTION__ << " at " << __LINE__ << std::endl;
//             continue;
        }
        int hcnt = mmB->get_hop_count();
        if(hcnt != -1)
        {
            arr_hops[hcnt]++;
        }
        else
            m_failed++;
    }
    failed = m_failed;
    max_hop = m_max_hop;
    return arr_hops;
//     std::copy(std::begin(m_arr_hops), std::end(m_arr_hops), std::begin(hop));
//     hop = m_arr_hops;
//     std::vector<value> results;
//     point sought = point(center_x, center_y);
//     m_tree.query(bgi::satisfies([&] (value const&v){return bg::distance(v.first,sought)<radius;}), std::back_inserter(results));
//     
//     BOOST_FOREACH(value const&v, results)
//     {
//         bs_ptr mmB = boost::dynamic_pointer_cast<mmWaveBS>(v.second);
//         if(!mmB)
//         {
//             std::cerr << "NULL Pointer " << __FUNCTION__ << __LINE__ << std::endl;
// //             continue;
//         }
//         int hcnt = mmB->get_hop_count();
//         if(hcnt != -1)
//             hop[hcnt]++;
//         else
//             failed++;
//         
//     }
//     std::lock_guard<std::mutex> guard(m_mutex);
//     
}


void Manager::reset_pointers()
{
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB)
            mmB.reset();
    }
}

double Manager::find_SNR_bottleneck()
{
    double bottleneck = 1e10;
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->is_route_found() && mmB->get_backhaul_Type()==Backhaul::IAB)
        {
            double snr = mmB->get_SNR();
            if(snr==0.)
                std::cout << "here\n";
            if(snr<bottleneck)
                bottleneck = snr;
        }
            
    }
    bottleneck = 10*log10(bottleneck); // returning the SNR value in dB
    return bottleneck;
}


double Manager::find_SINR_bottleneck()
{
    double bottleneck = 1e10;
    for(std::vector<bs_ptr>::iterator it=m_vector_BSs.begin(); it!=m_vector_BSs.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->is_route_found() && mmB->get_backhaul_Type()==Backhaul::IAB)
        {
            double sinr = mmB->get_SINR();
            if(sinr==0.)
                std::cout << "here\n";
            if(sinr<bottleneck)
                bottleneck = sinr;
        }
            
    }
    bottleneck = 10*log10(bottleneck); // returning the SNR value in dB
    return bottleneck;
}
