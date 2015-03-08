/* structs.h
 *
 * Copyright (C) 2015 Torsten Scholak <torsten.scholak@googlemail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __structs_h__
#define __structs_h__

#include "global.h"

#include "hdf5.h"

#include <stdbool.h>

#define NDATASET_MAX 10

typedef struct
{
  size_t ninput;
  char ** input;
  char * output;
  size_t savevery;
  
  size_t chunk;
  
  char * dataset[NDATASET_MAX];
  size_t dim[NDATASET_MAX];
  char ** member[NDATASET_MAX];
  double * binning[NDATASET_MAX],
         * limit_l[NDATASET_MAX],
         * limit_u[NDATASET_MAX];
  hbool_t * l10[NDATASET_MAX];
  
  size_t dim_merged;
  char ** member_merged;
  double * binning_merged,
         * limit_l_merged,
         * limit_u_merged;
  long int * limit_idl_merged,
           * limit_idu_merged;
  bool * l10_merged;
}
options_t;

typedef struct data
{
  long int id;
  size_t b;
  struct data ** v;
}
data_t;

typedef struct freq
{
  long int id;
  const long int * idl, * idu;
  
  unsigned long int c;
  
  const double * binning;
  
  struct freq * first, * next;
}
freq_t;

#endif
