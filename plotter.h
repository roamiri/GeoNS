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
#include "bits/stdc++.h"
#include <map>

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
    
    void closePlot(){plt::close();}
    
    void plot1DArray(std::vector<double> arr, std::string name, std::string legend, std::string xlabel, std::string ylabel, bool grid=true)
    {
        // Set the size of output image to 1200x780 pixels
        plt::figure_size(1200, 780);
        // Plot line from given x and y data. Color is selected automatically.
        // Plot a line whose name will show up as "log(x)" in the legend.
        plt::named_plot(legend,arr, "r--");
        // Set x-axis to interval [0,1000000]
        int xmax = arr.size();
        plt::xlim(0, xmax);
        plt::ylim(0, 1);
        // Add graph title
    //         plt::title("CDF");
        // Enable legend.
        plt::legend();
        if(grid)
            plt::grid(true);
        plt::xlabel(xlabel);
        plt::ylabel(ylabel);
        // Save the image (file format is determined by the extension)
        plt::save(name);
        plt::close();
    }
    
    void plot_CDF_Array(std::vector<std::vector<double>> DD, std::string name, std::vector<std::string> legends, std::string xlabel, std::string ylabel, std::vector<std::string> colors, bool grid=true)
    {

        plt::figure_size(1200, 780);
        // sort the data:
        int num_plots = DD.size();
        for(int i=0;i<num_plots;++i)
        {
            std::vector<double> data = DD[i];
            std::string legend = legends[i];
            std::string color = colors[i];
            sort(data.begin(), data.end());
            int n = data.size();

            // calculate the proportional values of samples
            std::vector<double>p;
            for(int i=0;i<n;++i)
                p.push_back(1.*i/(n-1));
            plt::named_plot(legend,data, p, color);
        }
//         plt::xlim(10., 30.);
        plt::ylim(0., 1.2);
        plt::legend();
        if(grid)
            plt::grid(true);
        std::map<std::string, std::string> attr;
        attr.insert(std::make_pair("size", "22"));
        plt::xlabel(xlabel, attr);
        plt::ylabel(ylabel, attr);
        plt::save(name);
        plt::close();
    }

};

#endif // PLOTTER_H
