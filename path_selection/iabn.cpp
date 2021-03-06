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

#include <math.h>
#include <limits>
#include <random>
#include "boost/math/special_functions/log1p.hpp"

#include <algorithm>
#include <iterator>
#include <bits/stdc++.h> 
#include <fstream>
#include <sstream>

#include "iabn.h"
#include "common.h"
#include "painter.h"
#include <boost/progress.hpp>

namespace bm = boost::math;

IABN::IABN(): Container()
{
    gen_wired = std::mt19937(rd());
    gen_IAB = std::mt19937(rd());  //TODO is it independent from the above?
    
//     m_painter = new Painter(svg_name);
//     m_painter->Start();
    
    std::cout << "IABN started!\n";
}

IABN::~IABN()
{

}

void IABN::generate_nodes(double area_coeff, bool b_tree)
{
    //   Create the nodes
    double node_density = 1e-4;
    std::uniform_real_distribution<> dis(0, 1);
    std::poisson_distribution<int> pd(area_coeff*def_Area*node_density);
    int num_nodes=pd(gen_IAB); // Poisson number of points
    std::cout << "Number of nodes before vicinity = " << num_nodes << std::endl;
    
    // Generate data on a disk with radius r with poisson point process    
    double area = area_coeff*def_Area*node_density;
    double r = sqrt(1/M_PI)*sqrt(area); // radius of the disk
    double xx0=r; double yy0=r;    // center of the disk
    set_center(xx0, yy0, r);
    std::cout << "center=" << xx0 << ", " << yy0 << ", " << r << std::endl; 
//     if(fixed) generate_fixed_nodes(fixed_count);
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
            BS = boost::shared_ptr<mmWaveBS>(new mmWaveBS(x,y, get_nextID()));
            BS->set_transmit_power(def_P_tx);
            if(!BS) std::cerr << __FUNCTION__ << std::endl;
            BS.get()->setColor(0);
            if(b_tree) m_tree.insert(std::make_pair(BS->get_loc(), BS));
            m_items.push_back(BS);
        }
    }
}

void IABN::generate_nodes(double node_density, bool fixed, int fixed_count, double wired_fractoin)
{
    
    //   Create the nodes
    std::uniform_real_distribution<> dis(0, 1);
    std::poisson_distribution<int> pd(def_Area*node_density);
    int num_nodes=pd(gen_IAB); // Poisson number of points
    std::cout << "Number of nodes before vicinity = " << num_nodes << std::endl;
    //     int num_nodes = 100; // Poisson number of points
    // Generate data on a disk with radius r with poisson point process    
    double area = def_Area*node_density;
    double r = sqrt(1/M_PI)*sqrt(area); // radius of the disk
    double xx0=r; double yy0=r;    // center of the disk
    set_center(xx0, yy0, r);
    std::cout << "center=" << xx0 << ", " << yy0 << ", " << r << std::endl; 
    if(fixed) generate_fixed_nodes(fixed_count);
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
            BS = boost::shared_ptr<mmWaveBS>(new mmWaveBS(x,y, get_nextID()));
            BS->set_transmit_power(def_P_tx);
            if(!BS) std::cerr << __FUNCTION__ << std::endl;
            BS.get()->setColor(0);
            m_tree.insert(std::make_pair(BS->get_loc(), BS)); //TODO maybe here!
            m_items.push_back(BS);
//             BS.get()->update_parent.connect_member(this, &IABN::listen_For_parent_update);
            if(fixed)
            {
                BS->set_backhaul_Type(Backhaul::wireless);
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
                    BS->set_backhaul_Type(Backhaul::wireless);
            }
        }
    }
}

void IABN::load_nodes(std::string f_name, bool fixed, int fixed_count, double wired_fractoin)
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
        double x = center_x + radius* vec_data[i][0];  // Convert from polar to Cartesian coordinates
        double y = center_y + radius* vec_data[i][1];
        
        bool vicinity = check_neighbors(x,y);
        
        if(vicinity)
        {
            bs_ptr BS;
            BS = boost::shared_ptr<mmWaveBS>(new mmWaveBS(x,y, get_nextID()));
            BS->set_transmit_power(def_P_tx);
            if(!BS) std::cerr << __FUNCTION__ << " at " << __LINE__ <<std::endl;
            BS.get()->setColor(0);
            m_tree.insert(std::make_pair(BS->get_loc(), BS)); //TODO maybe here!
            m_items.push_back(BS);
//             BS.get()->update_parent.connect_member(this, &IABN::listen_For_parent_update);
            if(fixed)
            {
                BS->set_backhaul_Type(Backhaul::wireless);
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
                    BS->set_backhaul_Type(Backhaul::wireless);
            }
        }
    }
}

void IABN::generate_fixed_nodes(int count)
{
    // Adding Fixed Wired BSs
    double delta_teta = 2*M_PI/count;
    for(int i =0;i<count;i++)
    {
        bs_ptr BS;
        double theta = i*delta_teta;
        double r2 = radius/4.;
        double x = center_x + r2 * cos(theta);  // Convert from polar to Cartesian coordinates
        double y = center_y + r2 * sin(theta);
        BS = boost::shared_ptr<mmWaveBS>(new mmWaveBS(x, y, get_nextID()));
        BS->set_transmit_power(def_P_tx);
        BS.get()->setColor(0);
        BS->set_backhaul_Type(Backhaul::wired);
        BS->set_hop_count(0);
        BS->route_found(true);
        m_tree.insert(std::make_pair(BS->get_loc(), BS));
        m_items.push_back(BS);
//         BS.get()->update_parent.connect_member(this, &IABN::listen_For_parent_update);
    }
}


/**
 * For scenario of fixed wired nodes
 */
// void IABN::update_locations()
// {
//     std::uniform_real_distribution<> dis(0, 1);
//     
//     for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
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
void IABN::update_locations(bool fixed, double wired_fractoin)
{
    std::uniform_real_distribution<> dis(0, 1);
    
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
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
            mmB->set_backhaul_Type(Backhaul::wireless);
        else{ 
            if(prob)
            {
                mmB->set_backhaul_Type(Backhaul::wired);
                mmB->set_hop_count(0); mmB->route_found(true);
            }
            else
            {
                mmB->set_backhaul_Type(Backhaul::wireless);
            }
        }
        
        m_tree.insert(std::make_pair(mmB->get_loc(), mmB));
//         std::cout << "Tree size = " << tree_size(1000) << std::endl;
    }
}



void IABN::listen_For_parent_update(const update_parent_msg& msg)
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
        if(mmB->get_parent()==id)
        {
            mmB->set_hop_count(hop_cnt+1);
//             mmB->route_found(true);
        }
    }
//     for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
//     {
//         bs_ptr mmB = (*it);
//         if(mmB->get_parent()==id)
//         {
//             mmB->set_hop_count(hop_cnt+1);
//             mmB->route_found(true);
//         }
//     }
}

void IABN::spread_hop_count()
{
    bool all_found = false;
    int counter = 0;
    while(!all_found && counter<10)
    {
        for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
        {
            bs_ptr mmB = (*it);
            if(mmB->get_hop_count()!=-1)
                mmB->update_load_hops();
            
        //         if(mmB->get_backhaul_Type()==Backhaul::wired)
        //             mmB->update_load_hops(0);
        }
        
        bool found = true;
        
        for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
        {
            bs_ptr mmB = (*it);
            if(mmB->get_hop_count()==-1 && mmB->get_parent()!=def_Nothing)
            {found = false; break;}
        }
        if(found)
            all_found = true;
        counter++;
    }
    
//     count_hops(m_arr_hops, m_failed);
}

void IABN::set_hop_counts()
{
    // set hop count of the nodes
    bool finish = false;
    int counter = 0;
    while((!finish) && (counter<10))
    {
        bool all_found = true;
//         std::lock_guard<std::mutex> guard(m_mutex);
        for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
        {
            bs_ptr mmB = (*it);
            if(!mmB) std::cerr << __func__ << std::endl;
            
            if(mmB->get_hop_count()!=-1)
            {
//                 std::cout << mmB->get_hop_count() << ",";
//                 mmB->emit_update_parent();
                int h = mmB->get_hop_count(); uint32_t id = mmB->getID();
                for(std::vector<bs_ptr>::iterator it2=m_items.begin(); it2!=m_items.end();++it2)
                {
                    bs_ptr mmB2 = (*it2);
                    if(mmB2->get_parent()==id)
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
//                     if(mmB2->get_parent()==mmB->getID())
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


void IABN::draw_svg(bool b)
{
    if(b)
    {
//         m_painter->draw_node_ID(m_items);
        m_painter->update<mmWaveBS>(m_items);
    }
}

/**
 * Select path based on wired first policy
 */
void IABN::path_selection_WF()
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
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
                    {
                        double x3 = mmB3->getX(); double y3= mmB3->getY(); point p3 = mmB3->get_loc();
                        double dist = bg::distance(sought, p3);
                        interf+= mmB->calculate_Interf_of_link(x3, y3, dist);
                    }
                }
                double d21 = bg::distance(sought, p2);
                double snr = mmB->calculate_SNR_of_link(x2, y2, d21);
                double sinr = mmB->calculate_SINR_of_link(x2,y2, d21, interf);
                // Rules
                bool b_snr = snr>max_snr;
                bool b_parent = mmB2.get()->get_parent()!=mmB.get()->getID();
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
            mmB->set_parent(parent);
            mmB->set_SNR(max_snr);
            mmB->set_SINR(max_sinr);
            Add_load_BS(parent, mmB);
        }
    }
}

void IABN::path_selection_HQF_Interf()
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
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
                        {
                            double x3 = mmB3->getX(); double y3= mmB3->getY(); point p3 = mmB3->get_loc();
                            double dist = bg::distance(sought, p3);
                            interf+= mmB->calculate_Interf_of_link(x3, y3, dist);
                        }
                    }
                    double d21 = bg::distance(sought, p2);
                    double snr = mmB->calculate_SNR_of_link(x2,y2, d21);
                    double sinr = mmB->calculate_SINR_of_link(x2,y2, d21, interf);
//                     // Rules
                    bool b_snr = sinr>max_sinr;
                    bool b_parent = mmB2.get()->get_parent()!=mmB.get()->getID();
                    
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
            mmB->set_parent(parent);
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
polygon2D IABN::directional_polygon(point p1, point p2, double phi_m)
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

void IABN::path_selection_HQF()
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
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
                        {
                            double x3 = mmB3->getX(); double y3= mmB3->getY(); point p3 = mmB3->get_loc();
                            double dist = bg::distance(sought, p3);
                            interf+= mmB->calculate_Interf_of_link(x3, y3, dist);
                        }
                }
                double d21 = bg::distance(sought, p2);
                double snr = mmB->calculate_SNR_of_link(x2,y2, d21);
                double sinr = mmB->calculate_SINR_of_link(x2,y2, d21, interf);
                // Rules
                bool b_snr = snr>max_snr;
                bool b_parent = mmB2.get()->get_parent()!=mmB.get()->getID();
                
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
            mmB->set_parent(parent);
            mmB->set_SNR(max_snr);
            mmB->set_SINR(max_sinr);
            Add_load_BS(parent, mmB);
        }
    }
}

/**
 * Select path based on position-aware policy
 */
void IABN::path_selection_PA()
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    //TODO check this method precisely.
    int total = m_items.size();
    boost::progress_display show_progress(total);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
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
                    {
                        double x3 = mmB3->getX(); double y3= mmB3->getY(); point p3 = mmB3->get_loc();
                        double dist = bg::distance(sought, p3);
                        interf+= mmB->calculate_Interf_of_link(x3, y3, dist);
                    }
                }
                double d21 = bg::distance(sought, p2);
                double snr = mmB->calculate_SNR_of_link(x2,y2, d21);
                double sinr = mmB->calculate_SINR_of_link(x2,y2, d21, interf);
                // Rules
                bool b_snr = snr>max_snr;
                bool b_parent = mmB2.get()->get_parent()!=mmB.get()->getID();
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
            mmB->set_parent(parent);
            mmB->set_SNR(max_snr);
            mmB->set_SINR(max_sinr);
            Add_load_BS(parent, mmB);
        }
        ++show_progress;
    }
}


point IABN::find_closest_wired(uint32_t id, point loc)
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
void IABN::path_selection_MLR()
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    //TODO maybe here!
//     bool b_all_nodes_route = false;
//     int count = 0;
//     while(!b_all_nodes_route && count<10)
//     {
        for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
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
//                     double snr = mmB.get()->calculate_SNR_of_link(mmB2.get()->getX(), mmB2.get()->getY());
                    double load = 1. + mmB2.get()->get_load_BS_count();
                    double BW =def_BW;
                    double d = def_BW/load;
                    double rate;// =d * bm::log1p(1.+snr)/bm::log1p(2.0);
    //                 std::cout << "rate= " << rate << ", ";
                    bool b_rate = rate > max_rate;
                    bool b_parent = mmB2.get()->get_parent()!=cid;
                    
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
                mmB->set_parent(parent_id);
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

bool IABN::check_all_routes()
{
    bool found = true;
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB->is_route_found())
        {found = false; break;}
    }
    return found;
}

void IABN::Add_load_BS(uint32_t parent, bs_ptr bs/*uint32_t member, point loc*/)
{
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->getID()==parent)
        {
            if(mmB->getID()!=bs->get_parent())
                std::cerr << "WTF!\n";
            mmB->add_to_load_BS(bs);
        }
    }
}


int IABN::get_IAB_count()
{
    int cc = 0;
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::wireless)
            cc++;
    }
    return cc;
}

int IABN::get_wired_count()
{
    int cc = 0;
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::wired)
            cc++;
    }
    return cc;
};

void IABN::reset(bool fixed)
{
//     std::lock_guard<std::mutex> guard(m_mutex);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->get_backhaul_Type()==Backhaul::wireless && fixed)
            mmB->reset();
        else
            mmB->reset();
    }
}

std::vector<int> IABN::count_hops(int &max_hop, int &failed)
{
    
    m_failed = 0; m_max_hop = -1;
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(m_max_hop < mmB->get_hop_count())
            m_max_hop = mmB->get_hop_count();
    }
//     memset(m_arr_hops, 0, sizeof(m_arr_hops));
    std::vector<int> arr_hops; arr_hops.resize(m_max_hop+1);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
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


// void IABN::reset_pointers()
// {
//     for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
//     {
//         bs_ptr mmB = (*it);
//         if(mmB)
//             mmB.reset();
//     }
// }

double IABN::find_SNR_bottleneck()
{
    double bottleneck = 1e10;
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->is_route_found() && mmB->get_backhaul_Type()==Backhaul::wireless)
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


double IABN::find_SINR_bottleneck()
{
    double bottleneck = 1e10;
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end(); ++it)
    {
        bs_ptr mmB = (*it);
        if(mmB->is_route_found() && mmB->get_backhaul_Type()==Backhaul::wireless)
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

void IABN::check_SINR_tree(matrix<double> &m)
{
    int Total_iter = m_items.size();
    boost::progress_display show_progress(Total_iter);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB) std::cerr << __FUNCTION__ << std::endl;
        
//         if(mmB.get()->get_backhaul_Type()==Backhaul::wired)
//             continue;
        
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
                    double d21 = bg::distance(sought, p2);
                     polygon2D poly = directional_polygon(p1, p2, mmB->get_phi_m());
                     std::vector<value> vec_query;
                     m_tree.query(bgi::intersects(poly), std::back_inserter(vec_query));
                     double interf=0.;
                     BOOST_FOREACH(value const&mz, vec_query)
                     {
                         bs_ptr mmB3 = boost::dynamic_pointer_cast<mmWaveBS>(mz.second);
                         uint32_t cid3 = mmB3->getID(); point p3 = mmB3->get_loc();
                         double d31 = bg::distance(sought, p3);
//                         std::cout << "cid3 = " << cid3 << std::endl;
                         if(cid3!=cid2 && cid3!=cid)
                             interf+= mmB->calculate_Interf_of_link(mmB3->getX(), mmB3->getY(), d31);
                     }
                    
//                    double snr = mmB->calculate_SNR_of_link(x2,y2, d21);
                     double sinr = mmB->calculate_SINR_of_link(x2,y2, d21, interf);
//                      if(cid>m.size1()-1 || cid2 > m.size2()-1)
//                          std::cout << "Here!!" << std::endl;
                     // Id starts from 1, so the index of the matrix is id-1
                     m(cid-1, cid2-1) = sinr;
                }
            }
        }
        ++show_progress;
    }
}

void IABN::check_SNR_array()
{
    int Total_iter = m_items.size();
    boost::progress_display show_progress(Total_iter);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB) std::cerr << __FUNCTION__ << std::endl;
        
        uint32_t cid = mmB.get()->getID();
        point sought = mmB->get_loc();
        for(std::vector<bs_ptr>::iterator it2=m_items.begin(); it2!=m_items.end();++it2)
        {
            bs_ptr mmB2 = (*it2);
            if(!mmB2) std::cerr << __FUNCTION__ << std::endl;
            uint32_t cid2 = mmB2.get()->getID();
            if( cid2!= cid)
            {
                double x2 = mmB2->getX(); double y2 = mmB2->getY(); point p2 = mmB2->get_loc();
                double dist = bg::distance(sought, mmB2->get_loc());
                if ( dist < def_MAX_MMWAVE_RANGE)
                {
                    double snr = mmB->calculate_SNR_of_link(x2,y2, dist);
                }
            }
        }
        ++show_progress;
    }
}

void IABN::check_SINR_array()
{
    int Total_iter = m_items.size();
    boost::progress_display show_progress(Total_iter);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB) std::cerr << __FUNCTION__ << std::endl;
        
        uint32_t cid = mmB.get()->getID();
        point sought = mmB->get_loc();
        double x1 = mmB->getX(); double y1 = mmB->getY();
        for(std::vector<bs_ptr>::iterator it2=m_items.begin(); it2!=m_items.end();++it2)
        {
            bs_ptr mmB2 = (*it2);
            if(!mmB2) std::cerr << __FUNCTION__ << std::endl;
            uint32_t cid2 = mmB2.get()->getID();
            if( cid2!= cid)
            {
                double x2 = mmB2->getX(); double y2 = mmB2->getY(); point p2 = mmB2->get_loc();
//                 double d21 = bg::distance(sought, p2);
                double d21 = euclidean_dist(x1,y1,x2,y2);
                if (d21 < def_MAX_MMWAVE_RANGE)
                {
                    double interf = 0.;
                    for(std::vector<bs_ptr>::iterator it3=m_items.begin(); it3!=m_items.end();++it3)
                    {
                        bs_ptr mmB3 = (*it3);
                        if(!mmB3) std::cerr << __FUNCTION__ << std::endl;
                        uint32_t cid3 = mmB3.get()->getID(); 
                        double x3 = mmB3->getX(); double y3 = mmB3->getY(); point p3 = mmB3->get_loc();
                        if(cid3!=cid2 && cid3!=cid)
                        {
//                             double d31 = bg::distance(sought, p3);
                            double d31 = euclidean_dist(x1,y1,x3,y3);
                            if (d31 < def_MAX_MMWAVE_RANGE)
                            {
                                point p11 = point(x2-x1, y2-y1);
                                point p22 = point(x3-x1, y3-y1);
                                double dot = bg::dot_product<point, point>(p11, p22);
                                double teta = std::acos(dot/(d21*d31));
                                if(teta <= mmB->get_phi_m()/2.)
                                {
                                    interf+= mmB->calculate_Interf_of_link(x3, y3, d31);
                                }
                            }
                        }
                    }
                    double sinr = mmB->calculate_SINR_of_link(x2,y2, d21, interf);
                }
            }
        }
        ++show_progress;
    }
}

void IABN::check_SINR_array2()
{
    int Total_iter = m_items.size();
    boost::progress_display show_progress(Total_iter);
    for(std::vector<bs_ptr>::iterator it=m_items.begin(); it!=m_items.end();++it)
    {
        bs_ptr mmB = (*it);
        if(!mmB) std::cerr << __FUNCTION__ << std::endl;
        
        uint32_t cid = mmB.get()->getID();
        point sought = mmB->get_loc();
        double x1 = mmB->getX(); double y1 = mmB->getY();
        std::map<bs_ptr, double>neighbors;
//         std::vector<bs_ptr> neighbors;
        
        for(std::vector<bs_ptr>::iterator it2=m_items.begin(); it2!=m_items.end();++it2)
        {
            bs_ptr mmB2 = (*it2);
            if(!mmB2) std::cerr << __FUNCTION__ << std::endl;
            uint32_t cid2 = mmB2.get()->getID();
            if( cid2!= cid)
            {
                point p2 = mmB2->get_loc();
                double d21 = bg::distance(sought, p2);        
                if (d21 < def_MAX_MMWAVE_RANGE)
                {
                    neighbors.insert( std::pair<bs_ptr, double>(mmB2, d21));
                }
            }
        }
        
        for(std::map<bs_ptr, double>::iterator it2=neighbors.begin(); it2!=neighbors.end();++it2)
        {
            bs_ptr mmB2 = it2->first;
            double d21 = it2->second;
            uint32_t cid2 = mmB2.get()->getID(); 
            double x2 = mmB2->getX(); double y2 = mmB2->getY(); point p2 = mmB2->get_loc();
            double interf = 0.;
            for(std::map<bs_ptr, double>::iterator it3=neighbors.begin(); it3!=neighbors.end();++it3)
            {
                bs_ptr mmB3 = it3->first;
                double d31 = it3->second;
                uint32_t cid3= mmB3.get()->getID(); 
                double x3 = mmB3->getX(); double y3 = mmB3->getY(); point p3 = mmB3->get_loc();
//                 double d31 = bg::distance(sought, p3);
                if(cid3!=cid2)
                {
//                     double d21 = bg::distance(sought, p2);
//                     double d31 = bg::distance(sought, p3);
                    point p11 = point(x2-x1, y2-y1);
                    point p22 = point(x3-x1, y3-y1);
                    double dot = bg::dot_product<point, point>(p11, p22);
                    double teta = std::acos(dot/(d21*d31));
                    if(teta <= mmB->get_phi_m()/2.)
                    {
                        interf+= mmB->calculate_Interf_of_link(x3, y3, d31);
                    }
                }
            }
//             double sinr = mmB->calculate_SINR_of_link(x2,y2, interf);
        }
        ++show_progress;
    }
}


matrix<double> IABN::Generate_Matrix_SINR()
{
    int num = get_IAB_count() + get_wired_count();
    // Matrix containing SINR of all links
    // Matrix is selected from boost::numeric::ublas::matrix
    matrix<double> matrix_sinr (num, num);
    for (unsigned i = 0; i < matrix_sinr.size1 (); ++ i)
        for (unsigned j = 0; j < matrix_sinr.size2 (); ++ j)
                matrix_sinr (i, j) = 0.0;
    check_SINR_tree(matrix_sinr);
    return matrix_sinr;
}
