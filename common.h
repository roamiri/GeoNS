#ifndef COMMON_H
#define COMMON_H

// #include "stdafx.h"
#include <iostream>
#include "time.h"
#include <math.h>
#include <random>

static double in_bound = 100.0;
static double out_bound = 200.0;
#define def_BW 4.e8 // Bandwidth = 400 MHz
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

#define RED 0xff // red color


enum Status{idle, candiate, inBound, outBound, clusterHead};
enum BS_type{wired, IAB};

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
    update_parent_msg(uint32_t iidd, int hcnt){id=iidd; hop_cnt=hcnt;}
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

static int poisson ( double lambda )
{
  double u = drand48();
  int k = 0;
  double sum = exp(-lambda);

  double sidste = sum;

  while ( true )
    {
      if ( u < sum ) { return k; }
      else 
	{
	  k++;
	  sidste *= lambda/k;
	  sum += sidste;
	}
    }
}

static std::size_t generateColor()
{
	std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
	std::mt19937_64 eng(rd()); 
	std::uniform_int_distribution<unsigned long long> distr;
	uint32_t clid = (uint32_t) distr(eng);
	
	std::size_t color = (std::size_t) (clid);
	return color;
}   
#endif // COMMON_H
