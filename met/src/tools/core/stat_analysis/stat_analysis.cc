// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2015
// ** University Corporation for Atmospheric Research (UCAR)
// ** National Center for Atmospheric Research (NCAR)
// ** Research Applications Lab (RAL)
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

////////////////////////////////////////////////////////////////////////
//
//   Filename:   stat_analysis.cc
//
//   Description:
//      Based on user specified options, this tool performs a variety
//      of analysis jobs on the STAT output of the Grid-Stat,
//      Point-Stat, and Wavelet-Stat tools.
//
//   Mod#   Date      Name            Description
//   ----   ----      ----            -----------
//   000    09/05/07  Halley Gotway   New
//   001    07/01/08  Halley Gotway   Add the rank_corr_flag to the
//                    config file to disable computing rank
//                    correlations.
//   002    06/21/10  Halley Gotway   Add the vif_flag to correct normal
//                    CI's for time series aggregations.
//   003    08/15/11  Oldenburg       Fix a bug related to parsing the
//                    config file alpha value list
//   004    08/16/11  Halley Gotway   Reimplementation of GO Index job
//                    with addition of generalized Skill Score Index
//   005    10/26/11  Holmes          Added use of command line class to
//                                    parse the command line arguments.
//   006    11/14/11  Holmes          Added code to enable reading of
//                                    multiple config files.
//   007    01/20/12  Halley Gotway   Modify logic so that command line
//                    job options override config file job options.
//   008    05/03/12  Halley Gotway   Switch to using vx_config library.
//   009    05/21/12  Halley Gotway   Add support of -fcst_valid_hour
//                    and -obs_valid_hour job command options.
//
////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdio>
#include <dirent.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <cmath>

#include "vx_log.h"

#include "stat_analysis.h"
#include "parse_stat_line.h"
#include "aggr_stat_line.h"
#include "stat_analysis_job.h"

////////////////////////////////////////////////////////////////////////

static void parse_command_line(int &argc, char **argv);
static void sanity_check();
static void usage();
static void set_lookin_path(const StringArray &);
static void set_out_filename(const StringArray &);
static void set_tmp_dir(const StringArray &);
static void set_logfile(const StringArray &);
static void set_verbosity_level(const StringArray &);
static void set_config_file(const StringArray &);
static void process_search_dirs();
static void process_stat_file(const char *, const STATAnalysisJob &,
                              int &, int &);
static void process_job(const char *, int);
static void clean_up();

//
// Routines for command-line switches
//
static void set_config(const char *);
static void set_search_dir(const char *);
static void set_out_file(const char *);
static void set_verbosity(int);

////////////////////////////////////////////////////////////////////////

int main(int argc, char * argv []) {
   int i;
   StringArray jobs_sa;
   ConcatString default_config_file;

   //
   // Set handler to be called for memory allocation error
   //
   set_new_handler(oom);

   //
   // Parse the command line
   //
   parse_command_line(argc, argv);

   //
   // If a config file was specified, set up the default job using the
   // config file.
   //
   if(config_file.length() > 0) {

      //
      // Create the default config file name
      //
      default_config_file = replace_path(default_config_filename);

      //
      // List the config files
      //
      mlog << Debug(1)
           << "Default Config File: " << default_config_file << "\n"
           << "User Config File: "    << config_file << "\n";

      //
      // Read config file constants, the default config file,
      // and then the user config file.
      //
      conf.read(replace_path(config_const_filename));
      conf.read(default_config_file);
      conf.read(config_file);
      
      //
      // Sanity check the command line and config file options
      //
      sanity_check();

      //
      // Setup the default job using the config file options
      //
      default_job.clear();
      set_job_from_config(conf, default_job);
      default_job.set_precision(conf.output_precision());

      //
      // Write out the default job
      //
      mlog << Debug(4)
           << "Default Job from the config file: \""
           << default_job.get_jobstring() << "\"\n";
   }

   //
   // Enclose within a try block to catch any run time errors, and
   // delete the temp file before exiting.
   //
   try {

      //
      // Amend the default job using command line options
      //
      mlog << Debug(4)
           << "Amending default job with command line options: \""
           << command_line_job_options << "\"\n";
      default_job.parse_job_command(command_line_job_options);

      //
      // Process the STAT files found in the search directories.
      //
      process_search_dirs();

      //
      // If a config file was specified, process the jobs.
      //
      if(config_file.length() != 0) {

         jobs_sa = conf.lookup_string_array(conf_key_jobs);

         for(i=0; i<jobs_sa.n_elements(); i++) {
            process_job(jobs_sa[i], i+1);
         }
      }
      //
      // Otherwise, process the job specified on the command line.
      //
      else {
         process_job(command_line_job_options, 1);
      }

   }
   catch(int j) { // Catch an error

      mlog << Error << "\nmain() -> "
           << "encountered an error value of " << j
           << ".  Calling clean_up() and usage() before exiting.\n\n";

      clean_up();
      usage();
   }

   //
   // Deallocate memory and clean up
   //
   clean_up();

   return(0);
}

////////////////////////////////////////////////////////////////////////

void parse_command_line(int &argc, char **argv) {
   CommandLine cline;
   int i;

   //
   // check for zero arguments
   //
   if (argc == 1)
      usage();

   config_file.erase();

   //
   // parse the command line into tokens
   //
   cline.set(argc, argv);

   //
   // Allow for unrecognized command line switches.
   // This must be called after set above since set calls
   // clear which would reset this to false.
   // This allows us to be able to handle single jobs on
   // the command line.
   //
   cline.allow_unrecognized_switches();

   //
   // set the usage function
   //
   cline.set_usage(usage);

   //
   // add the options function calls
   //
   cline.add(set_lookin_path, "-lookin", -1);
   cline.add(set_out_filename, "-out", 1);
   cline.add(set_tmp_dir, "-tmp_dir", 1);
   cline.add(set_logfile, "-log", 1);
   cline.add(set_verbosity_level, "-v", 1);
   cline.add(set_config_file, "-config", 1);

   //
   // parse the command line
   //
   cline.parse();

   //
   // Parse any remaining command line arguments into a job string
   //
   command_line_job_options.erase();

   for(i=0; i<cline.n(); i++) {
     
      //
      // store current argument
      //
      command_line_job_options << cline[i];

      //
      // add a space between arguments, except for the last argument
      //
      if (i + 1 != cline.n()) command_line_job_options << ' ';

   } // end for

   //
   // Check for at least one search file or directory
   //
   if(search_dirs.n_elements() == 0) {
      mlog << Error << "\nparse_command_line() -> "
           << "no STAT search files or directories specified!\n\n";

      exit(1);
   }

   return;
}

////////////////////////////////////////////////////////////////////////

void sanity_check() {
   unixtime ut_beg, ut_end;

   //
   // Conf: version
   //
   parse_conf_version(&conf);

   //
   // Check for fcst_valid_beg > fcst_valid_end
   //
   ut_beg = conf.lookup_unixtime(conf_key_fcst_valid_beg, false);
   ut_end = conf.lookup_unixtime(conf_key_fcst_valid_end, false);

   if((ut_beg > 0) && (ut_end > 0) && (ut_beg > ut_end)) {
      mlog << Error << "\nsanity_check() -> "
           << "\"" << conf_key_fcst_valid_beg << "\" is after \""
           << conf_key_fcst_valid_end << "\": "
           << unix_to_yyyymmdd_hhmmss(ut_beg) << " > "
           << unix_to_yyyymmdd_hhmmss(ut_end) << "!\n\n";
      exit(1);
   }

   //
   // Check for obs_valid_beg > obs_valid_end
   //
   ut_beg = conf.lookup_unixtime(conf_key_obs_valid_beg, false);
   ut_end = conf.lookup_unixtime(conf_key_obs_valid_end, false);

   if((ut_beg > 0) && (ut_end > 0) && (ut_beg > ut_end)) {
      mlog << Error << "\nsanity_check() -> "
           << "\"" << conf_key_obs_valid_beg << "\" is after \""
           << conf_key_obs_valid_end << "\": "
           << unix_to_yyyymmdd_hhmmss(ut_beg) << " > "
           << unix_to_yyyymmdd_hhmmss(ut_end) << "!\n\n";
      exit(1);
   }

   //
   // Check for fcst_init_beg > fcst_init_end
   //
   ut_beg = conf.lookup_unixtime(conf_key_fcst_init_beg, false);
   ut_end = conf.lookup_unixtime(conf_key_fcst_init_end, false);

   if((ut_beg > 0) && (ut_end > 0) && (ut_beg > ut_end)) {
      mlog << Error << "\nsanity_check() -> "
           << "\"" << conf_key_fcst_init_beg << "\" is after \""
           << conf_key_fcst_init_end << "\": "
           << unix_to_yyyymmdd_hhmmss(ut_beg) << " > "
           << unix_to_yyyymmdd_hhmmss(ut_end) << "!\n\n";
      exit(1);
   }

   //
   // Check for obs_init_beg > obs_init_end
   //
   ut_beg = conf.lookup_unixtime(conf_key_obs_init_beg, false);
   ut_end = conf.lookup_unixtime(conf_key_obs_init_end, false);

   if((ut_beg > 0) && (ut_end > 0) && (ut_beg > ut_end)) {
      mlog << Error << "\nsanity_check() -> "
           << "\"" << conf_key_obs_init_beg << "\" is after \""
           << conf_key_obs_init_end << "\": "
           << unix_to_yyyymmdd_hhmmss(ut_beg) << " > "
           << unix_to_yyyymmdd_hhmmss(ut_end) << "!\n\n";
      exit(1);
   }

   return;
}

////////////////////////////////////////////////////////////////////////

void set_config(const char *path) {

   config_file = path;

   return;
}

////////////////////////////////////////////////////////////////////////

void set_search_dir(const char *path) {

   search_dirs.add(path);

   return;
}

////////////////////////////////////////////////////////////////////////

void set_out_file(const char *path) {

   out_file = path;

   //
   // Create an output file and set the sa_out ofstream to it.
   //
   sa_out = new ofstream;
   sa_out->open(out_file);

   if(!(*sa_out)) {
      mlog << Error << "\nset_out_file()-> "
           << "can't open the output file \"" << out_file
           << "\" for writing!\n\n";
      exit(1);
   }

   mlog << Debug(1) << "Creating STAT-Analysis output file \""
        << out_file << "\"\n";

   return;
}

////////////////////////////////////////////////////////////////////////

void set_verbosity(int i) {

   mlog.set_verbosity_level(i);

   return;
}

////////////////////////////////////////////////////////////////////////

void process_search_dirs() {
   int n, i, j, max_len, n_read, n_keep;
   MetConfig go_conf;
   STATAnalysisJob go_job;
   StringArray suffix_list;

   //
   // Initialize
   //
   n_read = n_keep = 0;

   //
   // Search for files ending in .stat
   //
   suffix_list.add(stat_file_ext);

   //
   // Get the list of stat files in the search directories
   //
   files = get_filenames(search_dirs, suffix_list);

   n = files.n_elements();

   if(n == 0) {
      mlog << Error << "\nprocess_search_dirs() -> "
           << "no STAT files found in the directories specified!\n\n";

      throw(1);
   }

   //
   // Apply the GO Index filtering criteria for a command line job.
   //
   if(default_job.job_type == stat_job_go_index) {
      
      //
      // Read config file constants followed by the config file which
      // defines the GO Index.
      //
      go_conf.read(replace_path(config_const_filename));      
      go_conf.read(replace_path(go_index_config_file));

      //
      // Parse the contents of the GO Index config file into the
      // search job.
      //
      set_job_from_config(go_conf, go_job);
      
      //
      // Amend the default job with GO Index filtering criteria.
      //
      default_job.parse_job_command(go_job.get_jobstring());

   } // end if go_index

   //
   // Open up the temp file for storing the intermediate STAT line data
   //

   //
   // If the tmp_dir has not already been set on the command line,
   // use the config file setting or default setting.
   //
   if(tmp_dir.length() == 0) {
      if(config_file.length() != 0) tmp_dir = parse_conf_tmp_dir(&conf);
      else                          tmp_dir = default_tmp_dir;
   }

   //
   // Build the temp file name
   //
   tmp_file << tmp_dir << "/" << "tmp_stat_analysis";
   tmp_path = make_temp_file_name(tmp_file, '\0');

   //
   // Open the temp file
   //
   tmp_out.open(tmp_path);
   if(!tmp_out) {
      mlog << Error << "\nprocess_search_dirs() -> "
           << "can't open the temporary file \"" << tmp_path
           << "\" for writing!\n\n";

      throw(1);
   }

   //
   // Go through each input file
   //
   max_len = 0;

   for(i=0; i<n; i++)  {
      j = strlen(files[i]);
      if(j > max_len) max_len = j;
   }
   max_len += 3;

   for(i=0; i<n; i++) {
      if(mlog.verbosity_level() > 2) {

         mlog << Debug(3) << "Processing STAT file \"" << files[i] << "\" ";

         for(j=strlen(files[i]); j<max_len; j++) mlog << '.';

         mlog << " " << i+1 << " of " << n << "\n";

         if((j%5) == 4) mlog << '\n';

      }

      process_stat_file(files[i], default_job, n_read, n_keep);
   }

   mlog << Debug(2) << "STAT Lines read     = " << n_read << "\n";
   mlog << Debug(2) << "STAT Lines retained = " << n_keep << "\n";

   tmp_out.close();

   return;
}

////////////////////////////////////////////////////////////////////////

void process_stat_file(const char *filename, const STATAnalysisJob &j,
                       int &n_read, int &n_keep) {
   LineDataFile f;
   STATLine line;

   if(!(f.open(filename))) {
      mlog << Error << "\nprocess_stat_file() -> "
           << "unable to open input stat file \""
           << filename << "\"\n\n";

      throw(1);
   }

   while(f >> line) {

      //
      // Continue if the line is not a valid STAT line.
      //
      if(line.type() == no_stat_line_type) continue;

      if(!line.is_header()) n_read++;

      //
      // Pass header lines through to the output
      //
      if(line.is_header() || j.is_keeper(line)) {

         if(!line.is_header()) n_keep++;

         tmp_out << line;
      }
   } // end while

   f.close();

   return;
}

////////////////////////////////////////////////////////////////////////

void process_job(const char * jobstring, int n_job) {
   STATAnalysisJob job, go_job;
   ConcatString full_jobstring;
   MetConfig go_conf;

   //
   // Initialize to the default job
   //
   mlog << Debug(4)
        << "\nInitializing Job " << n_job << " to default job: \""
        << default_job.get_jobstring() << "\"\n";
   job = default_job;

   //
   // Parse the job command line options
   //
   mlog << Debug(4)
        << "\nAmending Job " << n_job << " with options: \""
        << jobstring << "\"\n";
   job.parse_job_command(jobstring);

   //
   // Special processing for the GO Index job.
   //
   if(job.job_type == stat_job_go_index) {

      //
      // Read config file constants followed by the config file which
      // defines the GO Index.
      //
      go_conf.read(replace_path(config_const_filename));
      go_conf.read(replace_path(go_index_config_file));

      //
      // Parse the contents of the GO Index config file into the
      // search job.
      //
      set_job_from_config(go_conf, go_job);

      //
      // Amend the current job with GO Index filtering criteria
      //
      mlog << Debug(4)
           << "\nAmending Job " << n_job << " with GO Index configuration file: "
           << replace_path(go_index_config_file) << "\n";
      job.parse_job_command(go_job.get_jobstring());

   } // end if go_index
   
   //
   // Amend the current job using any command line options
   //
   mlog << Debug(4)
        << "\nAmending Job " << n_job << " with command line options: \""
        << command_line_job_options << "\"\n";
   job.parse_job_command(command_line_job_options);
   
   //
   // Get the full jobstring
   //
   full_jobstring = job.get_jobstring();

   //
   // Do the job
   //
   do_job(full_jobstring, job, n_job, tmp_dir, tmp_path, sa_out);

   return;
}

////////////////////////////////////////////////////////////////////////

void clean_up() {

   //
   // Delete the temp file if it exists
   //
   if(tmp_path) remove_temp_file(tmp_path);

   return;
}

////////////////////////////////////////////////////////////////////////

void usage() {

   cout << "\n*** Model Evaluation Tools (MET" << met_version
        << ") ***\n\n"

        << "Usage: " << program_name << "\n"
        << "\t-lookin path\n"
        << "\t[-out filename]\n"
        << "\t[-tmp_dir path]\n"
        << "\t[-log file]\n"
        << "\t[-v level]\n"
        << "\t[-config config_file] | [JOB COMMAND LINE]\n\n"

        << "\twhere\t\"-lookin path\" specifies a STAT file or "
        << "top-level directory containing STAT files.  It allows the "
        << "use of wildcards (at least one required).\n"

        << "\t\t\"-out filename\" specifies a file name to which "
        << "output should be written rather than the screen (optional).\n"

        << "\t\t\"-tmp_dir path\" specifies the directory into which "
        << "temporary files should be written (optional).\n"

        << "\t\t\"-log file\" outputs log messages to the specified "
        << "file (optional).\n"

        << "\t\t\"-v level\" overrides the default level of logging ("
        << mlog.verbosity_level() << ") (optional).\n"

        << "\t\t\"-config config_file\" specifies a STATAnalysis "
        << "config file containing STATAnalysis jobs to be run.\n"

        << "\t\t\"JOB COMMAND LINE\" specifies all the arguments "
        << "necessary to perform a single STATAnalysis job.\n\n"

        << "\tNOTE: Refer to a STATAnalysis config file for details "
        << "on how to specify the JOB COMMAND LINE.\n\n"

        << flush;

   exit (1);
}

////////////////////////////////////////////////////////////////////////

void set_lookin_path(const StringArray & a)
{
   for (int i = 0; i < a.n_elements(); i++)
      set_search_dir(a[i]);
}

////////////////////////////////////////////////////////////////////////

void set_out_filename(const StringArray & a)
{
   set_out_file(a[0]);
}

////////////////////////////////////////////////////////////////////////

void set_tmp_dir(const StringArray & a)
{
   tmp_dir << a[0];
   if(opendir(tmp_dir) == NULL ) {
      mlog << Error << "\nparse_command_line() -> "
           << "Cannot access the tmp_dir temporary directory: "
           << tmp_dir << "\n\n";
      exit(1);
   }
}

////////////////////////////////////////////////////////////////////////

void set_logfile(const StringArray & a)
{
   ConcatString filename;

   filename = a[0];

   mlog.open_log_file(filename);
}

////////////////////////////////////////////////////////////////////////

void set_verbosity_level(const StringArray & a)
{
   set_verbosity(atoi(a[0]));
}

////////////////////////////////////////////////////////////////////////

void set_config_file(const StringArray & a)
{
   set_config(a[0]);
}

////////////////////////////////////////////////////////////////////////


