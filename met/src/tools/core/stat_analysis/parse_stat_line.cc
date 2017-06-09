// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2017
// ** University Corporation for Atmospheric Research (UCAR)
// ** National Center for Atmospheric Research (NCAR)
// ** Research Applications Lab (RAL)
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

////////////////////////////////////////////////////////////////////////
//
//   Filename:   parse_stat_line.h
//
//   Description:
//
//   Mod#   Date      Name            Description
//   ----   ----      ----            -----------
//   000    12/17/08  Halley Gotway   New
//   001    05/24/10  Halley Gotway   Add parse_rhist_line and
//                    parse_orank_line.
//   002    06/09/10  Halley Gotway   Add parse_mctc_ctable.
//   003    03/07/13  Halley Gotway   Add parse_ssvar_line.
//   004    05/19/14  Halley Gotway   Add OBS_QC to MPR and ORANK lines.
//   005    05/20/14  Halley Gotway   Add AFSS, UFSS, F_RATE, and O_RATE
//                                      to the NBRCNT line.
//   006    06/03/14  Halley Gotway   Add PHIST line type.
//   007    01/04/17  Halley Gotway   Switch integer offsets to version
//                    independent column names.
//
////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <cmath>

#include "vx_log.h"

#include "parse_stat_line.h"

////////////////////////////////////////////////////////////////////////

void parse_fho_ctable(STATLine &l, TTContingencyTable &ct) {
   int n, fy, fy_oy, oy;
   double f_rate, h_rate, o_rate;

   n      = atoi(l.get_item("TOTAL"));
   f_rate = atof(l.get_item("F_RATE"));
   h_rate = atof(l.get_item("H_RATE"));
   o_rate = atof(l.get_item("O_RATE"));

   fy    = nint(n * f_rate);
   fy_oy = nint(n * h_rate);
   oy    = nint(n * o_rate);

   // FY_OY
   ct.set_fy_oy(fy_oy);

   // FY_ON
   ct.set_fy_on(fy - fy_oy);

   // FN_OY
   ct.set_fn_oy(oy - fy_oy);

   // FN_ON
   ct.set_fn_on(n - fy - oy + fy_oy);

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_ctc_ctable(STATLine &l, TTContingencyTable &ct) {

   // FY_OY
   ct.set_fy_oy(atoi(l.get_item("FY_OY")));

   // FY_ON
   ct.set_fy_on(atoi(l.get_item("FY_ON")));

   // FN_OY
   ct.set_fn_oy(atoi(l.get_item("FN_OY")));

   // FN_ON
   ct.set_fn_on(atoi(l.get_item("FN_ON")));

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_mctc_ctable(STATLine &l, ContingencyTable &ct) {
   int n_cat, i, j;
   char col_str[max_str_len];

   // N_CAT
   n_cat = atoi(l.get_item("N_CAT"));
   ct.set_size(n_cat);

   // Fi_Oj
   for(i=0; i<n_cat; i++) {
      for(j=0; j<n_cat; j++) {
         sprintf(col_str, "F%i_O%i", i+1, j+1);
         ct.set_entry(i, j, atoi(l.get_item(col_str)));
      }
   }

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_nbrctc_ctable(STATLine &l, TTContingencyTable &ct) {

   // FY_OY
   ct.set_fy_oy(atoi(l.get_item("FY_OY")));

   // FY_ON
   ct.set_fy_on(atoi(l.get_item("FY_ON")));

   // FN_OY
   ct.set_fn_oy(atoi(l.get_item("FN_OY")));

   // FN_ON
   ct.set_fn_on(atoi(l.get_item("FN_ON")));

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_nx2_ctable(STATLine &l, Nx2ContingencyTable &pct) {
   int i, n, oy, on;
   char col_str[max_str_len];
   double *thresh = (double *) 0;

   // N_THRESH
   n = atoi(l.get_item("N_THRESH"));
   pct.set_size(n-1);

   // Allocate space for list of thresholds
   thresh = new double [n];

   for(i=0; i<n-1; i++) {

      // THRESH_i
      sprintf(col_str, "THRESH_%i", i+1);
      thresh[i] = atof(l.get_item(col_str));

      // OY_i
      sprintf(col_str, "OY_%i", i+1);
      oy = atoi(l.get_item(col_str));
      pct.set_entry(i, nx2_event_column, oy);

      // ON_i
      sprintf(col_str, "ON_%i", i+1);
      on = atoi(l.get_item(col_str));
      pct.set_entry(i, nx2_nonevent_column, on);
   }

   // THRESH_n
   sprintf(col_str, "THRESH_%i", n);
   thresh[n-1] = atof(l.get_item(col_str));
   pct.set_thresholds(thresh);

   if(thresh) { delete thresh; thresh = (double *) 0; }

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_sl1l2_line(STATLine &l, SL1L2Info &s_info) {

   s_info.clear();

   s_info.scount = atoi(l.get_item("TOTAL"));
   s_info.fbar   = atof(l.get_item("FBAR"));
   s_info.obar   = atof(l.get_item("OBAR"));
   s_info.fobar  = atof(l.get_item("FOBAR"));
   s_info.ffbar  = atof(l.get_item("FFBAR"));
   s_info.oobar  = atof(l.get_item("OOBAR"));
   s_info.mae    = atof(l.get_item("MAE"));

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_sal1l2_line(STATLine &l, SL1L2Info &s_info) {

   s_info.clear();

   s_info.sacount = atoi(l.get_item("TOTAL"));
   s_info.fabar   = atof(l.get_item("FABAR"));
   s_info.oabar   = atof(l.get_item("OABAR"));
   s_info.foabar  = atof(l.get_item("FOABAR"));
   s_info.ffabar  = atof(l.get_item("FFABAR"));
   s_info.ooabar  = atof(l.get_item("OOABAR"));
   s_info.mae     = atof(l.get_item("MAE"));

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_vl1l2_line(STATLine &l, VL1L2Info &v_info) {

   v_info.clear();

   v_info.vcount  = atoi(l.get_item("TOTAL"));
   v_info.ufbar   = atof(l.get_item("UFBAR"));
   v_info.vfbar   = atof(l.get_item("VFBAR"));
   v_info.uobar   = atof(l.get_item("UOBAR"));
   v_info.vobar   = atof(l.get_item("VOBAR"));
   v_info.uvfobar = atof(l.get_item("UVFOBAR"));
   v_info.uvffbar = atof(l.get_item("UVFFBAR"));
   v_info.uvoobar = atof(l.get_item("UVOOBAR"));

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_val1l2_line(STATLine &l, VL1L2Info &v_info) {

   v_info.clear();

   v_info.vacount  = atoi(l.get_item("TOTAL"));
   v_info.ufabar   = atof(l.get_item("UFABAR"));
   v_info.vfabar   = atof(l.get_item("VFABAR"));
   v_info.uoabar   = atof(l.get_item("UOABAR"));
   v_info.voabar   = atof(l.get_item("VOABAR"));
   v_info.uvfoabar = atof(l.get_item("UVFOABAR"));
   v_info.uvffabar = atof(l.get_item("UVFFABAR"));
   v_info.uvooabar = atof(l.get_item("UVOOABAR"));

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_nbrcnt_line(STATLine &l, NBRCNTInfo &n_info) {

   n_info.clear();

   n_info.nbr_wdth   = atoi(l.get_item("INTERP_PNTS"));
   n_info.fthresh.set(l.get_item("FCST_THRESH", false));
   n_info.othresh.set(l.get_item("OBS_THRESH", false));
   n_info.sl1l2_info.scount = atoi(l.get_item("TOTAL"));
   n_info.fbs.v      = atof(l.get_item("FBS"));
   n_info.fss.v      = atof(l.get_item("FSS"));
   n_info.afss.v     = atof(l.get_item("AFSS"));
   n_info.ufss.v     = atof(l.get_item("UFSS"));
   n_info.f_rate.v   = atof(l.get_item("F_RATE"));
   n_info.o_rate.v   = atof(l.get_item("O_RATE"));

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_mpr_line(STATLine &l, MPRData &m_data) {

   m_data.fcst_var = l.fcst_var();
   m_data.obs_var  = l.obs_var();
   m_data.total    = atoi(l.get_item("TOTAL"));
   m_data.index    = atoi(l.get_item("INDEX"));
   m_data.obs_sid  = l.get_item("OBS_SID", false);
   m_data.obs_lat  = atof(l.get_item("OBS_LAT"));
   m_data.obs_lon  = atof(l.get_item("OBS_LON"));
   m_data.obs_lvl  = atof(l.get_item("OBS_LVL"));
   m_data.obs_elv  = atof(l.get_item("OBS_ELV"));
   m_data.fcst     = atof(l.get_item("FCST"));
   m_data.obs      = atof(l.get_item("OBS"));
   m_data.climo    = atof(l.get_item("CLIMO"));
   m_data.obs_qc   = l.get_item("OBS_QC", false);

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_isc_line(STATLine &l, ISCInfo &i_info, int &iscale) {

   i_info.total    = atoi(l.get_item("TOTAL"));
   i_info.tile_dim = atoi(l.get_item("TILE_DIM"));
   i_info.tile_xll = atoi(l.get_item("TILE_XLL"));
   i_info.tile_yll = atoi(l.get_item("TILE_YLL"));
   i_info.n_scale  = atoi(l.get_item("NSCALE"));
   iscale          = atoi(l.get_item("ISCALE"));
   i_info.mse      = atof(l.get_item("MSE"));
   i_info.isc      = atof(l.get_item("ISC"));
   i_info.fen      = atof(l.get_item("FENERGY2"));
   i_info.oen      = atof(l.get_item("OENERGY2"));
   i_info.baser    = atof(l.get_item("BASER"));
   i_info.fbias    = atof(l.get_item("FBIAS"));

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_rhist_line(STATLine &l, RHISTData &r_data) {
   int i;
   char col_str[max_str_len];

   r_data.total  = atoi(l.get_item("TOTAL"));
   r_data.crps   = atof(l.get_item("CRPS"));
   r_data.ign    = atof(l.get_item("IGN"));
   r_data.n_rank = atoi(l.get_item("N_RANK"));
   r_data.crpss  = atof(l.get_item("CRPSS"));
   r_data.spread = atof(l.get_item("SPREAD"));

   r_data.rhist_na.clear();

   // Parse out RANK_i
   for(i=0; i<r_data.n_rank; i++) {
      sprintf(col_str, "RANK_%i", i+1);
      r_data.rhist_na.add(atoi(l.get_item(col_str)));
   }

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_phist_line(STATLine &l, PHISTData &p_data) {
   int i;
   char col_str[max_str_len];

   p_data.total    = atoi(l.get_item("TOTAL"));
   p_data.bin_size = atof(l.get_item("BIN_SIZE"));
   p_data.n_bin    = atoi(l.get_item("N_BIN"));

   p_data.phist_na.clear();

   // Parse out BIN_i
   for(i=0; i<p_data.n_bin; i++) {
      sprintf(col_str, "BIN_%i", i+1);
      p_data.phist_na.add(atoi(l.get_item(col_str)));
   }

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_orank_line(STATLine &l, ORANKData &o_data) {
   int i;
   char col_str[max_str_len];

   o_data.total     = atoi(l.get_item("TOTAL"));
   o_data.index     = atoi(l.get_item("INDEX"));
   o_data.obs_sid   = l.get_item("OBS_SID", false);
   o_data.obs_lat   = atof(l.get_item("OBS_LAT"));
   o_data.obs_lon   = atof(l.get_item("OBS_LON"));
   o_data.obs_lvl   = atof(l.get_item("OBS_LVL"));
   o_data.obs_elv   = atof(l.get_item("OBS_ELV"));
   o_data.obs       = atof(l.get_item("OBS"));

   o_data.pit       = atof(l.get_item("PIT"));

   o_data.rank      = atoi(l.get_item("RANK"));
   o_data.n_ens_vld = atoi(l.get_item("N_ENS_VLD"));
   o_data.n_ens     = atoi(l.get_item("N_ENS"));

   // Parse out ENS_i
   o_data.ens_na.clear();
   for(i=0; i<o_data.n_ens; i++) {
      sprintf(col_str, "ENS_%i", i+1);
      o_data.ens_na.add(atof(l.get_item(col_str)));
   }

   o_data.obs_qc     = l.get_item("OBS_QC", false);
   o_data.ens_mean   = atof(l.get_item("ENS_MEAN"));
   o_data.climo      = atof(l.get_item("CLIMO"));
   o_data.ens_spread = atof(l.get_item("ENS_SPREAD"));

   return;
}

////////////////////////////////////////////////////////////////////////

void parse_ssvar_line(STATLine &l, SSVARInfo &ssvar_info) {

   ssvar_info.n_bin    = atoi(l.get_item("N_BIN"));
   ssvar_info.bin_i    = atoi(l.get_item("BIN_I"));
   ssvar_info.bin_n    = atoi(l.get_item("BIN_N"));

   ssvar_info.var_min  = atof(l.get_item("VAR_MIN"));
   ssvar_info.var_max  = atof(l.get_item("VAR_MAX"));
   ssvar_info.var_mean = atof(l.get_item("VAR_MEAN"));

   ssvar_info.sl1l2_info.scount = ssvar_info.bin_n;
   ssvar_info.sl1l2_info.fbar   = atof(l.get_item("FBAR"));
   ssvar_info.sl1l2_info.obar   = atof(l.get_item("OBAR"));
   ssvar_info.sl1l2_info.fobar  = atof(l.get_item("FOBAR"));
   ssvar_info.sl1l2_info.ffbar  = atof(l.get_item("FFBAR"));
   ssvar_info.sl1l2_info.oobar  = atof(l.get_item("OOBAR"));

   return;
}

////////////////////////////////////////////////////////////////////////