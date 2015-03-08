/* data.c
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

#include "data.h"

data_t *
data_alloc (
  const size_t bc, const size_t * const bv
)
{
  size_t i;
  data_t * data;
  
  data = malloc (sizeof (* data));
  data->id = 0;
  data->b = 0;
  data->v = NULL;
  
  if (bc)
  {
    data->b = bv[0];
    data->v = malloc (sizeof (* (data->v)) * data->b);
    for (i = 0; i < data->b; i++)
      data->v[i] = data_alloc (bc - 1, & bv[1]);
  }
  
  return (data);
}

void
data_free (
  data_t * data
)
{
  size_t i;
  
  if (data->v)
  {
    for (i = 0; i < data->b; i++)
      data_free (data->v[i]);
    free (data->v);
  }
  free (data);
  data = NULL;
}

static int
data_sort_compare (
  const void * a, const void * b
)
{
  data_t * const * da = a;
  data_t * const * db = b;
  long int ida = (* da)->id;
  long int idb = (* db)->id;
  
  if (ida < idb)
    return (-1);
  else if (ida == idb)
    return (0);
  else
    return (1);
}

void
data_sort (
  data_t * const data
)
{
  size_t i;
  
  if (data->v)
  {
    qsort (data->v, data->b, sizeof (* (data->v)), data_sort_compare);
    for (i = 0; i < data->b; i++)
      data_sort (data->v[i]);
  }
}

void
data_dump (
  const data_t * const data
)
{
  size_t i;
  
  printf ("%lu %li\n", (unsigned long int) data->b, data->id);
  if (data->v)
    for (i = 0; i < data->b; i++)
      data_dump (data->v[i]);
}

data_t *
descend (
  data_t * const data,
  const size_t dc, const size_t * const dv
)
{
  if (dc)
    return (descend (data->v[dv[0]], dc - 1, & dv[1]));
  else
    return (data);
}
