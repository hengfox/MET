

// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2007
// ** University Corporation for Atmospheric Research (UCAR)
// ** National Center for Atmospheric Research (NCAR)
// ** Research Applications Lab (RAL)
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*




////////////////////////////////////////////////////////////////////////


   //
   //  Thanks to John Halley Gotway for collecting the grid info
   //


////////////////////////////////////////////////////////////////////////


using namespace std;

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

#include "vx_data_grids/grid.h"


////////////////////////////////////////////////////////////////////////


   //
   //  DTC Lambert grids
   //


static const LambertData dtc_lambert_grids [] = { 

   { "DTC164", 30, 48, 20.47,  122.042, 0.0, 0.0, 98.8, 13.3, ncep_earth_radius_km, 376, 280 },
   { "DTC165", 30, 48, 20.653, 121.907, 0.0, 0.0, 98.8, 13.3, ncep_earth_radius_km, 168, 280 },
   { "DTC166", 30, 48, 23.114, 100.997, 0.0, 0.0, 98.8, 13.3, ncep_earth_radius_km, 208, 280 },

};


static const int n_dtc_lambert_grids = sizeof(dtc_lambert_grids)/sizeof(*dtc_lambert_grids);


////////////////////////////////////////////////////////////////////////


   //
   //  NCEP Lat/Lon (PlateCarree) Grids
   //


static const LatLonData ncep_latlon_grids [] = {

   { "G002", -90.0,     -0.0,   2.5,   2.5,    73, 144 }, 
   { "G003", -90.0,     -0.0,   1,     1.0,   181, 360 }, 
   { "G004", -90.0,     -0.0,   0.5,   0.5,   361, 720 }, 
   { "G029",   0.0,     -0.0,   2.5,   2.5,    37, 145 }, 
   { "G030", -90.0,     -0.0,   2.5,   2.5,    37, 145 }, 
   { "G033",   0.0,     -0.0,   2,     2,      46, 181 }, 
   { "G034", -90.0,     -0.0,   2,     2,      46, 181 }, 
   { "G045", -90.0,     -0.0,   1.25,  1.25,  145, 288 }, 
   { "G085",   0.5,     -0.5,   1,     1.0,    90, 360 }, 
   { "G086", -89.5,     -0.5,   1,     1.0,    90, 360 }, 
   { "G110",  25.063,  124.938, 0.125, 0.125, 224, 464 }, 
   { "G175",   0.0,   -130.0,   0.09,  0.09,  334, 556 }, 
   { "G228", -90.0,     -0.0,   2.5,   2.5,    73, 144 }, 
   { "G229", -90.0,     -0.0,   1,     1.0,   181, 360 }, 
   { "G230", -90.0,     -0.0,   0.5,   0.5,   361, 720 }, 
   { "G231",   0.0,     -0.0,   0.5,   0.5,   181, 720 }, 
   { "G232",   0.0,     -0.0,   1,     1.0,    91, 360 }, 
   { "G233", -78.0,     -0.0,   1,     1.25,  157, 288 }, 
   { "G234", -45.0,     98.0,   0.25,  0.25,  241, 133 }, 
   { "G243",  10.0,    170.0,   0.4,   0.4,   101, 126 }, 
   { "G248",  14.5,     71.5,   0.075, 0.075, 101, 135 }, 
   { "G250",  16.5,    162.0,   0.075, 0.075, 101, 135 }, 
   { "G251",  26.35,    83.05,  0.1,   0.1,   210, 332 }, 

};

static const int n_ncep_latlon_grids = sizeof(ncep_latlon_grids)/sizeof(*ncep_latlon_grids);


////////////////////////////////////////////////////////////////////////


   //
   //  NCEP Stereographic grids
   //


static const StereographicData ncep_st_grids [] = {

      //
      //  Northern Hemisphere grids
      //

   { "G005", 'N',  60.0,   7.647,  133.443, 0.0, 0.0,  105.0, 190.5,    ncep_earth_radius_km,   53,  57 },
   { "G006", 'N',  60.0,   7.647,  133.443, 0.0, 0.0,  105.0, 190.5,    ncep_earth_radius_km,   53,  45 },
   { "G027", 'N',  60.0, -20.826,  125.0,   0.0, 0.0,   80.0, 381.0,    ncep_earth_radius_km,   65,  65 },
   { "G055", 'N',  60.0, -10.947,  154.289, 0.0, 0.0,  105.0, 254.0,    ncep_earth_radius_km,   87,  71 },
   { "G056", 'N',  60.0,   7.647,  133.443, 0.0, 0.0,  105.0, 127.0,    ncep_earth_radius_km,   87,  71 },
   { "G087", 'N',  60.0,  22.876,  120.491, 0.0, 0.0,  105.0,  68.153,  ncep_earth_radius_km,   81,  62 },
   { "G088", 'N',  60.0,  10.0,    128.0,   0.0, 0.0,  105.0,  15.0,    ncep_earth_radius_km,  580, 548 },
   { "G100", 'N',  60.0,  17.108,  129.296, 0.0, 0.0,  105.0,  91.452,  ncep_earth_radius_km,   83,  83 },
   { "G101", 'N',  60.0,  10.528,  137.146, 0.0, 0.0,  105.0,  91.452,  ncep_earth_radius_km,  113,  91 },
   { "G103", 'N',  60.0,  22.405,  121.352, 0.0, 0.0,  105.0,  91.452,  ncep_earth_radius_km,   65,  56 },
   { "G104", 'N',  60.0,  -0.268,  139.475, 0.0, 0.0,  105.0,  90.755,  ncep_earth_radius_km,  147, 110 },
   { "G105", 'N',  60.0,  17.529,  129.296, 0.0, 0.0,  105.0,  90.755,  ncep_earth_radius_km,   83,  83 },
   { "G106", 'N',  60.0,  17.533,  129.296, 0.0, 0.0,  105.0,  45.373,  ncep_earth_radius_km,  165, 117 },
   { "G107", 'N',  60.0,  23.438,  120.168, 0.0, 0.0,  105.0,  45.373,  ncep_earth_radius_km,  120,  92 },
   { "G201", 'N',  60.0, -20.826,  150.0,   0.0, 0.0,  105.0, 381.0,    ncep_earth_radius_km,   65,  65 },
   { "G202", 'N',  60.0,   7.838,  141.028, 0.0, 0.0,  105.0, 190.5,    ncep_earth_radius_km,   65,  43 },
   { "G203", 'N',  60.0,  19.132,  185.837, 0.0, 0.0,  150.0, 190.5,    ncep_earth_radius_km,   45,  39 },
   { "G205", 'N',  60.0,   0.616,  84.904,  0.0, 0.0,   60.0, 190.5,    ncep_earth_radius_km,   45,  39 },
   { "G207", 'N',  60.0,  42.085,  175.641, 0.0, 0.0,  150.0,  95.25,   ncep_earth_radius_km,   49,  35 },
   { "G213", 'N',  60.0,   7.838,  141.028, 0.0, 0.0,  105.0,  95.25,   ncep_earth_radius_km,  129,  85 },
   { "G214", 'N',  60.0,  42.085,  175.641, 0.0, 0.0,  150.0,  47.625,  ncep_earth_radius_km,   97,  69 },
   { "G216", 'N',  60.0,  30.0,    173.0,   0.0, 0.0,  135.0,  45.0,    ncep_earth_radius_km,  139, 107 },
   { "G217", 'N',  60.0,  30.0,    173.0,   0.0, 0.0,  135.0,  22.5,    ncep_earth_radius_km,  277, 213 },
   { "G223", 'N',  60.0, -20.826,  150.0,   0.0, 0.0,  105.0, 190.5,    ncep_earth_radius_km,  129, 129 },
   { "G240", 'N',  60.0,  23.098,  119.036, 0.0, 0.0,  105.0,   4.7625, ncep_earth_radius_km, 1121, 881 },
   { "G242", 'N',  60.0,  30.0,    173.0,   0.0, 0.0,  135.0,  11.25,   ncep_earth_radius_km,  553, 425 },
   { "G249", 'N',  60.0,  45.4,    171.6,   0.0, 0.0,  150.0,   9.868,  ncep_earth_radius_km,  367, 343 },

      //
      //  Southern Hemisphere grids
      //

   // { "G028", 'S', -60.0,  20.826, -145.0,   0.0, 0.0, -100.0, 381.0,    ncep_earth_radius_km,   65,  65 },
   // { "G224", 'S',  60.0,  20.826, -120.0,   0.0, 0.0,  105.0, 381.0,    ncep_earth_radius_km,   65,  65 },
      { "G224", 'S', -60.0, -90.0,      0.0,  32.0, 32.0,  -75.0, 381.0,    ncep_earth_radius_km,   65,  65 },

};

static const int n_ncep_st_grids = sizeof(ncep_st_grids)/sizeof(*ncep_st_grids);


////////////////////////////////////////////////////////////////////////


   //
   //  NCEP Lambert grids
   //


static const LambertData ncep_lambert_grids [] = {


   { "G130", 25.0, 25.0, 16.281,  126.138, 0.0, 0.0,  95.0, 13.545087, ncep_earth_radius_km,  451,  337 },
   { "G145", 36.0, 46.0, 32.174,   90.159, 0.0, 0.0,  79.5, 12.0,      ncep_earth_radius_km,  169,  145 },
   { "G146", 36.0, 46.0, 32.353,   89.994, 0.0, 0.0,  79.5, 12.0,      ncep_earth_radius_km,  166,  142 },
   { "G163", 38.0, 38.0, 20.6,    118.3,   0.0, 0.0,  95.0,  5.0,      ncep_earth_radius_km, 1008,  722 },
   { "G206", 25.0, 25.0, 22.289,  117.991, 0.0, 0.0,  95.0, 81.271,    ncep_earth_radius_km,   51,   41 },
   { "G209", 45.0, 45.0, -4.85,   151.1,   0.0, 0.0, 111.0, 44.0,      ncep_earth_radius_km,  275,  223 },
   { "G211", 25.0, 25.0, 12.19,   133.459, 0.0, 0.0,  95.0, 81.271,    ncep_earth_radius_km,   93,   65 },
   { "G212", 25.0, 25.0, 12.19,   133.459, 0.0, 0.0,  95.0, 40.635,    ncep_earth_radius_km,  185,  129 },
   { "G215", 25.0, 25.0, 12.19,   133.459, 0.0, 0.0,  95.0, 20.318,    ncep_earth_radius_km,  369,  257 },
   { "G218", 25.0, 25.0, 12.19,   133.459, 0.0, 0.0,  95.0, 12.191,    ncep_earth_radius_km,  614,  428 },
   { "G221", 50.0, 50.0, 1.0,     145.5,   0.0, 0.0, 107.0, 32.463,    ncep_earth_radius_km,  349,  277 },
   { "G222", 45.0, 45.0, -4.85,   151.1,   0.0, 0.0, 111.0, 88.0,      ncep_earth_radius_km,  138,  112 },
   { "G226", 25.0, 25.0, 12.19,   133.459, 0.0, 0.0,  95.0, 10.159,    ncep_earth_radius_km,  737,  513 },
   { "G227", 25.0, 25.0, 12.19,   133.459, 0.0, 0.0,  95.0, 5.079,     ncep_earth_radius_km, 1473, 1025 },
   { "G236", 25.0, 25.0, 16.281, -233.862, 0.0, 0.0,  95.0, 40.635,    ncep_earth_radius_km,  151,  113 },
   { "G237", 50.0, 50.0, 16.201, -285.72,  0.0, 0.0, 107.0, 32.463,    ncep_earth_radius_km,   54,   47 },
   { "G241", 45.0, 45.0, -4.85,   151.1,   0.0, 0.0, 111.0, 22.0,      ncep_earth_radius_km,  549,  445 },
   { "G245", 35.0, 35.0, 22.98,    92.84,  0.0, 0.0,  80.0,  8.0,      ncep_earth_radius_km,  336,  372 },
   { "G246", 40.0, 40.0, 25.97,   127.973, 0.0, 0.0, 115.0,  8.0,      ncep_earth_radius_km,  332,  371 },
   { "G247", 35.0, 35.0, 22.98,   110.84,  0.0, 0.0,  98.0,  8.0,      ncep_earth_radius_km,  336,  372 },
   { "G252", 25.0, 25.0, 16.281,  126.138, 0.0, 0.0,  95.0, 20.317,    ncep_earth_radius_km,  301,  225 },

};


static const int n_ncep_lambert_grids = sizeof(ncep_lambert_grids)/sizeof(*ncep_lambert_grids);


////////////////////////////////////////////////////////////////////////


   //
   //  NCEP Mercator grids
   //
   //    Note: Do not define NCEP Grid number 8 since it's range of longitude is 363.104, and the latlon_to_xy
   //          routine won't be well defined.
   //


static const MercatorData ncep_mercator_grids [] = {

   { "G001", -48.090,    0.000, 48.090,   0.000,  73,  23 }, 
   { "G053", -61.050,    0.000, 61.050,   0.000, 117,  51 }, 
   { "G195",  16.829,   68.196, 19.747,  63.972, 177, 129 }, 
   { "G196",  18.067,  161.626, 23.082, 153.969, 321, 225 }, 
   { "G199",  12.350,  216.314, 16.794, 179.960, 193, 193 }, 
   { "G204", -25.000, -110.000, 60.644, 109.129,  93,  68 }, 
   { "G208",   9.343,  167.315, 28.092, 145.878,  29,  27 }, 
   { "G210",   9.000,   77.000, 26.422,  58.625,  25,  25 }, 
   { "G225", -25.000,  250.000, 60.640, 109.129, 185, 135 }, 
   { "G254", -35.000,  250.000, 60.789, 109.129, 369, 300 }, 

};

static const int n_ncep_mercator_grids = sizeof(ncep_mercator_grids)/sizeof(*ncep_mercator_grids);


////////////////////////////////////////////////////////////////////////


   //
   //  Code for misc functions
   //


////////////////////////////////////////////////////////////////////////


bool find_grid_by_name(const char * name, Grid & g)

{

GridInfo i;
bool status = false;

status = find_grid_by_name(name, i);

if ( !status || !(i.ok()) )  return ( false );

status = false;

if ( i.lc )  { g.set( *(i.lc) );  status = true; }
if ( i.st )  { g.set( *(i.st) );  status = true; }
if ( i.ll )  { g.set( *(i.ll) );  status = true; }
if ( i.m  )  { g.set( *(i.m)  );  status = true; }

return ( status );

}


////////////////////////////////////////////////////////////////////////


bool find_grid_by_name(const char * name, GridInfo & i)

{

int j;

i.clear();

   //
   //  try dtc lambert grids
   //

for (j=0; j<n_dtc_lambert_grids; ++j)  {

   if ( strcmp(name, dtc_lambert_grids[j].name) == 0 )  {

      i.set( dtc_lambert_grids[j] );

      return ( true );

   }

}

   //
   //  try ncep latlon grids
   //

for (j=0; j<n_ncep_latlon_grids; ++j)  {

   if ( strcmp(name, ncep_latlon_grids[j].name) == 0 )  {

      i.set( ncep_latlon_grids[j] );

      return ( true );

   }

}

   //
   //  try ncep stereographic grids
   //

for (j=0; j<n_ncep_st_grids; ++j)  {

   if ( strcmp(name, ncep_st_grids[j].name) == 0 )  {

      i.set( ncep_st_grids[j] );

      return ( true );

   }

}


   //
   //  try ncep lambert grids
   //

for (j=0; j<n_ncep_lambert_grids; ++j)  {

   if ( strcmp(name, ncep_lambert_grids[j].name) == 0 )  {

      i.set( ncep_lambert_grids[j] );

      return ( true );

   }

}


   //
   //  try ncep mercator grids
   //

for (j=0; j<n_ncep_mercator_grids; ++j)  {

   if ( strcmp(name, ncep_mercator_grids[j].name) == 0 )  {

      i.set( ncep_mercator_grids[j] );

      return ( true );

   }

}

   //
   //  nope
   //

return ( false );

}


////////////////////////////////////////////////////////////////////////



