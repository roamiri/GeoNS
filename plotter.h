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
    
    
    void plot1DArray(std::vector<double> arr, std::string name, std::string legend, bool grid=true)
    {
        plt::plot(arr, "r--");
            // Set the size of output image to 1200x780 pixels
        plt::figure_size(1200, 780);
        // Plot line from given x and y data. Color is selected automatically.
        // Plot a line whose name will show up as "log(x)" in the legend.
        plt::named_plot(legend,arr);
        // Set x-axis to interval [0,1000000]
        plt::xlim(0, 10);
        // Add graph title
//         plt::title("CDF");
        // Enable legend.
        plt::legend();
        if(grid)
            plt::grid(true);
        // Save the image (file format is determined by the extension)
        plt::save(name);
        
    }
    

};

#endif // PLOTTER_H
