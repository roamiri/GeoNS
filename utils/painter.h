/*
 * Copyright (c) 2018, <copyright holder> <email>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <copyright holder> <email> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> <email> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef PAINTER_H
#define PAINTER_H

#include <iostream>
#include <thread>
#include "simple_svg.h"
#include "common.h"
#include <memory>
#include <mutex>
#include <sys/prctl.h>

// using namespace svg;

class Painter
{
public:
    Painter(std::string name)
    {
        m_dimesnions = new svg::Dimensions(100, 100);
        m_doc = new svg::Document(name, svg::Layout(*m_dimesnions, svg::Layout::BottomLeft));
    }
    ~Painter(){m_stopThread = true;}
	void Start()
    {
        char thread_name_buff [20];
        sprintf(thread_name_buff, "Painter");
        prctl(PR_SET_NAME,thread_name_buff,0,0,0);
    }
	
	void Enable(){m_draw = true;}
	void add_to_draw_queue(std::shared_ptr<draw_object> dd){m_objects.push_back(dd);}
	
    template<class T>
	void update(std::vector<boost::shared_ptr<T>>const &v)
    {
        int size = v.size();
        double x_shift = 0.;
        double y_shift = 0.;
        std::lock_guard<std::mutex> guard(m_mutex);
        for(int i=0;i<size;i++)
        {
            boost::shared_ptr<T> dd = v[i];
            double x = (0.2) * (dd.get()->getX() + x_shift);
            double y = (0.2) * (dd.get()->getY() + y_shift);
        
            std::size_t color = dd.get()->getColor();
            std::size_t red = (color & 0xff0000) >> 16; 
            std::size_t green =(color & 0x00ff00) >> 8; 
            std::size_t blue = (color & 0x0000ff);  	
    // 		std::cout << "Status = " << dd.get()->getStatus() << std::endl;
            // Text of number of hops to the wired node
            *m_doc << svg::Text(svg::Point(x, y+2), std::to_string(dd->get_hop_count()), svg::Fill(svg::Color(0,0,0)), svg::Font(2.,"Verdana"));
            // Drawing the nodes
            if(dd.get()->get_backhaul_Type()==Backhaul::wired)
                *m_doc << svg::Circle(svg::Point(x, y), 2, svg::Fill(svg::Color(0,0,0)), svg::Stroke(1, svg::Color(RED,0,0)));
            else
                *m_doc << svg::Circle(svg::Point(x, y), 2, svg::Fill(svg::Color(red,green,blue)), svg::Stroke(1, svg::Color(red, green, blue)));
            
        }
        
    // 	std::lock_guard<std::mutex> guard1(m_mutex);
        for(int i=0;i<size;i++)
        {
            // Drawing paths
            boost::shared_ptr<T> mmB = v[i];
            double x1 = (0.1) * (mmB->getX()+x_shift);
            double y1 = (0.1) * (mmB->getY()+y_shift);
            uint32_t parent = mmB->get_parent();
            //TODO correct this search!!!
            if(parent!=def_Nothing)
            {
                double x2 = (0.1) * (v[parent-1]->getX()+x_shift);
                double y2 = (0.1) * (v[parent-1]->getY()+y_shift);
                *m_doc << svg::Line(svg::Point(x1,y1), svg::Point(x2,y2), svg::Stroke(0.5, svg::Color(0,0,BLUE)));
            }
        }
        m_doc->save();
    }
    
    template<class T>
    void draw_node_ID(std::vector<T>const &v)
    {
        int size = v.size();
        double x_shift = 0.;
        double y_shift = 0.;
        std::lock_guard<std::mutex> guard(m_mutex);
        for(int i=0;i<size;i++)
        {
            T dd = v[i];
            double x = (0.1) * (dd.get()->getX() + x_shift);
            double y = (0.1) * (dd.get()->getY() + y_shift);
        
            std::size_t color = dd.get()->getColor();
            std::size_t red = (color & 0xff0000) >> 16; 
            std::size_t green =(color & 0x00ff00) >> 8; 
            std::size_t blue = (color & 0x0000ff);
    // 		std::cout << "Status = " << dd.get()->getStatus() << std::endl;
            // Text of number of hops to the wired node
            *m_doc << svg::Text(svg::Point(x, y+2), std::to_string(dd->getID()), svg::Fill(svg::Color(0,0,0)), svg::Font(2.,"Verdana"));
            // Drawing the nodes
            if(dd.get()->get_backhaul_Type()==Backhaul::wired)
                *m_doc << svg::Circle(svg::Point(x, y), 2, svg::Fill(svg::Color(0,0,0)), svg::Stroke(1, svg::Color(RED,0,0)));
            else
                *m_doc << svg::Circle(svg::Point(x, y), 2, svg::Fill(svg::Color(red,green,blue)), svg::Stroke(1, svg::Color(red, green, blue)));
            
        }
        m_doc->save();
    }
    
    
template<class T>
void draw_neighbors(std::vector<boost::shared_ptr<T>>const &v)
{
    int size = v.size();
    double x_shift = 0.;
    double y_shift = 0.;
    std::lock_guard<std::mutex> guard(m_mutex);
    for(int i=0;i<size;i++)
    {
        boost::shared_ptr<T> dd = v[i];
        double x = (0.1) * (dd.get()->getX() + x_shift);
        double y = (0.1) * (dd.get()->getY() + y_shift);
    
        std::size_t color = dd.get()->getColor();
        std::size_t red = (color & 0xff0000) >> 16; 
        std::size_t green =(color & 0x00ff00) >> 8; 
        std::size_t blue = (color & 0x0000ff);
        // Drawing the nodes
        *m_doc << svg::Circle(svg::Point(x, y), 2, svg::Fill(svg::Color(red,green,blue)), svg::Stroke(1, svg::Color(red, green, blue)));
        *m_doc << svg::Text(svg::Point(x, y+2), std::to_string(dd->get_num_neighbors()), svg::Fill(svg::Color(0,0,0)), svg::Font(2.,"Verdana"));
        std::vector<uint32_t> nn = dd->get_neighborsID();
        for(int j=0;j<nn.size();++j)
        {
            // Drawing paths
            boost::shared_ptr<T> mmB = v[nn[j]-1];
            double xn = (0.1) * (mmB->getX()+x_shift);
            double yn = (0.1) * (mmB->getY()+y_shift);
            *m_doc << svg::Line(svg::Point(x,y), svg::Point(xn,yn), svg::Stroke(0.2, svg::Color(0,0,BLUE)));
        }
    }
    m_doc->save();
}

template<typename T>
void draw_clusters(std::vector<boost::shared_ptr<T>>const &v)
{
    int size = v.size();
	for(int i=0;i<size;i++)
	{
		boost::shared_ptr<T> dd = v[i];
		double x = (0.1) * dd.get()->getX();
		double y = (0.1) * dd.get()->getY();
	
		std::size_t color = dd.get()->getColor();
		std::size_t red = (color & 0xff0000) >> 16; 
		std::size_t green =(color & 0x00ff00) >> 8; 
		std::size_t blue = (color & 0x0000ff);  	
// 		std::cout << "Status = " << dd.get()->getStatus() << std::endl;
		if(dd.get()->getStatus()==Status::clusterHead)
			*m_doc << svg::Circle(svg::Point(x, y), 2, svg::Fill(svg::Color(0,0,0)), svg::Stroke(1, svg::Color(red,green,blue)));
		else
			*m_doc << svg::Circle(svg::Point(x, y), 2, svg::Fill(svg::Color(red,green,blue)), svg::Stroke(1, svg::Color(red, green, blue)));
		
		m_doc->save();
	}
}


private:
	
	std::vector<std::shared_ptr<draw_object>> m_objects;
// 	std::vector<bs_ptr>const &m_nodes;

	
	bool m_draw = false;
	svg::Document* m_doc;
	svg::Dimensions* m_dimesnions;
	
	bool m_stopThread = false;
// 	std::thread m_draw_thread;

	void ThreadMain()
    {
        while(!m_stopThread)
        {
            std::this_thread::sleep_for( std::chrono::seconds(1) );
            if(m_draw)
            {
    // 			update();
                m_draw = false;
            }
        }
    }
    
    std::mutex m_mutex;

};

#endif // PAINTER_H
