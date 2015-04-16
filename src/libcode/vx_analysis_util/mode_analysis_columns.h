// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2015
// ** University Corporation for Atmospheric Research (UCAR)
// ** National Center for Atmospheric Research (NCAR)
// ** Research Applications Lab (RAL)
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*



////////////////////////////////////////////////////////////////////////


    //
    //   Warning:
    //
    //      This file is machine generated.
    //
    //      Do not edit by hand
    //
    //
    //      File generated by program "make_columns_header"
    //
    //      on Dec 28, 2007   2:11 pm MST
    //


////////////////////////////////////////////////////////////////////////


#ifndef  __MODE_ANALYSIS_COLUMNS_H__
#define  __MODE_ANALYSIS_COLUMNS_H__


////////////////////////////////////////////////////////////////////////


static const char * lc_mode_columns [] = {

   "version",                    //   1
   "model",                      //   2
   "fcst_lead",                  //   3
   "fcst_valid",                 //   4
   "fcst_accum",                 //   5

   "obs_lead",                   //   6
   "obs_valid",                  //   7
   "obs_accum",                  //   8
   "fcst_rad",                   //   9
   "fcst_thr",                   //  10

   "obs_rad",                    //  11
   "obs_thr",                    //  12
   "fcst_var",                   //  13
   "fcst_lev",                   //  14
   "obs_var",                    //  15

   "obs_lev",                    //  16
   "object_id",                  //  17
   "object_cat",                 //  18
   "centroid_x",                 //  19
   "centroid_y",                 //  20

   "centroid_lat",               //  21
   "centroid_lon",               //  22
   "axis_ang",                   //  23
   "length",                     //  24
   "width",                      //  25

   "area",                       //  26
   "area_filter",                //  27
   "area_thresh",                //  28
   "curvature",                  //  29
   "curvature_x",                //  30
   "curvature_y",                //  31

   "complexity",                 //  32
   "intensity_10",               //  33
   "intensity_25",               //  34
   "intensity_50",               //  35
   "intensity_75",               //  36

   "intensity_90",               //  37
   "intensity_user",             //  38
   "intensity_sum",              //  39
   "centroid_dist",              //  40
   "boundary_dist",              //  41

   "convex_hull_dist",           //  42
   "angle_diff",                 //  43
   "area_ratio",                 //  44
   "intersection_area",          //  45
   "union_area",                 //  46

   "symmetric_diff",             //  47
   "intersection_over_area",     //  48
   "complexity_ratio",           //  49
   "percentile_intensity_ratio", //  50
   "interest"                    //  51

};


////////////////////////////////////////////////////////////////////////


static const int n_mode_columns = sizeof(lc_mode_columns)/sizeof(*lc_mode_columns);


////////////////////////////////////////////////////////////////////////


   //
   //  mode column numbers
   //


static const int version_column                     =  0;
static const int model_column                       =  1;
static const int fcst_lead_column                   =  2;
static const int fcst_valid_column                  =  3;
static const int fcst_accum_column                  =  4;

static const int obs_lead_column                    =  5;
static const int obs_valid_column                   =  6;
static const int obs_accum_column                   =  7;
static const int fcst_rad_column                    =  8;
static const int fcst_thr_column                    =  9;

static const int obs_rad_column                     = 10;
static const int obs_thr_column                     = 11;
static const int fcst_var_column                    = 12;
static const int fcst_lev_column                    = 13;
static const int obs_var_column                     = 14;

static const int obs_lev_column                     = 15;
static const int object_id_column                   = 16;
static const int object_cat_column                  = 17;
static const int centroid_x_column                  = 18;
static const int centroid_y_column                  = 19;

static const int centroid_lat_column                = 20;
static const int centroid_lon_column                = 21;
static const int axis_ang_column                    = 22;
static const int length_column                      = 23;
static const int width_column                       = 24;

static const int area_column                        = 25;
static const int area_filter_column                 = 26;
static const int area_thresh_column                 = 27;
static const int curvature_column                   = 28;
static const int curvature_x_column                 = 29;

static const int curvature_y_column                 = 30;
static const int complexity_column                  = 31;
static const int intensity_10_column                = 32;
static const int intensity_25_column                = 33;
static const int intensity_50_column                = 34;

static const int intensity_75_column                = 35;
static const int intensity_90_column                = 36;
static const int intensity_user_column              = 37;
static const int intensity_sum_column               = 38;
static const int centroid_dist_column               = 39;

static const int boundary_dist_column               = 40;
static const int convex_hull_dist_column            = 41;
static const int angle_diff_column                  = 42;
static const int area_ratio_column                  = 43;
static const int intersection_area_column           = 44;

static const int union_area_column                  = 45;
static const int symmetric_diff_column              = 46;
static const int intersection_over_area_column      = 47;
static const int complexity_ratio_column            = 48;
static const int percentile_intensity_ratio_column  = 49;

static const int interest_column                    = 50;


////////////////////////////////////////////////////////////////////////


#endif   /*  __MODE_ANALYSIS_COLUMNS_H__  */


////////////////////////////////////////////////////////////////////////


