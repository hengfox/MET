// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2019
// ** University Corporation for Atmospheric Research (UCAR)
// ** National Center for Atmospheric Research (NCAR)
// ** Research Applications Lab (RAL)
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

////////////////////////////////////////////////////////////////////////

#ifndef  __THERMO_H__
#define  __THERMO_H__

////////////////////////////////////////////////////////////////////////

#include "vx_util.h"
#include "vx_log.h"

////////////////////////////////////////////////////////////////////////

const double epsilon = 0.622; // molecular weight ratio (water / dry air)

////////////////////////////////////////////////////////////////////////

double mixing_ratio(double part_press, double tot_press);

////////////////////////////////////////////////////////////////////////

double virtual_temperature(double temperature, double mixing);

////////////////////////////////////////////////////////////////////////

double saturation_vapor_pressure(double temperature);

////////////////////////////////////////////////////////////////////////

#endif  // __THERMO_H__

////////////////////////////////////////////////////////////////////////
