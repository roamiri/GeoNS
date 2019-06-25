#ifndef COMMON_H
#define COMMON_H

// #include "stdafx.h"
#include <iostream>
#include "time.h"
#include <math.h>
#include <random>

static double in_bound = 100.0;
static double out_bound = 200.0;

enum Status{idle, candiate, inBound, outBound, clusterHead};

// Calculate Eclidean Distance to the power 2 , //NOTE no sqrt cause it is expensive!!
static double euclidean_dist2(double x1, double y1, double x2, double y2)
{
	return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

static void counter(int seconds)
{
	clock_t endTurn = clock() + seconds * CLOCKS_PER_SEC;
	while(clock() < endTurn){}
}

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
