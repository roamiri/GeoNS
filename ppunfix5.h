//  C++ Library: ppunfix5.h
//
//  LSN April 2000  
//      May   2001  cleaned
//      Oct   2001  new function estimate_means added
//
//  Modification of ppunfix4.h
//
//  NEW FEATURES: faster as space extended when needed
//                can initialise with extra integer which means n(X) constant
//
// ---------------------------------------------------------------------------
//  
//  En punktprocess er defineret i k=1,2,3 dimensioner paa [0,ai]^k (*ikke s^2)
//  med tilhoerende edge-correction window [lowi,uppi]^k  (*nyt)
//  Relationen er altid lebesgue afstand (*indskraenkning) og relationen
//  findes selvom type process ikke er defineret (*NYT)
//  Processen kan vaere Strauss eller Geyer
//
//  k: dimension af process {1,2,3} DEFAULT=3   
//     initialiseres med initDim(k) eller readData(fil,k) (1)
//  n: antal punkter DEFAULT=0 (DEFAULT is random number of points)
//     initialiseres med initNum(n), readData(fil,k) eller makeProcess() (1)
//  R: relations afstand DEFAULT=0
//     initialiseres med initRelafst(R) (1)
//  S: definitionsrum DEFAULT=[0,1]^k
//     initialiseres med initAkse(i,lgd) saa (i+1)'te akse er [0,lgd]
//  W: edge-correction vindue DEFAULT=S
//     initialiseres med initWindow(i,low,upp) saa (i+1)'te akse er [low,upp]
//     OBS: initWindow skal koeres EFTER initAkse (ellers overskrives W=S)
//
//  (1) k, n og R initialiseres ogsaa ved def af special-process Strauss/Geyer
//      Desuden initialiseres gamma (og c for Geyer)
//  
//
//  Tilknyttede funktioner:
//
//  void random_number ( void );     
//  void fixed_number ( void );         Optional: can be called with or without
//  void fixed_number ( int num );                specifying number of ponts
//
//  void initNum ( int num );
//  void initDim ( int dim );
//  void initRelafst ( ind afst );
//  void initAkse ( int i, double lgd );
//  void initWindow ( int i, double low, double upp );
//  
//  void readData ( string filename, int dim );
//  void saveData ( string filename );
//  
//  void transform ( int akseno, double & theta );
//  void invTransform ( int akseno, double & theta );
//
//  void copy ( pointprocess *x );
//  void resetcoords ( int coord, pointprocess *x );  
//       Can only be used to reset coordinates coords to value of x, 
//       when other coords equals
//  void croptowindow ( void );
//
//  bool inside ( point eta );
//  double whatis ( int i, int coord ); i er nummer punkt coord i {0,1,2}
//  int whatis_n ( void );            #punkter i S
//  int whatis_n_window ( void );     #punkter i W
//  int whatis_s ( void );            #naboer i S
//  int whatis_s_window (void);       #naboer i W plus par hvor x1 i W x2 i S\W
//  int whatis_k ( void );
//  double whatis_R ( void );  
//  double whatis_akse ( int akseno );
//  double whatis_L ( int akseno );
//  double whatis_U ( int akseno );
//  double whatis_sum ( int coord );         #sum af coord-coordinater
//  double whatis_sum_window ( int coord );  #sum af coord-coordinater i W
//
//  void triplets_canon ( int & nn, int & ss, int & ww ); 
//      # Beregner kanoniske statistic for triplets process: n(x), s(x), w(x)
//
//  int quadcount2D ( double x1, double x2, double y1, double y2 ); OBS kun 2D
//      problemet med observerede data er, at de ofte ligger paa skillelinjen
//      loesning: Gundersens metode. OBS: korrekt selv for yderste hoejre kant
//  
//  void countNeighboursTo( point eta, int & nn, int & nn_w );
//      Taeller antal naboer til eta i  nn: S og nn_w: W
//      OBS: inklusiv eta selv hvis eta er i punktprocesses hhv pp snit W
//  int countNeighbours( void );
//  int countNeighbours_window ( void );
//  
//  void binoprocess ( void );
//  void poissonprocess ( double intensity );
//  void emptyprocess ( void );
//  void makeMHBDstep ( void );     
//      Random number case: Metropolis-Hastings birth/death
//  void makeMHRstep ( void );
//      Fixed number case: Metropolis-Hastings replace
//  void makeprocess ( int );
//      Calls upper two according to case int=#iterations
//  void begin_chain_here ( int );
//      Chain start in current point pattern and int MH-steps are performed
//  
//  virtual double acceptProb( int j, int & nn, int & nn_w ) {}
//      Generates the Strauss or the Geyer point processes in a window of the
//      plane or the space with a random number of points.
//  
//      Parameters:  beta, gamma, R and c (Geyer)
//                   xakse, yakse and zakse (length of the axis)
//                   WARNING: if c is an integer program does not work!!!
//
//      Algorithm: Metropolis-Hastings birth and death. 
//                 Cf. e.g. J. Moeller (1999) sect 4.3.2 (special case p 150)
//
//      OBS: Det er denne process som opdaterer s og sw!!!
//      nn og nn_w er 'snyde'-variable som recorder 
//      nn = s(eta;x snit W) og nn_w = s(eta;x snit S\W)
//      Bemaerk at der skal traekkes 1 fra hvis eta selv er talt med som nabo
//
//
//  Kommentar til mig selv:
//
//  *En af de store forskelle paa dette og tidligere programmer
//  *er maaden hvorpaa s er inkorporeret: Den er angivet som global variabel
//  *GRUND: countNeighbours koerer langsomt, og den er koert en gang allerede
//  *i hvert Metropolis-step, saa den information lagres til senere brug
//  *Trick for ikke at beregne s ved readData og poissonprocess:
//  *   s og sw initialiseres til -1 fra start
//      hvis makeMHstep bliver udfoert beregnes de automatisk
//      ellers beregnes de foerst naar der spoerges: whatis_s(_window)
//      Bemaek, ved transformation saettes s og sw til -1 igen
//
//  *MODIFICATIONS: enable to make MCMC importance sampling and edge-correction




#ifndef __ppunfix5_h
#define __ppunfix5_h

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string>
#include <cassert>

#include "common.h" 

using namespace std;

class point
{

 private:

 public:

  double pt[3];

  double lebdist ( point& q, int k );      
  bool relatedTo ( int k, double R, point& q); 
  void replaceWith ( point& p );
  void unifpoint ( int k, double xakse, double yakse, double zakse );

  void transform ( int & akseno, double & a, double & theta );
  void invTransform ( int & akseno, double & a, double & theta );
  void invtransOtrans ( int & akseno, double & a, double & theta1, double & theta2 );
  
};



class pointprocess
{
 protected:

  int maxlgd, n, k, s, sw;  
  double R;
  bool randomnumber;
  double akse[3], windowL[3], windowU[3];

  void extend_dataset( void );
  void makeMHBDstep ( void );
  void makeMHRstep ( void );
  bool inside ( point eta );
  void countNeighboursTo( point eta, int & nn, int & nn_w );
  int countNeighbours ( void );
  int countNeighbours ( double afst );
  int countNeighbours_window ( void );

 public:

  point *data;

  pointprocess ( void );

  void random_number ( void );
  void fixed_number ( void );
  void fixed_number ( int num );

  void initNum ( int num );
  void initDim ( int dim );
  void initRelafst ( double afst );
  void initAkse ( int i, double lgd );
  void initWindow ( int i, double low, double upp );

  void readData ( string filename, int dim );
  void saveData ( string filename );

  void transform ( int akseno, double & theta );
  void invTransform ( int akseno, double & theta );
  void invtransOtrans ( int akseno, double & theta1, double & theta2 );

  void copy ( pointprocess *x );
  void resetcoords ( int coord, pointprocess *x );
  void croptowindow ( void );

  double whatis ( int i, int coord );
  int whatis_n ( void );
  int whatis_n_window ( void );
  int whatis_s ( void );
  int whatis_s ( double afst );
  int whatis_s_window (void);
  int whatis_k ( void );
  double whatis_R ( void );  
  double whatis_akse ( int akseno );
  double whatis_L ( int akseno );
  double whatis_U ( int akseno );
  double whatis_sum ( int coord );
  double whatis_sum_window ( int coord );
  double dist ( int coord1, int coord2 );

  void triplets_canon ( int & nn, int & ss, int & ww ); 

  int quadcount2D ( double x1, double x2, double y1, double y2 );

  void binoprocess ( void );
  void poissonprocess ( double intensity );
  void emptyprocess ( void );
  void begin_chain_here ( int );
  void makeprocess ( int );

  void estimate_means ( int nburn, int nsamp, int nspac, 
                        double & En, double & Es );

  //double Lnul ( double theta[k] );

  virtual double acceptProb( int j, int & nn, int & nn_w ) {} // random num
  virtual double acceptProb( point xi, int etaIndex ) {}      // fixed num

};



class straussprocess : public pointprocess
{
 protected:

  double beta, gamma;

 public:

  straussprocess( int dim, double b, double gam, double afs );
  straussprocess( int dim, double b, double gam, double afs, int num );

  double acceptProb ( int j, int & nn, int & nn_w );
  double acceptProb( point xi, int etaIndex );

  // STRAUSSPROCESSENS EGNE FUNKTIONER
  void resetGamma ( double gam );
  void resetBeta ( double b );

  double estiNON ( void );
  double estiPOW ( double obsNON, double Gpow );

};


class geyerprocess : public pointprocess
{
  double beta, gamma, c;

 public:

  geyerprocess
  ( int dim, double b, double gam, double afs, double upperlim );

  geyerprocess
  ( int dim, double b, double gam, double afs, double upperlim, int num );

  double acceptProb ( int j, int & nn, int & nn_w );
  double acceptProb( point xi, int etaIndex );

  // GEYERPROCESSENS EGNE FUNKTIONER
  double u( void );
  void reset_c ( double cval );
};






//---------------------------------------------------------------------------
// ******************************* POINT ************************************
//---------------------------------------------------------------------------

void point :: unifpoint ( int k, double xakse, double yakse, double zakse )
{
  if ( k==2 )
    {
      pt[0] = drand48()*xakse;
      pt[1] = drand48()*yakse;
      pt[2] = 0;
    }
  if ( k==3 )
    {
      pt[0] = drand48()*xakse;
      pt[1] = drand48()*yakse;
      pt[2] = drand48()*zakse;
    }
}


double point :: lebdist ( point& q, int k )
{
  int i;
  double res = 0;
  for ( i=0; i<k; i++ ) //k is real dimension
    {
      res += pow((pt[i] - q.pt[i]),2);
    }

  return sqrt(res);
}


bool point :: relatedTo( int k, double R, point& q )
{
  if ( lebdist( q, k ) <= R ) { return true; }
  else { return false; }
}


/*
bool point :: relatedTo( int k, double R, point& q )
{
  // Periodic relation (torus)

  // we need to make all translations of one of the two points
  // there are 3^k different translations
  // associate: 0 = subtract, 1 = void, 2 = add
  // Notice that (0,0,0) is usual relation

  // Example in two dimensions, where mid square is the window.
  // Need to consider all 9 points * whether one is related to @
  //
  //  ----------------
  //  |    |    |    |
  //  |    |    |    |
  //  |*   |*   |*   |
  //  ----------------
  //  |    |   @|    |
  //  |    |    |    |
  //  |*   |*   |*   |
  //  ----------------
  //  |    |    |    |
  //  |    |    |    |
  //  |*   |*   |*   |
  //  ----------------

  // (only need to add if point closer than R to left border)

  // OBS OBS OBS : Only made for unit square and two dimensions


  point copy_q;

  int i, j;
  
  copy_q.replaceWith(q);
  for ( i = 0; i < k; i++ ) 
    copy_q.pt[i] -= 2;

  for ( i = 0; i < 3; i++ )     // i is the coordinate
    {
      for ( j = 0; j < 3; j++ )
	{
	  copy_q.pt[0] = q.pt[0] + i - 1;
	  copy_q.pt[1] = q.pt[1] + j - 1;

	  // only unit square, else = q.pt[i] + j*ai - 1 
	  // where ai is the length of ith side of the window

	  if ( lebdist( copy_q, k ) <= R ) 
	    return true; 
	}
    }
  
  return false;
}
*/

void point :: replaceWith( point& p )
{
  int j;
  for ( j=0; j<3; j++ ) 
    pt[j] = p.pt[j];
}


void point :: transform ( int & akseno, double & a, double & theta )
{
  if ( ! (-0.001 < theta && theta < 0.001) )
    pt[akseno] = log((exp(theta*a)-1)*pt[akseno]/a + 1)/theta;
}


void point :: invTransform ( int & akseno, double & a, double & theta )
{
  if ( ! (-0.001 < theta && theta < 0.001) )
    pt[akseno] = a*(exp(theta*pt[akseno])-1)/(exp(theta*a)-1);
}


void point :: invtransOtrans ( int & akseno, double & a, double & theta1, double & theta2 )
{
  if ( ! ( -0.0001 < theta1-theta2 && theta1-theta2 < 0.0001 ) )
    pt[akseno] = 
      a*( pow( (exp(theta1*a)-1)*pt[akseno]/a + 1 , theta2/theta1) )/
      (exp(theta2*a)-1);
}



//---------------------------------------------------------------------------
// **************************** POINTPROCESS ********************************
//---------------------------------------------------------------------------


pointprocess :: pointprocess ( void ) 
{ 
  // Default settings

  maxlgd = 200;
  data = new point[maxlgd];
  n=0; k=3;
  s=-1; sw=-1;
  R=0;
  randomnumber = true;
  akse[0]=1; akse[1]=1; akse[2]=1; 
  windowL[0]=0; windowL[1]=0; windowL[2]=0; 
  windowU[0]=1; windowU[1]=1; windowU[2]=1; 
}      



void pointprocess :: extend_dataset( void )
{
  point *olddata;
  olddata = data;
  data = new point[2*maxlgd];

  int i;
  for ( i = 0; i < maxlgd; i++ ) 
    data[i].replaceWith(olddata[i]);
      
  delete [] olddata;
  maxlgd *= 2;
}



void pointprocess :: random_number ( void )
{
  randomnumber = true;
}


void pointprocess :: fixed_number ( void )
{
  randomnumber = false;
}

void pointprocess :: fixed_number ( int num )
{
  randomnumber = false;
  n = num;
}


void pointprocess :: initNum ( int num ) 
{ 
  n = num; 
  s = -1;
  sw = -1;
}


void pointprocess :: initDim ( int dim ) 
{ 
  if ( ( dim == 1 ) || ( dim==2 ) || ( dim==3 ) )
    k=dim;
  else { cout << "Dimensionen skal vaere 1, 2 eller 3" << endl; abort(); }

  s = -1;
  sw = -1;
}


void pointprocess :: initRelafst ( double afst )
{
  if ( afst > 0 ) 
    R = afst; 
  else { cout << "Relations-afstanden R skal vaere positiv" << endl; abort(); }

  s = -1;
  sw = -1;
}


void pointprocess :: initAkse ( int i, double lgd )
{
  if ( i < k && (( i==0 ) || ( i==1 ) ||  (i==2 )) )
    akse[i] = lgd;
  else { cout << "Akse-nummer er ugyldigt" << endl; abort(); }

  s = -1;
  sw = -1;

}


void pointprocess :: initWindow ( int i, double low, double upp )
{
  if ( i < k && (( i==0 ) || ( i==1 ) ||  (i==2 )) )
    {
      windowL[i] = low;
      windowU[i] = upp;
    }
  else { cout << "Akse-nummer er ugyldigt" << endl; abort(); }

  s = -1;
  sw = -1;

}



void pointprocess :: readData ( string filename, int dim )
{
 ifstream infile ( filename.c_str() );
 if (infile)
   {
     int i=0; int j;

     while ( ! infile.eof() )
       {
	 if ( i >= maxlgd ) 
	   extend_dataset();

         for ( j = 0; j < dim; j++ )
	   infile >> data[i].pt[j];

         i++;
       }
     k=dim;
     n=--i;
     s=-1;
     sw=-1;
   }
 else 
   { 
     cout << "Error: infile does not exist" << endl; 
     abort();
   }
}


void  pointprocess :: saveData ( string filename )
{ 
  ofstream OutputFile ( filename.c_str() );
  if ( OutputFile )
    {
      int i,j;
      for ( i=0; i<n; i++ )
        { 
          for ( j=0; j<k; j++ )
	    OutputFile << data[i].pt[j] << " ";
          OutputFile << endl;
        }
      OutputFile.close();
      cout << "The data has been stored on the file: " << filename << endl;
    }
  else { cout << "COULD NOT CREATE FILE" << endl; abort(); }  
}



void pointprocess :: transform ( int akseno, double & theta )
{
  if ( akseno < k && ( akseno == 0 || akseno == 1 || akseno == 2 ) )
    {
      int i;
      for ( i=0; i<n; i++ )
	data[i].transform(akseno,akse[akseno],theta);

      s=-1;   // Da antal naboer er forkert efter transformationen
      sw=-1;
    }
  else
    {
      cout << "Axis number is not valid in call transform" << endl;
      abort();
    }
}


void pointprocess :: invTransform ( int akseno, double & theta )
{
  if ( akseno < k && ( akseno == 0 || akseno == 1 || akseno == 2 ) )
    {
      int i;
      for ( i=0; i<n; i++ )
	data[i].invTransform(akseno,akse[akseno],theta);

      s=-1;   // Da antal naboer er forkert efter transformationen
      sw=-1;
    }
  else
    {
      cout << "Axis number is not valid in call invTransform" << endl;
      abort();
    }
}


void pointprocess :: invtransOtrans ( int akseno, double & theta1, double & theta2 )
{
  if ( akseno < k && ( akseno == 0 || akseno == 1 || akseno == 2 ) )
    {
      int i;
      for ( i=0; i<n; i++ )
	data[i].invtransOtrans(akseno,akse[akseno],theta1,theta2);

      s=-1;   // Da antal naboer er forkert efter transformationen
      sw=-1;
    }
  else
    {
      cout << "Axis number is not valid in call transform" << endl;
      abort();
    }
}



void pointprocess :: copy ( pointprocess *x )
{
  int i,j;

  n = x -> whatis_n();

  while ( n > maxlgd ) 
    extend_dataset();

  k = x -> whatis_k();

  for (i = 0; i < n; i++ )
    for (j = 0; j < k; j++ )
      data[i].pt[j] = x -> data[i].pt[j];

  s = x -> whatis_s(); 
  sw = x -> whatis_s_window();  
  R = x -> whatis_R();  

  for (j = 0; j < k; j++ )
    {
      akse[j] = x -> whatis_akse(j);
      windowL[j] = x -> whatis_L(j);
      windowU[j] = x -> whatis_U(j);
    }
}


void pointprocess :: resetcoords ( int coord, pointprocess *x )
{
  assert ( coord < k && (( coord==0 ) || ( coord==1 ) ||  ( coord==2 )) );
  // Index out of range: point number or coordinate

  assert ( k == x -> whatis_k() && n == x -> whatis_n() );
  // Error in resetcoords - dimension of k or n differ
       
  int i;
  for (i = 0; i < n; i++ ) data[i].pt[coord] = x -> data[i].pt[coord];

  s = x -> whatis_s(); 
  sw = x -> whatis_s_window();  
  R = x -> whatis_R();  

}


void pointprocess :: croptowindow ( void )
{
  int i, j, up = 0;

  int norig = n;

  n =  whatis_n_window();

  for (i = 0; i < norig; i++ )
    if ( inside(data[i]) ) 
      {
	for (j = 0; j < k; j++ )
	  data[up].pt[j] = data[i].pt[j] - windowL[j];
	
	up++;
      }


  s = -1;
  sw = -1;

  for (j = 0; j < k; j++ )
    {
      akse[j] = windowU[j]-windowL[j];
      windowL[j] = 0;
      windowU[j] = akse[j];
    }
  
}


bool pointprocess :: inside ( point eta )
{
  int j;
  for ( j = 0; j < k; j++ )
    {
      if ( eta.pt[j] < windowL[j] ) { return false; }
      if ( windowU[j] < eta.pt[j] ) { return false; }
    }
  return true;
}


double pointprocess :: whatis ( int i, int coord )
{
  if ( i < n && coord < k && (( coord==0 ) || ( coord==1 ) ||  ( coord==2 )) )
    return data[i].pt[coord];
  else 
    { 
      cout << "Index out of range: point number or coordinate" << endl; 
      abort(); 
    }
}


int pointprocess :: whatis_n ( void )
{
  return n;
}


int pointprocess :: whatis_n_window ( void )
{
  int res = 0, i;
  for ( i = 0; i < n; i++ )
    if ( inside(data[i]) ) 
      res++; 

  return res;
}


int pointprocess :: whatis_s ( void )
{
  if ( s == -1 ) { s = countNeighbours(); }
  return s;
}


int pointprocess :: whatis_s ( double afst )
{
  return countNeighbours(afst); 
}


int pointprocess :: whatis_s_window ( void )
{
  if ( sw < 0 ) { sw = countNeighbours_window(); }
  return sw;
}
  

int pointprocess :: whatis_k ( void )
{
  return k;
}


double pointprocess :: whatis_R ( void )
{
  return R;
}


double pointprocess :: whatis_akse ( int akseno )
{
  return akse[akseno];
}


double pointprocess :: whatis_L ( int akseno )
{
  return windowL[akseno];
}


double pointprocess :: whatis_U ( int akseno )
{
  return windowU[akseno];
}


double pointprocess :: whatis_sum ( int coord )
{
  double sum = 0;

  if ( coord < k && (( coord==0 ) || ( coord==1 ) ||  ( coord==2 )) )
    {   
      int i;
      
      for ( i = 0; i < n; i++ ) { sum += data[i].pt[coord]; }
    }
  else 
    { 
      cout << "Index out of range: coordinate (whatis_sum)" << endl; 
      abort();
    }

  return sum;
}


double pointprocess :: whatis_sum_window ( int coord )
{
  double sum = 0;

  if ( coord < k && (( coord==0 ) || ( coord==1 ) ||  ( coord==2 )) )
    {   
      int i;
      double tmp, a = windowL[coord], b = windowU[coord];
      // Kunne bruge inside() i stedet, men dette er hurtigere

      for ( i = 0; i < n; i++ ) 
	{ 
	  tmp = data[i].pt[coord]; 
	  if ( a < tmp && tmp < b ) { sum += tmp; }
	}
    }
  else 
    { 
      cout << "Index out of range: coordinate (whatis_sum_window)" << endl; 
      abort();
    }

  return sum;
}


double pointprocess :: dist ( int coord1, int coord2 )
{
  return data[coord1].lebdist(data[coord2],k);
}


void pointprocess :: triplets_canon ( int & nn, int & ss, int & ww )
{
  nn = n;
  ss = 0;
  ww = 0;

  int i, j, l;
  for ( i = 0; i < n; i++ )
    for ( j = 0; j < i; j++ )
      if ( data[i].relatedTo(k,R,data[j]) )
	{
	  ss++;
	  for ( l = 0; l < j; l++ )
	    if ( data[l].relatedTo(k,R,data[i]) && 
		 data[l].relatedTo(k,R,data[j]) )
	      ww++;
		    
		
	}
}
		  

int pointprocess :: quadcount2D ( double x1, double x2, double y1, double y2 )
{
  int antal=0; 
  int i;

  for ( i=0; i<n; i++ )
    if ( (x1-0.0000001 < data[i].pt[0]) && (data[i].pt[0] < x2) ) 
      if ( (y1-0.0000001 < data[i].pt[1]) && (data[i].pt[1] < y2) ) 
	antal++;  

  return antal;
}



void pointprocess :: countNeighboursTo( point eta, int & nn, int & nn_w )
{
  // NUMBER OF NEIGHBOURS TO THE PT eta IN  nn: S, nn_w: W
  // (INCLUSIVE eta ITSELF IF eta IN {pp snit S} and {pp snit W}, RESPECTIVELY)

  nn = 0; 
  nn_w = 0;
  int i;

  for ( i = 0; i < n; i++ )
    if ( eta.relatedTo(k, R, data[i]) ) 
      { 
	nn++; 
	if ( inside(data[i]) ) 
	  nn_w++; 
      }
    
}



int pointprocess :: countNeighbours(void )
{
  int ant=0; 
  int i; int j;

  for ( j=0; j<n; j++ )
    for ( i=0; i<j; i++ )
      if ( data[i].relatedTo(k,R,data[j]) )  
	ant++; 

  return ant;
}


int pointprocess :: countNeighbours( double afst )
{
  int ant=0; 
  int i; int j;

  for ( j=0; j<n; j++ )
    for ( i=0; i<j; i++ )
      if ( data[i].relatedTo(k,afst,data[j]) )  
	ant++; 

  return ant;
}


int pointprocess :: countNeighbours_window ( void )
{
  // TAELLER ANTAL NABOER I ET WINDUE. DVS PUNKT i SKAL LIGGE I W
  // MENS PUNKT j KAN LIGGE HVORSOMHELST I S
  // HINT: sw(x) = sum_{eta i x snit W} s(eta; x\eta)
  // = # naboer hvor mindst det ene punkt ligger i W
  
  bool pktiW[n];

  int i; int j, ant = 0; 

  for ( i=0; i<n; i++ )
    {
      if ( inside(data[i]) )
	pktiW[i] = true; 
      else  
	pktiW[i] = false; 
    }

  for ( i=0; i<n; i++ )
    for ( j=0; j<i; j++ )         // KUN TAELLE ET PUNKTPAR MED EN GANG 
      if ( pktiW[i] || pktiW[j] ) // MINDST ET AF DE TO PUNKTER SKAL LIGGE I W
	if ( data[i].relatedTo(k,R,data[j]) ) 
	  ant++; 

  return ant;
}



void  pointprocess :: binoprocess ( void )
{
  while ( n > maxlgd )
    extend_dataset();

  int i;
  for ( i=0; i<n; i++ ) 
    data[i].unifpoint(k,akse[0],akse[1],akse[2]); 
}


void pointprocess :: poissonprocess ( double intensity )
{
  double areaS = 1;
  int i;
  for ( i = 0; i < k; i++ )
    areaS *= akse[i];         // AREA/VOLUME OF OBSERVATION WINDOW S

  n = poisson(intensity*areaS); // ANT PKTER ER POISSON FORDELT
  binoprocess();
}



void pointprocess :: emptyprocess ( void ) 
{
  n = 0;
  s = 0;
  sw = 0;
}



void pointprocess :: makeMHBDstep ( void )
{
  int nn;     // Beregningen af antal naboer til nye punkt foregaar
  int nn_w;   // i acceptProb. Derfor laves en dummyvariabel som recorder
              // dette resultat saaledes at s kan opdateres               
	      // nn   = s(eta;x\eta snit S)  etas naboer i x i S 
              // nn_w = s(eta;x\eta snit W)  etas naboer i x i W 


  if ( drand48() > 0.5 )
    {
      // PROPOSAL: ADD A UNIFORMLY CHOSEN POINT IN S TO PP

      assert ( n <= maxlgd );  // Test the statement, abort if false with 
                               // appropiate error message
			       // Not supposed to be more points because 
                               // we increse with 1 point at a time

      if ( n == maxlgd ) 
	extend_dataset();

      
      data[n].unifpoint(k, akse[0], akse[1], akse[2]);
      // NEW POINT IS PLACED ON n'th ENTRY IN DATA (RIGHT AFTER DATA) (*)

      if ( acceptProb(n,nn,nn_w) > drand48() ) 
        {
          // VED ACCEPT SKAL PROCESSEN OPDATERES: n INCREASES 1 (*)
          //
          // s(xUeta) = s(x)+s(eta;x)
          // eta i W:      sw(xUeta) = sw(x) + s(eta;x)
          // eta ikke i W: sw(xUeta) = sw(x) + s(eta;x snit W)

          s += nn;
          if ( inside(data[n]) ) { sw += nn; }
          else { sw += nn_w; }

          n++;     

        }
    }
  else
    {
      // PROPOSAL: DELETE A UNIFORMLY CHOSEN POINT FROM PP

      if ( n>0 )   // OBS: n=0 => ACCEPTPROB = 0
        {
          int etaIndex = (int) (drand48()*n); 
          // CHOSEN POINT IS ENTRY NO etaIndex IN PP: 0 <= etaIndex < n 

          if ( acceptProb(etaIndex,nn,nn_w) > drand48() ) 
       	    { 
              // VED ACCEPT SKAL PROCESSEN OPDATERES: MOVE LAST POINT TO
              // DELETED POINTS PLACE AND DECREASE n BY 1n INCREASES 1 (*)
              // 
              // s(x\eta)=s(x)-s(eta;x\eta)
              // eta i W => sw(x\eta)=sw(x)-s(eta;x\eta)
              // eta ikke i W => sw(x\eta)=sw(x)-s(eta;x\eta snit W)

              s -= nn;   
              if ( inside(data[etaIndex]) ) { sw -= nn; }
              else { sw -= nn_w; }

	      data[etaIndex].replaceWith( data[n-1] );
	      n--; 

      	    }
	}
    }
}




void pointprocess :: makeMHRstep ( void )   
{
  // Metropolis-Hastings replacement of a point
  // This is not refined to use existing information ss and ss_w

  point xi;
  xi.unifpoint(k, akse[0], akse[1], akse[2]); // Proposed new point xi

  int etaIndex;
  etaIndex = (int) (drand48()*n);   // Proposed existing point eta

  if ( drand48() < acceptProb(xi,etaIndex) )
        data[etaIndex].replaceWith(xi);
}


void pointprocess :: begin_chain_here ( int iterations )
{
  int i; 
  
  if ( randomnumber )
    for ( i = 0; i < iterations; i++ )
      makeMHBDstep();
  else
    for ( i = 0; i < iterations; i++ )
      makeMHRstep();
}



void pointprocess :: makeprocess ( int iterations )
{
  if ( randomnumber )
    emptyprocess();
  else
    binoprocess();

  begin_chain_here( iterations );

}



void pointprocess :: estimate_means ( int nburn, int nsamp, int nspac, 
                                      double & En, double & Es )
{
  double sum_n = 0, sum_s = 0;

  // first bring markov chain into equilibrium:
  makeprocess(nburn);

  int i;
  for ( i = 0; i < nsamp; i++ )
    {
      // then run nspac steps more in markov chain
      begin_chain_here(nspac);

      // and sample the number of points/neighbours
      sum_n += whatis_n();
      sum_s += whatis_s();
    }

  // Results:

  En = sum_n/nsamp;
  Es = sum_s/nsamp;

}



/*
double pointprocess :: Lnul ( double theta[k] )
{
  double res = 1;
  for ( i = 0; i < k; i++ )
    {
      res *= ((theta[1]*akse[i])/(exp(theta[i]*akse[i])-1)) 
             * exp(theta[i]*whatis_sum(i));
    }
  return res;
}
*/


//---------------------------------------------------------------------------
// ******************** SPECIALIZATION: STRAUSSPROCESS **********************
//---------------------------------------------------------------------------


straussprocess :: straussprocess ( int dim, double b, double gam, double afs )
{ 
  k = dim; 
  beta = b;
  gamma = gam;
  R = afs;
}


straussprocess :: straussprocess ( int dim, double b, double gam, double afs, int num )
{ 
  k = dim; 
  beta = b;
  gamma = gam;
  R = afs;
  n = num;
  randomnumber = false;
}



double straussprocess :: acceptProb( point xi, int etaIndex )
{
  // FIXED NUMBER CASE

  // CALCULATES h = f((xUxi)\eta) / f(x) = gamma^( s(x\eta;xi) - s(x\eta,eta) )

  int s_xi = 0; 
  int s_eta = 0;

  int j;
  for ( j=0; j<n; j++ )
    {
      if ( j != etaIndex && data[j].relatedTo(k,R,xi) )
            s_xi++;
      if ( j != etaIndex && data[j].relatedTo(k,R,data[etaIndex]) ) 
            s_eta++;
    }

  return pow(gamma,(s_xi-s_eta));
}


double straussprocess :: acceptProb( int j, int & nn, int & nn_w )
{
  // RANDOM NUMBER CASE

  // CALCULATES r OR 1/r (CASE: ADD OR DELETE) CF J. MOELLER (1999)
  //              ADD:    j=n   - NEW PT IS PUT AFTER END OF PP
  //              DELETE: 0<j<n - CHOSEN PT IS ON OF PTS FROM PP
  // 
  // ACCEPTPROB a = min( 1, r OR 1/r ) WHICH CAN BE USED DIRECTLY
  // AS: ( a > drand48() ) <==> ( r RESP 1/r > drand48() )

  double areaS = 1;
  int i;
  for ( i = 0; i < k; i++ )
    areaS *= akse[i];        // AREA/VOLUME OF OBSERVATION WINDOW S


  countNeighboursTo(data[j], nn, nn_w); // Det er her den store beregningsdel
                                        // af s foregaar, derfor overfoeres nn
                                        // og nn_w som dummy-variable (snyd)
					// nn_w bruges kun naar eta i S\W

  if ( j == n )
    {
      // r = beta * gamma^s(xi;x) * area(S) / (n(x)+1)

      return (double) areaS*beta*exp(log(gamma)*nn) / (n+1);

      // OBS: nn og nn_w korrekte idet data[n] ikke tilhoerer pp og derfor
      // ikke selv er blevet talt med som et punkt
    }
  else
    {
      // 1/r = n(x) / ( beta * gamma^s(eta;x) * area(S) )

      nn--;
      // -1 AS THE POINT ITSELF HAS BEEN COUNTED AS A NEIGHBOUR PAIR
      // OBS: Hvis eta ligger i W burde nn_w egentlig taelles 1 ned,
      // mens nn_w er korrekt hvis eta ligger i S\W.
      // Eftersom vi KUN bruger nn_w naar eta ligger i S\W taelles ikke ned

      return (double) n / ( areaS*beta*exp(log(gamma)*nn) );
    }
}
  


void straussprocess :: resetGamma ( double gam )
{
  gamma = gam;
  s = -1;
  sw = -1;
}


void straussprocess :: resetBeta ( double b )
{
  beta = b;
  s = -1;
  sw = -1;
}


double straussprocess :: estiNON ( void )
{ 
  makeprocess(50000);
  int oldNON = countNeighbours();
  int sum = 0;
  
  point xi;
  int i, j, etaIndex, s_xi, s_eta;
  double h;
  int NOI = 10000; 
  for ( i=0; i<NOI; i++ )
    { 
      xi.unifpoint(k, akse[0], akse[1], akse[2]);
      etaIndex = (int) (drand48()*n);
      s_xi = 0; 
      s_eta = 0;
      for ( j=0; j<n; j++ )
        {
          if ( j != etaIndex && data[j].relatedTo(k,R,xi) ) 
	    s_xi++; 
          if ( j != etaIndex && data[j].relatedTo(k,R,data[etaIndex]) ) 
	    s_eta++; 
        }
      h = pow(gamma,(s_xi-s_eta));
      if ( h > drand48() ) 
        { 
          data[etaIndex].replaceWith(xi); 
          oldNON += s_xi - s_eta;
        }
      sum += oldNON;
    }
  return (double) sum/NOI;
}


double straussprocess :: estiPOW ( double obsNON, double Gpow )
{ 
  makeprocess(50000);
  int oldNON = countNeighbours();
  double sumpow = 0;

  point xi;
  int i, j, etaIndex, s_xi, s_eta;
  double h;
  int NOI = 10000; 
  for ( i=0; i<NOI; i++ )
    { 
      xi.unifpoint(k, akse[0], akse[1], akse[2]);
      etaIndex = (int) (drand48()*n);
      s_xi = 0; 
      s_eta = 0;
      for ( j=0; j<n; j++ )
        {
          if ( j != etaIndex && data[j].relatedTo(k,R,xi) ) 
	    s_xi++; 
          if ( j != etaIndex && data[j].relatedTo(k,R,data[etaIndex]) ) 
	    s_eta++; 
        }
      h = pow(gamma,(s_xi-s_eta));
      if ( h > drand48() ) 
        { 
          data[etaIndex].replaceWith(xi); 
          oldNON += s_xi - s_eta;
        }
      sumpow += pow(Gpow,(obsNON-oldNON));
    }
  return sumpow/NOI;
}




//---------------------------------------------------------------------------
// ******************** SPECIALIZATION: GEYERPROCESS ************************
//---------------------------------------------------------------------------



geyerprocess :: geyerprocess
     ( int dim, double b, double gam, double afs, double upperlim, int num )
{ 
  k = dim; 
  beta = b;
  gamma = gam;
  R = afs;
  c = upperlim;
  n = num;
  randomnumber = false;
}


double geyerprocess :: acceptProb( point xi, int etaIndex )
{
  // FIXED NUMBER CASE

  // CALCULATES h = f((xUxi)\eta) / f(x)

  return 3.2;
  
}


double geyerprocess :: acceptProb( int j, int & nn, int & nn_w )
{
  // RANDOM NUMBER CASE

  // CALCULATES r OR 1/r (CASE: ADD OR DELETE) CF J. MOELLER (1999)
  //              ADD:    j=n   - NEW PT IS PUT AFTER END OF PP
  //              DELETE: 0<j<n - CHOSEN PT IS ON OF PTS FROM PP
  // 
  // ACCEPTPROB a = min( 1, r OR 1/r ) WHICH CAN BE USED DIRECTLY
  // AS: ( a > drand48() ) <==> ( r RESP 1/r > drand48() )

  double areaS = 1;
  int i;
  for ( i = 0; i < k; i++ )
    areaS *= akse[i];        // AREA/VOLUME OF OBSERVATION WINDOW S


  int antnabo, dummy;
  double sum = 0;

  if ( j == n )      
    {
      // Proposal: Add a point, data[n]
      // xi = *data[j] og u = data[i]
      // r = beta/(n+1) * gamma^( min(c,s(xi,x)) + 
      //             SUM_{u i x, u~xi}[ min(c,s(u,x)+1)-min(c,s(u,x)) ] )

      for ( i = 0; i < n; i++ )
        {
           if ( data[j].relatedTo(k, R, data[i]) )
             {
                countNeighboursTo(data[i], antnabo, dummy);
		antnabo--;  // data[i] er selv blevet talt som nabo
                if ( antnabo < c-1 ) { sum++; }
                if ( (c-1 < antnabo) && (antnabo < c) ) { sum += c-antnabo; }
		// IF ( antnabo >= c ) ADD 0
		// NOTICE: PROBLEM WITH c BEING INTEGER IN CASE: antnabo==c-1
             }
        }

      countNeighboursTo(data[j], nn, nn_w);  
      // Dette er det ene af 2 steder i acceptProb hvor den store beregningsdel
      //  af s foregaar, derfor overfoeres nn og nn_w som dummy-variable (snyd)

      if ( nn < c ) { sum += nn; }
      else { sum += c; }

      return (double) areaS*beta*exp( log(gamma)*sum ) / (n+1);

    }
  else    
    {
      // Proposal: Deleta a point, data[j]
      // eta = data[j] og u = data[i]
      // r = beta/(n) * gamma^(  min(c,s(eta,x)) + 
      //       SUM_{u i x\{eta}, u~eta}[ min(c,s(u,x))-min(c,s(u,x)-1) ] 

      for ( i = 0; i < n; i++ )
        {
           if ( data[j].relatedTo(k, R, data[i]) && (i != j) )
             {
                countNeighboursTo(data[i], antnabo, dummy);
		antnabo--;  // data[i] er selv blevet talt som nabo
                if ( antnabo < c ) { sum++; }
                if ( (c < antnabo) && (antnabo < c+1) ) { sum += c-antnabo+1; }
		// IF ( antnabo >= c+1 ) ADD 0
		// NOTICE: PROBLEM WITH c BEING INTEGER IN CASE: antnabo==c
	     }
	}

      countNeighboursTo(data[j], nn, nn_w);
      nn--;
      // -1 AS THE POINT ITSELF HAS BEEN COUNTED AS A NEIGHBOUR PAIR
      // Dette er det andet af 2 steder i acceptProb hvor store beregningsdel
      //  af s foregaar, derfor overfoeres nn og nn_w som dummy-variable (snyd)
      // Egentlig skal nn_w ogsaa taelles ned hvis eta ligger i W, men da vi
      // kun bruger ss_w naar eta ligger i S\W taelles ikke ned

      if ( nn < c ) { sum += nn; }
      else { sum += c; }

      return (double) n / ( areaS*beta*exp( log(gamma)*sum ) );

    }
}



double geyerprocess :: u ( void )
{
  double u = 0;
  double sum; 
  int i; int j;

  for ( j=0; j<n; j++ )
    {
      sum = 0;
      for ( i=0; i<n; i++ )
	if ( (i != j) && (sum < c) )
	  if ( data[i].relatedTo(k,R,data[j]) )  
	    sum += 1.0; 


      if ( sum < c ) { u += sum; }
      else { u += c; }
    }

  return u;
}


void geyerprocess :: reset_c ( double cval )
{
  c = cval;
  s = -1;
  sw = -1;
}


#endif











