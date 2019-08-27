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

// #include "painter.h"

// using namespace svg;

// Painter::Painter(std::string name)
//:m_draw_thread(),
//m_nodes(v)
// {
// 	m_nodes = &nodes;
	
//     Start(v);
// }

Painter::~Painter()
{
// 	m_stopThread = true;
// 	if(m_draw_thread.joinable())
// 		m_draw_thread.join();
// 	std::cout << "Deconstruct " << __FILE__ << std::endl;
}

void Painter::Start(/*std::vector<bs_ptr>const &v*/)
{
// 	m_draw_thread = std::thread(&Painter::ThreadMain, this);
// 	char thread_name_buff [20];
//     sprintf(thread_name_buff, "Painter");
//     prctl(PR_SET_NAME,thread_name_buff,0,0,0);
}

void Painter::ThreadMain(/*std::vector<bs_ptr>const &v*/)
{
	
}

void Painter::Enable()
{
//     m_nodes = v;
// 	m_draw = true;
// 	std::cout << "ready to draw!" << std::endl;
}

void Painter::add_to_draw_queue(std::shared_ptr<draw_object> dd)
{
// 	std::cout << "new draw object" << dd->x << ", " << dd->y << std::endl;
// 	m_objects.push_back(dd);
// 	std::cout << "new draw object" << m_objects.back()->x << ", " << m_objects.back()->y << std::endl;
}

// template<class T>
// void Painter::update(std::vector<T>const &v)
// {
// 	
// }


// template<class T>
// void Painter::draw_node_ID(std::vector<T>const &v)
// {
//     
// }
