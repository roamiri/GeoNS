/*
 * Copyright 2019 <copyright holder> <email>
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
 */

#ifndef PLOTTER_H
#define PLOTTER_H

#include "matplotlibcpp.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace plt = matplotlibcpp;

/**
 * @todo write docs
 */
class plotter
{
public:
    /**
     * Default constructor
     */
    plotter(){}

    /**
     * Destructor
     */
    ~plotter(){}
    
    
    void plot1DArray(std::vector<double> arr, std::string name)
    {
        plt::plot(arr);
        plt::save(name);
    }
    

};

#endif // PLOTTER_H
