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
#include "mmwavebs.h"

class Painter
{
public:
    Painter(std::vector<std::shared_ptr<mmWaveBS>>const &v);
    ~Painter();
	void Start();
	
	void Enable();
	void add_to_draw_queue(std::shared_ptr<draw_object> dd);
	
private:
	
	std::vector<std::shared_ptr<draw_object>> m_objects;
	std::vector<std::shared_ptr<mmWaveBS>>const &m_nodes;
	
	bool m_draw = false;
	svg::Document* m_doc;
	svg::Dimensions* m_dimesnions;
	
	bool m_stopThread = false;
	std::thread m_draw_thread;
	
	void ThreadMain();
	void update();

};

#endif // PAINTER_H
