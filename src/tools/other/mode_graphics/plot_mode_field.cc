// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2015
// ** University Corporation for Atmospheric Research (UCAR)
// ** National Center for Atmospheric Research (NCAR)
// ** Research Applications Lab (RAL)
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*


////////////////////////////////////////////////////////////////////////


static const int anno_height       = 100;

static const int ctable_width      =  15;

static const int ctable_text_width =  30;


////////////////////////////////////////////////////////////////////////


using namespace std;


////////////////////////////////////////////////////////////////////////


using namespace std;

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>
#include <cmath>

#include "cgraph.h"

#include "mode_nc_output_file.h"

#include "vx_plot_util.h"
#include "vx_config.h"
#include "configobjecttype_to_string.h"


////////////////////////////////////////////////////////////////////////


static ConcatString program_name;

static CommandLine cline;

static ColorTable ctable;

static MetConfig config;

static Dictionary * sources;

static int n_sources = 0;

static double M = 1.0;
static double B = 1.0;

static const char        filename_name [] = "file_name";
static const char      fontfamily_name [] = "font_family";
static const char          outdir_name [] = "output_directory";
static const char        plotinfo_name [] = "plot_info";
static const char        plotsize_name [] = "size";
static const char     borderwidth_name [] = "border_width";
static const char         mapinfo_name [] = "map_info";
static const char       linecolor_name [] = "line_color";
static const char       linewidth_name [] = "line_width";
static const char          source_name [] = "source";
static const char       rawctable_name [] = "raw_ctable_filename";
static const char       objctable_name [] = "obj_ctable_filename";
static const char          doanno_name [] = "do_annotation";
static const char     annobgcolor_name [] = "anno_background_color";
static const char   annotextcolor_name [] = "anno_text_color";
static const char    annofontsize_name [] = "anno_font_size";
static const char   titlefontsize_name [] = "title_font_size";
static const char  ctablefontsize_name [] = "colortable_font_size";

enum PlotField {

   raw_field, 

   simple_obj_field, 

   composite_obj_field, 

   no_plot_field

};

static const Color black (  0,   0,   0);
static const Color white (255, 255, 255);
static const Color blue  (  0,   0, 255);

static const Color unmatched_color = blue;

static int Nx = 0;
static int Ny = 0;

static bool fcst_obs_set = false;

static bool do_obs = true;

static bool do_anno = true;

static bool do_data_rescale = false;

static ConcatString ctable_filename;
static ConcatString output_directory = ".";

   //
   //  default plot info
   //

static int plot_size = 4;

static int border_width = 10;

static int colortable_font_size = 10.0;

static Color anno_bg_color    = white;
static Color anno_text_color  = black;

static ConcatString raw_ctable_filename;
static ConcatString obj_ctable_filename;

static FontFamily family = ff_Helvetica;

static double title_font_size = 30.0;
static double anno_font_size  = 20.0;

static double map_width = 0.0;


////////////////////////////////////////////////////////////////////////


   //
   //  default values for command-line switches
   //

static PlotField plot_field = no_plot_field;

static ConcatString config_filename;   //  no default ... must be set on command line


////////////////////////////////////////////////////////////////////////


static void usage();

static void set_config    (const StringArray &);

static void set_obs       (const StringArray &);
static void set_fcst      (const StringArray &);

static void set_raw       (const StringArray &);
static void set_simple    (const StringArray &);
static void set_composite (const StringArray &);

static void set_logfile   (const StringArray &);
static void set_verbosity (const StringArray &);

static void sanity_check();

static void read_config();

static void do_plot (const char * mode_nc_filename);

static void get_data_ppm (ModeNcOutputFile &, Ppm &);

static void fill_box (const Box &, const Color &, Cgraph &);
static void clip_box (const Box &, Cgraph &);

static void draw_map (Cgraph &, const Box & map_box, const Grid &);

static void draw_mapfile (Cgraph &, const Box & map_box, const Grid &, Dictionary *);

static void draw_region  (Cgraph &, const Grid &, const Box & map_box, const MapRegion &);

static Color        get_dict_color  (Dictionary *, const char * id);
static int          get_dict_int    (Dictionary *, const char * id);
static double       get_dict_double (Dictionary *, const char * id);
static ConcatString get_dict_string (Dictionary *, const char * id);
static bool         get_dict_bool   (Dictionary *, const char * id);

static void time_string(int seconds, char * out, const int len);

static void annotate(const ModeNcOutputFile &, Cgraph &, const Box &, const Box &);

static double calc_text_scale(Cgraph &, const double target_width, const char *);

static void draw_obj_colortable(Cgraph &, const Box & map_box, const int Nobjs);
static void draw_raw_colortable(Cgraph &, const Box & map_box);


////////////////////////////////////////////////////////////////////////


int main(int argc, char * argv [])

{

program_name = get_short_name(argv[0]);

cline.set(argc, argv);

cline.set_usage(usage);

cline.add(set_config,    "-config",    1);

cline.add(set_obs,       "-obs",       0);
cline.add(set_fcst,      "-fcst",      0);

cline.add(set_raw,       "-raw",       0);
cline.add(set_simple,    "-simple",    0);
cline.add(set_composite, "-composite", 0);

cline.add(set_logfile,   "-log",       1);
cline.add(set_verbosity, "-v",         1);

cline.parse();

if ( cline.n() == 0 )  usage();

if ( config_filename.empty() )  {

   mlog << Error
        << "\n\n  " << program_name << ": no config file set!\n\n";

   exit ( 1 );

}

read_config();

sanity_check();

int j;

for (j=0; j<(cline.n()); ++j)  {

   mlog << Debug(2)
        << "Making plot " << (j + 1) << " of " << cline.n() << '\n';

   do_plot(cline[j]);

}


return ( 0 );

}


////////////////////////////////////////////////////////////////////////


void read_config()

{

mlog << Debug(1)
     << "Config file: " << config_filename << "\n";

if ( ! config.read(config_filename) )  {

   mlog << Error
        << "\n\n  " << program_name << ": read_config() -> unable to read config file \"" 
        << config_filename << "\"\n\n";

   exit ( 1 );

}

if ( mlog.verbosity_level() >= 5 ) config.dump(cout);

const DictionaryEntry * e = 0;
const char * name = 0;
Dictionary * map_info = 0;
Dictionary * plot_info = 0;
ConcatString s;

   //
   // plot info
   //

name = plotinfo_name;

e = config.lookup(name);

if ( !e )   {

   mlog << Error
        << "\n\n  " << program_name << ": read_config() -> lookup failed for plotinfo id \"" 
        << name << "\"\n\n";

   exit ( 1 );

}

plot_info = e->dict_value();

s = get_dict_string(plot_info, fontfamily_name);

     if ( s == "Helvetica")   family = ff_Helvetica;
else if ( s == "NewCentury")  family = ff_NewCentury;
else if ( s == "Palatino")    family = ff_Palatino;
else if ( s == "Times")       family = ff_Times;
else if ( s == "Courier")     family = ff_Courier;
else {

   mlog << Error 
        << "\n\n  " << program_name << ": bad font family \"" 
        << s << "\" in config file\n\n";

   exit ( 1 );

}

s                     = get_dict_string(plot_info, outdir_name);
output_directory      = replace_path(s);

anno_bg_color         = get_dict_color(plot_info, annobgcolor_name);
anno_text_color       = get_dict_color(plot_info, annotextcolor_name);

anno_font_size        = get_dict_double(plot_info, annofontsize_name);
title_font_size       = get_dict_double(plot_info, titlefontsize_name);
colortable_font_size  = get_dict_double(plot_info, ctablefontsize_name);

plot_size             = get_dict_int(plot_info, plotsize_name);
border_width          = get_dict_int(plot_info, borderwidth_name);

s                     = get_dict_string(plot_info, rawctable_name);
raw_ctable_filename   = replace_path(s);

s                     = get_dict_string(plot_info, objctable_name);
obj_ctable_filename   = replace_path(s);

do_anno               = get_dict_bool(plot_info, doanno_name);


   //
   //  map info
   //

name = mapinfo_name;

e = config.lookup(name);

if ( !e )   {

   mlog << Error
        << "\n\n  " << program_name << ": read_config() -> lookup failed for mapinfo id \"" 
        << name << "\"\n\n";

   exit ( 1 );

}

map_info = e->dict_value();

   //
   //  map data
   //

name = source_name;

e = map_info->lookup(name);

if ( !e )   {

   mlog << Error
        << "\n\n  " << program_name << ": read_config() -> lookup failed for mapfiles id \"" 
        << name << "\"\n\n";

   exit ( 1 );

}

if ( ! (e->is_array()) )  {

   mlog << Error
        << "\n\n  " << program_name << ": read_config() -> id \"" 
        << name << "\" is not an array!\n\n";

   exit ( 1 );

}

sources = e->array_value();

n_sources = sources->n_entries();


   //
   //  done
   //

return;

}


////////////////////////////////////////////////////////////////////////


void do_plot(const char * mode_nc_filename)

{

ModeNcOutputFile mode_in;
ConcatString output_filename;
Cgraph plot;
Ppm image;
Box whole_box, map_box, anno_box;
bool is_object_field = false;
int n_fcst, n_obs, n;

mlog << Debug(1)
     << "Reading MODE file: " << mode_nc_filename << "\n";

   //
   //  open the mode file
   //

if ( ! mode_in.open(mode_nc_filename) )  {

   mlog << Error
        << "\n\n  " << program_name << ": unable to open MODE netcdf file \""
        << mode_nc_filename << "\"\n\n";

   exit ( 1 );

}

if ( mlog.verbosity_level() >= 5 ) mode_in.dump(cout);

output_filename << output_directory << '/' << get_short_name(mode_nc_filename);

output_filename.chomp(".nc");

output_filename << (do_obs ? "_obs" : "_fcst");

switch ( plot_field )  {

   case raw_field:
      output_filename << "_raw.png";
      is_object_field = false;
      break;

   case simple_obj_field:
      output_filename << "_simple.png";
      is_object_field = true;
      n_fcst = mode_in.n_fcst_simple_objs();
      n_obs  = mode_in.n_obs_simple_objs();
      break;

   case composite_obj_field:
      output_filename << "_comp.png";
      is_object_field = true;
      n_fcst = mode_in.n_fcst_comp_objs();
      n_obs  = mode_in.n_obs_comp_objs();
      break;

      default:
      mlog << Error << "\n\n  " << program_name << ": do_plot() -> bad field selected\n\n";
      exit ( 1 );
      break;

}   //  switch

mlog << Debug(1)
     << "Writing image file: " << output_filename << "\n";

   //
   //  read the colortable
   //

if ( ! ctable.read(ctable_filename) )  {

   mlog << Error
        << "\n\n  " << program_name << ": unable to open colortable file \""
        << ctable_filename << "\"\n\n";

   exit ( 1 );

}

   //
   //  get the grid size
   //

Nx = plot_size*(mode_in.nx());
Ny = plot_size*(mode_in.ny());

map_width = Nx;

if ( do_anno )  {

   whole_box.set_llwh(0.0, 0.0, Nx + 2*border_width + ctable_width + ctable_text_width, Ny + anno_height + border_width);

   map_box.set_llwh(border_width, anno_height, Nx, Ny);

   anno_box.set_llwh(0.0, 0.0, Nx + 2*border_width, anno_height);

} else {

   map_box.set_llwh(0.0, 0.0, Nx, Ny);

   whole_box = map_box;

}

   //
   //  open the plot
   //

plot.open(output_filename, whole_box.width(), whole_box.height());

plot.set_family(family);

if ( do_anno )   fill_box(whole_box, anno_bg_color, plot);

   //
   //  get a ppm of the data field
   //

get_data_ppm(mode_in, image);

plot.import(image, map_box.left(), map_box.bottom(), 0.0, 0.0, 1.0);

   //
   //  draw the map
   //

if ( do_anno )  {   //  set up clipping path, if needed

   plot.gsave();

   clip_box(map_box, plot);

}

draw_map(plot, map_box, mode_in.grid());

if ( do_anno )  plot.grestore();

   //
   //  do the annotation, if needed
   //

if ( do_anno )  annotate(mode_in, plot, anno_box, map_box);

   //
   //  colortable
   //

if ( do_anno )  {

   if ( is_object_field )  {

      n = (n_fcst > n_obs) ? n_fcst : n_obs;

      draw_obj_colortable(plot, map_box, n);

   } else {

      draw_raw_colortable(plot, map_box);

   }

}

   //
   //  done
   //

plot.showpage();

mode_in.close();

return;

}


////////////////////////////////////////////////////////////////////////


void usage()

{

mlog << Error << "\n\n   usage:  " << program_name << ": -simple|-composite|-raw  -obs|-fcst -config path -log path -v n mode_nc_file_list\n\n";

exit ( 1 );

return;

}


////////////////////////////////////////////////////////////////////////


void set_config (const StringArray & a)

{

config_filename = a[0];

return;

}


////////////////////////////////////////////////////////////////////////


void set_raw(const StringArray &)

{

plot_field = raw_field;

do_data_rescale = true;

return;

}


////////////////////////////////////////////////////////////////////////


void set_obs(const StringArray &)

{

do_obs = true;

fcst_obs_set = true;

return;

}


////////////////////////////////////////////////////////////////////////


void set_fcst(const StringArray &)

{

do_obs = false;

fcst_obs_set = true;

return;

}


////////////////////////////////////////////////////////////////////////


void set_simple(const StringArray &)

{

plot_field = simple_obj_field;

return;

}


////////////////////////////////////////////////////////////////////////


void set_composite(const StringArray &)

{

plot_field = composite_obj_field;

return;

}


////////////////////////////////////////////////////////////////////////


void set_logfile(const StringArray & a)

{

ConcatString filename;

filename = a[0];

mlog.open_log_file(filename);

return;

}


////////////////////////////////////////////////////////////////////////


void set_verbosity(const StringArray & a)

{

mlog.set_verbosity_level(atoi(a[0]));

return;

}


////////////////////////////////////////////////////////////////////////


void sanity_check()

{

if ( !fcst_obs_set )  {

   mlog << Error
        << "\n\n  " << program_name << ": fcst/obs not set!\n\n";

   exit ( 1 );

}

if ( plot_field == no_plot_field )  {

   mlog << Error
        << "\n\n  " << program_name << ": plot field not set!\n\n";

   exit ( 1 );

}

if ( raw_ctable_filename.empty() )  {

   mlog << Error
        << "\n\n  " << program_name << ": no raw colortable file set!\n\n";

   exit ( 1 );

}

if ( obj_ctable_filename.empty() )  {

   mlog << Error
        << "\n\n  " << program_name << ": no object colortable file set!\n\n";

   exit ( 1 );

}

if ( border_width < 0 )  border_width = 0;

if ( ! do_anno )  border_width = 0;

if ( plot_size < 1 )  plot_size = 1;

if ( plot_field == raw_field )  ctable_filename = raw_ctable_filename;
else                            ctable_filename = obj_ctable_filename;

return;

}


////////////////////////////////////////////////////////////////////////


void get_data_ppm(ModeNcOutputFile & mode_in, Ppm & image)

{

int k;
int x, y, mx, my;
double value;
double min_value, max_value;
Color color;


if ( do_data_rescale )  {

   mode_in.get_fcst_raw_range(min_value, max_value);

   M = 1.0/(max_value - min_value);

   B = -M*min_value;

}


image.set_size_xy(Nx, Ny);

for (x=0; x<(image.nx()); ++x)  {

   mx = x/plot_size;

   for (y=0; y<(image.ny()); ++y)  {

      my = y/plot_size;

      if ( plot_field == raw_field )  {

         if ( do_obs )  value = mode_in.obs_raw  (mx, my);
         else           value = mode_in.fcst_raw (mx, my);

         if ( do_data_rescale && (value > -9000.0) )  value = M*value + B;

         color = ctable.interp(value);

      } else {   //  object

         if ( plot_field == composite_obj_field )  {

            if ( do_obs )  k = mode_in.obs_comp_id  (mx, my);
            else           k = mode_in.fcst_comp_id (mx, my);

         } else {   //  simple

            if ( do_obs )  k = mode_in.obs_obj_id  (mx, my);
            else           k = mode_in.fcst_obj_id (mx, my);

         }

         value = (double) k;

              if ( k ==    -1 )  color = unmatched_color;
         else if ( k == -9999 )  color = white;
         else                    color = ctable.nearest(value);

      }

      image.putxy(color, x, y);

   }

}


   //
   //  done
   //

return;

}


////////////////////////////////////////////////////////////////////////


void fill_box(const Box & b, const Color & c, Cgraph & plot)

{

plot.gsave();

   plot.set_color(c);

   plot.newpath();

   plot.moveto(b.left(),  b.bottom());
   plot.lineto(b.right(), b.bottom());
   plot.lineto(b.right(), b.top());
   plot.lineto(b.left(),  b.top());

   plot.closepath();

   plot.fill();

plot.grestore();

return;

}


////////////////////////////////////////////////////////////////////////


void clip_box(const Box & b, Cgraph & plot)

{

plot.newpath();

plot.moveto(b.left(),  b.bottom());
plot.lineto(b.right(), b.bottom());
plot.lineto(b.right(), b.top());
plot.lineto(b.left(),  b.top());

plot.closepath();

plot.clip();

return;

}


////////////////////////////////////////////////////////////////////////


void draw_map(Cgraph & plot, const Box & map_box, const Grid & grid)

{

int j;
Dictionary & s = *(sources);
const DictionaryEntry * e = 0;
Dictionary * dict = 0;


for (j=0; j<n_sources; ++j)  {

   e = s[j];

   if ( ! (e->is_dictionary()) )  {

      mlog << Error
           << "\n\n  " << program_name << ": draw_map() -> non-dictionary found "
           << "in the source array in the config file!\n\n";

      exit ( 1 );

   }

   dict = e->dict_value();

   draw_mapfile(plot, map_box, grid, dict);

}


   //
   //  done
   //

return;

}


////////////////////////////////////////////////////////////////////////


void draw_mapfile(Cgraph & plot, const Box & map_box, const Grid & grid, Dictionary * dict)

{

ifstream in;
MapRegion r;
double map_linewidth = 1.0;
ConcatString s;
ConcatString map_filename;
Color map_color;


s = get_dict_string (dict, filename_name);

map_filename  = replace_path(s);

map_linewidth = get_dict_double (dict, linewidth_name);

map_color     = get_dict_color  (dict, linecolor_name);

in.open(map_filename);

if ( !in )  {

   mlog << Error
        << "\n\n  " << program_name << ": draw_mapfile() -> unable to open map data file \""
        << map_filename << "\"\n\n";

   exit ( 1 );

}

plot.gsave();

   plot.setlinewidth(map_linewidth);

   plot.set_color(map_color);

   while ( in >> r )  {

      draw_region(plot, grid, map_box, r);

   }

plot.grestore();



   //
   //  done
   //

in.close();

return;

}


////////////////////////////////////////////////////////////////////////


void draw_region(Cgraph & plot, const Grid & grid, const Box & map_box, const MapRegion & r)

{

int j;
double x_grid, y_grid;
double x_page_1, y_page_1, x_page_2, y_page_2;
const double width = map_box.width();
const double x_ll = map_box.left();
const double y_ll = map_box.bottom();

plot.newpath();

grid.latlon_to_xy(r.lat[0], r.lon[0], x_grid, y_grid);

x_page_1 = x_grid*plot_size;
y_page_1 = y_grid*plot_size;

x_page_1 += x_ll;
y_page_1 += y_ll;

plot.moveto(x_page_1, y_page_1);

for (j=1; j<(r.n_points); ++j)  {   //  j starts at one, here

   grid.latlon_to_xy(r.lat[j], r.lon[j], x_grid, y_grid);

   x_page_2 = x_grid*plot_size;
   y_page_2 = y_grid*plot_size;

   x_page_2 += x_ll;
   y_page_2 += y_ll;

      //
      // Check for regions which overlap the edge of the grid
      // Finish the previous path and begin a new one
      //

   if ( fabs(x_page_2 - x_page_1) > 0.90*width )  {

      plot.stroke();

      plot.newpath();

      plot.moveto(x_page_2, y_page_2);

   } else  plot.lineto(x_page_2, y_page_2);

   x_page_1 = x_page_2;   
   y_page_1 = y_page_2;   

}   //  for j

plot.stroke();

return;

}


////////////////////////////////////////////////////////////////////////


ConcatString get_dict_string (Dictionary * dict, const char * id)

{

ConcatString s;
const DictionaryEntry * e = 0;


e = dict->lookup(id);

if ( !e )  {

   mlog << Error
        << "\n\n  " << program_name << ": get_dict_string() -> lookup failed for \""
        << id << "\"\n\n";

   exit ( 1 );

}

if ( e->type() != StringType )  {

   mlog << Error
        << "\n\n  " << program_name << ": get_dict_string() -> entry \""
        << id << "\" is not a string\n\n";

   exit ( 1 );

}

s = *(e->string_value());

return ( s );

}


////////////////////////////////////////////////////////////////////////


bool get_dict_bool(Dictionary * dict, const char * id)

{

bool tf = false;
const DictionaryEntry * e = 0;


e = dict->lookup(id);

if ( !e )  {

   mlog << Error
        << "\n\n  " << program_name << ": get_dict_bool() -> lookup failed for \""
        << id << "\"\n\n";

   exit ( 1 );

}

tf = e->b_value();

return ( tf );

}


////////////////////////////////////////////////////////////////////////


int get_dict_int(Dictionary * dict, const char * id)

{

int k = 0;
const DictionaryEntry * e = 0;


e = dict->lookup(id);

if ( !e )  {

   mlog << Error
        << "\n\n  " << program_name << ": get_dict_int() -> lookup failed for \""
        << id << "\"\n\n";

   exit ( 1 );

}

k = e->i_value();

return ( k );

}


////////////////////////////////////////////////////////////////////////


double get_dict_double(Dictionary * dict, const char * id)

{

double t = 0.0;
const DictionaryEntry * e = 0;


e = dict->lookup(id);

if ( !e )  {

   mlog << Error
        << "\n\n  " << program_name << ": get_dict_double() -> lookup failed for \""
        << id << "\"\n\n";

   exit ( 1 );

}

     if ( e->type() == IntegerType )  t = (double) (e->i_value());
else if ( e->type() == FloatType   )  t = (double) (e->d_value());
else{

   mlog << Error 
        << "\n\n  " << program_name << ": get_dict_double() -> bad type for id \""
        << id << "\" ... expecting number\n\n";

   exit ( 1 );

}


return ( t );

}



////////////////////////////////////////////////////////////////////////


Color get_dict_color(Dictionary * dict, const char * id)

{

int j;
int rgb[3];
Color c;
const DictionaryEntry * e = 0;


e = dict->lookup(id);

if ( !e )  {

   mlog << Error
        << "\n\n  " << program_name << ": get_dict_color() -> lookup failed for color \""
        << id << "\n\n";

   exit ( 1 );

}

const Dictionary & array = *(e->array_value());

for (j=0; j<3; ++j)  {

   e = array[j];

   rgb[j] = e->i_value();

}

c.set_rgb(rgb[0], rgb[1], rgb[2]);

return ( c );

}


////////////////////////////////////////////////////////////////////////


void time_string(int seconds, char * out, const int len)

{

int h, m, s;

h = seconds/3600;
m = (seconds%3600)/60;
s = seconds%60;

     if ( (m == 0) && (s == 0) )  snprintf(out, len, "%02dh",          h);
else if ( s == 0 )                snprintf(out, len, "%02d:%02d",      h, m);
else                              snprintf(out, len, "%02d:%02d:%02d", h, m, s);

return;

}


////////////////////////////////////////////////////////////////////////


void annotate(const ModeNcOutputFile & mode_in, Cgraph & plot, const Box & anno_box, const Box & map_box)

{

ConcatString title;
ConcatString fcst_obs, raw_obj;
ConcatString s;
char junk[256], ts[256];
int month, day, year, hour, minute, second;
int lead_seconds;
double scale, delta;
double htab1, htab2;
double vtab1, vtab2;
const double scale_tol = 0.05;

htab1 = border_width + 10.0;

   //
   //  title
   //

if ( do_obs )  fcst_obs = "Observed";
else           fcst_obs = "Forecast";

switch ( plot_field )  {

   case raw_field:
      raw_obj = "Raw";
      break;

   case simple_obj_field:
      raw_obj = "Simple Object";
      break;

   case composite_obj_field:
      raw_obj = "Composite Object";
      break;

   default:
      mlog << Error << "\n\n  " << program_name << ": annotate() -> bad plot field\n\n";
      exit ( 1 );
      break;

}

plot.set_color(anno_text_color);

title << cs_erase
      << fcst_obs << ' '
      << raw_obj  << ' '
      << "Field";

plot.bold(title_font_size);

scale = calc_text_scale(plot, map_width - 10.0, title);

if ( fabs(scale - 1.0) > scale_tol )  plot.bold(scale*title_font_size);

plot.write_centered_text(1, 1, 0.5*(plot.page_width()), anno_height - plot.current_font_size() - 10.0, 0.5, 0.0, title);


plot.roman(anno_font_size);

s = mode_in.short_filename();

scale = calc_text_scale(plot, map_width - 10.0, s);

if ( fabs(scale - 1.0) > scale_tol )  plot.bold(scale*anno_font_size);

plot.write_centered_text(1, 1, 0.5*(plot.page_width()), 25.0, 0.5, 0.0, s);


vtab1 = anno_height + 0.5*anno_font_size;
vtab2 = vtab1 + 1.5*anno_font_size;

delta = 0.0;


plot.bold(anno_font_size);
plot.set_color(black);
plot.write_centered_text(1, 1, htab1, vtab1, 0.0, 0.0, "Lead");

if ( plot.LastTextWidth > delta )  delta = plot.LastTextWidth;

plot.bold(anno_font_size);
plot.set_color(black);
plot.write_centered_text(1, 1, htab1, vtab2, 0.0, 0.0, "Valid");

if ( plot.LastTextWidth > delta )  delta = plot.LastTextWidth;

htab2 = htab1 + delta + 20.0;


   //
   // lead time
   //

plot.roman(anno_font_size);
plot.set_color(anno_text_color);

lead_seconds = (int) (mode_in.valid_time() - mode_in.init_time());

time_string(lead_seconds, ts, sizeof(ts));

plot.write_centered_text(1, 1, htab2, vtab1, 0.0, 0.0, ts);

   //
   //  valid time
   //

plot.roman(anno_font_size);
plot.set_color(anno_text_color);

unix_to_mdyhms(mode_in.valid_time(), month, day, year, hour, minute, second);

time_string((int) (mode_in.valid_time()%86400), ts, sizeof(ts));

snprintf(junk, sizeof(junk), "%s %d, %d  %s", short_month_name[month], day, year, ts);

plot.write_centered_text(1, 1, htab2, vtab2, 0.0, 0.0, junk);


   //
   //  done
   //

return;

}


////////////////////////////////////////////////////////////////////////


double calc_text_scale(Cgraph & plot, const double target_width, const char * text)

{

if ( empty(text) )  {

   mlog << Error
        << "\n\n  " << program_name << ": calc_text_scale() -> empty text!\n\n";

   exit ( 1 );

}

double s;

plot.write_centered_text(1, 1, 0.0, 0.0, 0.0, 0.0, text, false);

s = target_width/(plot.LastTextWidth);

if ( s > 1.0 )  s = 1.0;

return ( s );

}


////////////////////////////////////////////////////////////////////////


void draw_obj_colortable(Cgraph & plot, const Box & map_box, const int Nobjs)

{

int j, k;
CtableEntry e;
double x, y;
const double dy = (map_box.height())/Nobjs;
Box b;
char junk[256];


x = map_box.right();

y = map_box.bottom();

plot.roman(colortable_font_size);

for (j=0; j<Nobjs; ++j)  {

   b.set_llwh(x, y, ctable_width, dy);

   e = ctable[j];

   fill_box(b, e.color(), plot);

   k = nint(e.value_low());

   snprintf(junk, sizeof(junk), "%d", k);

   plot.write_centered_text(2, 1, x + ctable_width + 2.0, y + 0.5*dy, 0.0, 0.5, junk);

   y += dy;

}



plot.setgray(0.0);

plot.setlinewidth(0.2);

y = map_box.bottom();

for (j=0; j<=Nobjs; ++j)  {

   plot.line(x, y, x + ctable_width, y);

   y += dy;

}

   //
   //  done
   //

return;

}


////////////////////////////////////////////////////////////////////////


void draw_raw_colortable(Cgraph & plot, const Box & map_box)

{

int j;
CtableEntry e;
double x, y;
const double dy = (map_box.height())/(ctable.n_entries());
Box b;
char junk[256];
double t;
const int N = ctable.n_entries();



x = map_box.right();

y = map_box.bottom();

plot.roman(colortable_font_size);

for (j=0; j<N; ++j)  {

   b.set_llwh(x, y, ctable_width, dy);

   e = ctable[j];

   fill_box(b, e.color(), plot);

   t = 0.5*(e.value_low() + e.value_high());

   t = (t - B)/M;

   snprintf(junk, sizeof(junk), "%.2f", t);

   plot.write_centered_text(2, 1, x + ctable_width + 2.0, y + 0.5*dy, 0.0, 0.5, junk);

   y += dy;

}


plot.setgray(0.0);

plot.setlinewidth(0.2);

y = map_box.bottom();

for (j=0; j<=N; ++j)  {

   plot.line(x, y, x + ctable_width, y);

   y += dy;

}



   //
   //  done
   //

return;

}


////////////////////////////////////////////////////////////////////////


