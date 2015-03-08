/* freq.h
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

#ifndef __freq_h__
#define __freq_h__

#include "global.h"

#include "hdf5.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include "structs.h"

freq_t *
freq_alloc (
  const long int id,
  const long int * const idl, const long int * const idu,
  const double * const binning,
  freq_t * const freq
);

void
freq_free (
  freq_t * freq
);

static unsigned long int
freq_charge (
  const data_t * const data
);

void
freq_accumulate (
  freq_t * const freq,
  const data_t * const data
);

void
freq_dump (
  const freq_t * const freq
);

unsigned long int
freq_counter (
  const freq_t * const freq
);

static void
freq_h5write (
  double * const buf, double * const bufcur, const size_t bufl,
  const hid_t dset,
  const freq_t * const freq, const double * const binning, const long int * const idl, const long int * const idu,
  const double er
);

void
freq_save (
  const hid_t dset,
  const freq_t * const freq,
  const size_t dim
);

#endif
