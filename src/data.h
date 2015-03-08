/* data.h
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

#ifndef __data_h__
#define __data_h__

#include "global.h"

#include <stdio.h>

#include "structs.h"

data_t *
data_alloc (
  const size_t bc, const size_t * const bv
);

void
data_free (
  data_t * data
);

static int
data_sort_compare (
  const void * a, const void * b
);

void
data_sort (
  data_t * const data
);

void
data_dump (
  const data_t * const data
);

data_t *
descend (
  data_t * const data,
  const size_t dc, const size_t * const dv
);

#endif
