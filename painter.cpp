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

#include "painter.h"
#include <sys/prctl.h>

using namespace svg;

Painter::Painter(/*std::vector<std::shared_ptr<mmWaveBS>>const &v*/)
//:m_draw_thread(),
//m_nodes(v)
{
// 	m_nodes = &nodes;
	m_dimesnions = new svg::Dimensions(100, 100);
	m_doc = new svg::Document("network.svg", svg::Layout(*m_dimesnions, svg::Layout::BottomLeft));
//     Start(v);
}

Painter::~Painter()
{
	m_stopThread = true;
// 	if(m_draw_thread.joinable())
// 		m_draw_thread.join();
	std::cout << "Deconstruct " << __FILE__ << std::endl;
}

void Painter::Start(/*std::vector<std::shared_ptr<mmWaveBS>>const &v*/)
{
// 	m_draw_thread = std::thread(&Painter::ThreadMain, this);
	char thread_name_buff [20];
    sprintf(thread_name_buff, "Painter");
    prctl(PR_SET_NAME,thread_name_buff,0,0,0);
}

void Painter::ThreadMain(std::vector<std::shared_ptr<mmWaveBS>>const &v)
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

void Painter::Enable()
{
//     m_nodes = v;
	m_draw = true;
// 	std::cout << "ready to draw!" << std::endl;
}

void Painter::add_to_draw_queue(std::shared_ptr<draw_object> dd)
{
// 	std::cout << "new draw object" << dd->x << ", " << dd->y << std::endl;
	m_objects.push_back(dd);
// 	std::cout << "new draw object" << m_objects.back()->x << ", " << m_objects.back()->y << std::endl;
}

void Painter::update(std::vector<std::shared_ptr<mmWaveBS>>const &v)
{
	int size = v.size();
    double x_shift = 0.;
    double y_shift = 0.;
    std::lock_guard<std::mutex> guard(m_mutex);
	for(int i=0;i<size;i++)
	{
		std::shared_ptr<mmWaveBS> dd = v[i];
		double x = (0.1) * (dd.get()->getX() + x_shift);
		double y = (0.1) * (dd.get()->getY() + y_shift);
	
		std::size_t color = dd.get()->getColor();
		std::size_t red = (color & 0xff0000) >> 16; 
		std::size_t green =(color & 0x00ff00) >> 8; 
		std::size_t blue = (color & 0x0000ff);  	
// 		std::cout << "Status = " << dd.get()->getStatus() << std::endl;
        // Text of number of hops to the wired node
        *m_doc << Text(Point(x, y+2), std::to_string(dd->get_hop_count()), Fill(Color(0,0,0)), Font(2.,"Verdana"));
        // Drawing the nodes
		if(dd.get()->get_backhaul_Type()==Backhaul::wired)
			*m_doc << Circle(Point(x, y), 2, Fill(Color(0,0,0)), Stroke(1, Color(RED,0,0)));
		else
			*m_doc << Circle(Point(x, y), 2, Fill(Color(red,green,blue)), Stroke(1, Color(red, green, blue)));
		
	}
	
// 	std::lock_guard<std::mutex> guard1(m_mutex);
	for(int i=0;i<size;i++)
    {
        // Drawing paths
        std::shared_ptr<mmWaveBS> mmB = v[i];
        double x1 = (0.1) * (mmB->getX()+x_shift);
        double y1 = (0.1) * (mmB->getY()+y_shift);
        uint32_t parent = mmB->get_IAB_parent();
        //TODO correct this search!!!
        if(parent!=def_Nothing)
        {
            double x2 = (0.1) * (v[parent-1]->getX()+x_shift);
            double y2 = (0.1) * (v[parent-1]->getY()+y_shift);
            *m_doc << Line(Point(x1,y1), Point(x2,y2), Stroke(0.5, Color(0,0,BLUE)));
        }
    }
    m_doc->save();
}


