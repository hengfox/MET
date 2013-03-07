// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2012
// ** University Corporation for Atmospheric Research (UCAR)
// ** National Center for Atmospheric Research (NCAR)
// ** Research Applications Lab (RAL)
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

////////////////////////////////////////////////////////////////////////
//
//   Filename:   stat_analysis_job.h
//
//   Description:
//
//   Mod#   Date      Name            Description
//   ----   ----      ----            -----------
//   000    12/17/08  Halley Gotway   New
//   001    08/16/11  Halley Gotway   Reimplementation of GO Index job
//                    with addition of generalized Skill Score Index
//   002    05/03/12  Halley Gotway   Switch to using vx_config library.
//   003    02/04/13  Halley Gotway   Add -by case option.
//   004    03/07/13  Halley Gotway   Add aggregate SSVAR lines.
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

#ifndef  __STAT_ANALYSIS_JOB_H__
#define  __STAT_ANALYSIS_JOB_H__

////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cmath>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "vx_analysis_util.h"
#include "vx_statistics.h"
#include "vx_stat_out.h"
#include "aggr_stat_line.h"

////////////////////////////////////////////////////////////////////////

extern void set_job_from_config(MetConfig &, STATAnalysisJob &);

extern void do_job(const ConcatString &, STATAnalysisJob &, int,
               const ConcatString &, const ConcatString &,
               ofstream *);

extern void do_job_filter(const ConcatString &, LineDataFile &,
               STATAnalysisJob &, int &, int &, ofstream *);

extern void do_job_summary(const ConcatString &, LineDataFile &,
               STATAnalysisJob &, int &, int &, ofstream *);

extern void do_job_aggr(const ConcatString &, LineDataFile &,
               STATAnalysisJob &, int &, int &, ofstream *);

extern void do_job_aggr_stat(const ConcatString &, LineDataFile &,
               STATAnalysisJob &, int &, int &, ofstream *,
               const ConcatString &);

extern void do_job_go_index(const ConcatString &, LineDataFile &,
               STATAnalysisJob &, int &, int &, ofstream *);

extern void do_job_ss_index(const ConcatString &, LineDataFile &,
               STATAnalysisJob &, int &, int &, ofstream *);

////////////////////////////////////////////////////////////////////////

extern void write_job_aggr_hdr(STATAnalysisJob &,
               int, int, AsciiTable &);

extern void write_job_aggr_ctc(STATAnalysisJob &, STATLineType,
               map<ConcatString, AggrCTCInfo> &, AsciiTable &);
               
extern void write_job_aggr_mctc(STATAnalysisJob &, STATLineType,
               map<ConcatString, AggrMCTCInfo> &, AsciiTable &);

extern void write_job_aggr_pct(STATAnalysisJob &, STATLineType,
               map<ConcatString, AggrPCTInfo> &, AsciiTable &);

extern void write_job_aggr_psum(STATAnalysisJob &, STATLineType,
               map<ConcatString, AggrPSumInfo> &, AsciiTable &);

extern void write_job_aggr_wind(STATAnalysisJob &, STATLineType,
               map<ConcatString, AggrWindInfo> &, AsciiTable &);

extern void write_job_aggr_rhist(STATAnalysisJob &, STATLineType,
               map<ConcatString, AggrRHISTInfo> &, AsciiTable &);

extern void write_job_aggr_ssvar(STATAnalysisJob &, STATLineType,
               map<ConcatString, AggrSSVARInfo> &, AsciiTable &);

extern void write_job_aggr_orank(STATAnalysisJob &, STATLineType,
               map<ConcatString, AggrORANKInfo> &, AsciiTable &);

extern void write_job_aggr_isc(STATAnalysisJob &, STATLineType,
               map<ConcatString, AggrISCInfo> &, AsciiTable &);

extern void write_job_aggr_mpr(STATAnalysisJob &, STATLineType,
               map<ConcatString, AggrMPRInfo> &, AsciiTable &,
               const char *);

////////////////////////////////////////////////////////////////////////

extern void setup_table    (AsciiTable &);
extern void write_table    (AsciiTable &,  ofstream *);
extern void write_jobstring(const ConcatString &, ofstream *);
extern void write_line     (const ConcatString &, ofstream *);

////////////////////////////////////////////////////////////////////////

extern double compute_ss_index(const ConcatString &, LineDataFile &,
                 STATAnalysisJob &, int &, int &);

////////////////////////////////////////////////////////////////////////

#endif   //  __STAT_ANALYSIS_JOB_H__

////////////////////////////////////////////////////////////////////////
