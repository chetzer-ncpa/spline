#include <iostream>
#include <fstream>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <cstdlib>
#include <cmath>
#include <iomanip>

using namespace std;

int main( int argc, char **argv ) {
  
  if (argc != 7) {
    cerr << "Usage: " << endl 
	  << "spline <input file> <input lines> <start time> <end time> <time interval> <output data file>" << endl;
    return 1;
  }
  
  int npts = atoi( argv[ 2 ] );
  double new_t0 = atof( argv[ 3 ] );
  double new_t1 = atof( argv[ 4 ] );
  double new_dt = atof( argv[ 5 ] );
  char *targetfile = argv[ 6 ];
  
  if (npts == 0 || new_dt == 0.0) {
    cerr << "Error with input arguments." << endl;
    return 1;
  }
  
  // read in the source data
  ifstream orig_data_stream( argv[ 1 ], ios_base::in );
  if (!(orig_data_stream.good())) {
    cerr << "Error with file " << argv[ 1 ] << "...aborting." << endl;
    return 1;
  }
  
  double *t0 = new double[ npts ];
  double *d0 = new double[ npts ];
  
  double temp1, temp2;
  orig_data_stream >> temp1 >> temp2;
  int counter = 0;
  while (orig_data_stream.good()) {
    t0[counter] =  temp1;
    d0[counter++] = temp2;
    orig_data_stream >> temp1 >> temp2;
    if (div(counter,1000000).rem == 0) {
      cout << "Read " << counter << " values." << endl;
    }
  }
  orig_data_stream.close();
  
  if (counter != npts) {
    cerr << npts << " points expected but " << counter << " points read!  Aborting..." << endl;
    delete [] t0;
    delete [] d0;
    return 1;
  }
  cout << "read " << npts << " values." << endl;
 
  // open the output file
  ofstream new_data_stream( targetfile, ios_base::out );
  if (!(new_data_stream.good())) {
    cerr << "Error opening target file " << targetfile << "...aborting." << endl;
    new_data_stream.close();
    delete [] t0;
    delete [] d0;
    return 1;
  }
  new_data_stream.setf( ios::fixed, ios::floatfield );
 
  
  // set up spline
  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  gsl_spline *spline = gsl_spline_alloc( gsl_interp_cspline, npts );
  gsl_spline_init( spline, t0, d0, npts );
  
  // get new data values
  double t, new_d;
  int n_new_pts = (int)round((new_t1 - new_t0) / new_dt) + 1;
  //for (double t = new_t0; t <= new_t1; t += new_dt) {
  for (int i = 0; i < n_new_pts; i++) {
    t = new_t0 + (double)i * new_dt;
    //if (t > new_t1) break;    // just in case we overshoot with 
    new_d = gsl_spline_eval( spline, t, acc );
    new_data_stream << setprecision( 5 ) << t << "   " << setprecision( 9 ) << new_d << endl;
  }
  
  gsl_spline_free( spline );
  gsl_interp_accel_free( acc );
  
  delete [] t0;
  delete [] d0;

  
  return 0;
}