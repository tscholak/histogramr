/* options.h
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

#ifndef __options_h__
#define __options_h__

#include "global.h"

#include "hdf5.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <limits.h>
#include <float.h>

#include <math.h>

#include "structs.h"

enum
{
  OPT_DATASET = 'd',
  OPT_ATTRIBUTE = 'a',
  OPT_MEMBER = 'm',
  OPT_BINNING = 'b',
  OPT_LIMIT = 'l',

  OPT_L10 = 'L',
  
  OPT_INPUT = 'i',
  OPT_OUTPUT = 'o',
  
  OPT_SAVEVERY = 'e',

  OPT_HELP = 'h',
  OPT_VERSION = 'V'
};

void
options_defaults (
  options_t * const options
);

void
options_free (
  options_t * const options
);

void
options_prep (
  options_t * const options,
  int argc, char * const argv[]
);

void
options_write (
  const options_t * const options,
  const hid_t dset
);

static size_t
countchar (
  const char * const str, const char what
);

static int
parse_member (
  char ** const member, char * str, const size_t dim
);

static int
parse_binning (
  double * const binning, char * str, const size_t dim
);

static int
parse_limit (
  double * const limit_l, double * const limit_u, char * str, const size_t dim
);

static int
parse_l10 (
  hbool_t * const l10, char * str, const size_t dim
);

static bool
strtobool (
  const char * str
);

static void
print_usage (
  void
);

static void
print_version (
  void
);

#endif
