#ifndef COMMON_H
#define COMMON_H

// #include "stdafx.h"
#include <iostream>
#include "time.h"
#include <math.h>
#include <random>
#include <fstream>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>


static double in_bound = 100.0;
static double out_bound = 200.0;

#define def_Nothing 4e3  // for initializing uint32_t 

#define lambda_MBS 5e-6
#define lambda_SBS 1e-4

#define def_prob_Wired 0.1
#define def_BW 400000000 // Bandwidth = 400 MHz  // Reference: Distributed Path Selection by Polese
#define def_fc  28.e9 // fc = 28 GHz
#define def_P_tx  30 // dBm
#define def_access_alpha_los 2.0 //  References is "Tractable Model for Rate in Self-Backhauled Millimeter Wave Cellular Networks" by S. Singh, et. al. IEEE JSAC. 2015
#define def_backhaul_alpha_los 2.0
#define def_access_alpha_nlos 3.3
#define def_backhaul_alpha_nlos 3.5
#define def_beta 70 // dBm, path loss at 1 m
#define def_access_zeta_los 5.2 //  
#define def_backhaul_zeta_los 4.2
#define def_access_zeta_nlos 7.6
#define def_backhaul_zeta_nlos 7.9
#define def_G_max 18 // dB main lobe gain
#define def_G_min -2 // dB side lobe gain
#define def_teta_b 10 // degree beam-width
#define def_NoiseFigure 10 // dB
#define def_MAX_MMWAVE_RANGE 200 // maximum mmWave range = 200 meters

#define RED 0xff // red color
#define BLUE 0xff // BLUE color

#define def_Area 1.e6 // 1 Km2 as te area
#define def_min_dis 10 // 10 meter minimum distance between the nodes

enum Status{idle, candiate, inBound, outBound, clusterHead};
enum Backhaul{wired, IAB};
enum Path_Policy{HQF, WF, PA, MLR}; //HQF = High Quality First, WF = Wired First, PA = Position Aware, MLR = Maximum local rate
    
static uint32_t m_nextId;
static uint32_t get_nextID() {m_nextId++; return m_nextId;}

static double dBm_to_watt(double pp)
{
    double pdB = pp - 30;
    return pow(10., pdB/10.);
}
// Calculate Eclidean Distance to the power 2 , //NOTE no sqrt cause it is expensive!!
static double euclidean_dist2(double x1, double y1, double x2, double y2)
{
	return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

static double euclidean_dist(double x1, double y1, double x2, double y2)
{
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

static void counter(int seconds)
{
	clock_t endTurn = clock() + seconds * CLOCKS_PER_SEC;
	while(clock() < endTurn){}
}

struct update_parent_msg
{
    uint32_t id;
    int hop_cnt;
    float xx; float yy;
    update_parent_msg(uint32_t iidd, int hcnt, float x, float y){id=iidd; hop_cnt=hcnt; xx=x; yy=y;}
};

struct candidacy_msg
{
	double x;
	double y;
	uint32_t id;
	candidacy_msg(double xx,double yy,uint32_t ii){x = xx; y = yy; id = ii;}
};

struct cluster_head_msg
{
	double x;
	double y;
	uint32_t id;
	std::size_t color;
	cluster_head_msg(double xx,double yy,uint32_t ii, std::size_t c)
	{x = xx; y = yy; id = ii; color=c;}
};

class draw_object
{
public:
	draw_object(double xx, double yy, std::size_t color[3], bool head)
	{x = xx; y= yy; red = color[0]; green = color[1]; blue= color[2]; cluster_head = head;}
	double x;
	double y;
	std::size_t red;
	std::size_t green;
	std::size_t blue;
	bool cluster_head;
};

static std::random_device rdd;
static std::mt19937 gen(rdd());

static int poisson(int lambda)
{
    std::poisson_distribution<int> pd(lambda);
    return pd(gen);
}

// static int poisson ( double lambda )
// {
//   double u = drand48();
//   int k = 0;
//   double sum = exp(-lambda);
// 
//   double sidste = sum;
// 
//   while ( true )
//     {
//       if ( u < sum ) { return k; }
//       else 
// 	{
// 	  k++;
// 	  sidste *= lambda/k;
// 	  sum += sidste;
// 	}
//     }
// }

static std::size_t generateColor()
{
	std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
	std::mt19937_64 eng(rd()); 
	std::uniform_int_distribution<unsigned long long> distr;
	uint32_t clid = (uint32_t) distr(eng);
	
	std::size_t color = (std::size_t) (clid);
	return color;
}

static void save1DArrayasText(boost::numeric::ublas::vector<double> array, int size, std::string name)
{
    std::ofstream test;
    test.open(name);
    test << array;
    test << "\n";
    test.close();
//     for(int i=0;i<size;i++)
//     {
//         test << i;//std::to_string(i);
//         test << " ";
//         test << array;//std::to_string(array[i]);
//         test << "\n";
//     }
}

static std::vector<double> boostVtoStdV(boost::numeric::ublas::vector<double> v)
{
    size_t size = v.size();
    std::vector<double> out(size, 0.);
    for(int i=0; i<size; ++i)
        out[i]=v[i];
    
    return out;
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

static void printProgress (double percentage)
{
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}

#endif // COMMON_H
