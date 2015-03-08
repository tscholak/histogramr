/* freq.c
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

#include "freq.h"

freq_t *
freq_alloc (
  const long int id,
  const long int * const idl, const long int * const idu,
  const double * const binning,
  freq_t * const freq
)
{
  freq_t * cur;
  
  cur = malloc (sizeof (* cur));
  
  cur->id = id;
  cur->idl = idl;
  cur->idu = idu;
  
  cur->binning = binning;
  
  cur->next = freq;
  cur->first = NULL;
  
  cur->c = 0;
  
  return (cur);
}

void
freq_free (
  freq_t * freq
)
{
  freq_t * cur, * next;
  
  next = freq->first;
  while ((cur = next))
  {
    next = cur->next;
    freq_free (cur);
  }
  free (freq);
  freq = NULL;
}

static unsigned long int
freq_charge (
  const data_t * const data
)
{
  size_t i;
  unsigned long int c;
  
  if (! data->v)
    c = 1;
  else
    for (i = 0, c = 0; i < data->b; i++)
      c += freq_charge (data->v[i]);
  
  return (c);
}

void
freq_accumulate (
  freq_t * const freq,
  const data_t * const data
)
{
  size_t i;
  long int id;
  freq_t * prev, * cur;
  
  freq->c += freq_charge (data);
  
  if (data->v)
  {
    i = 0;
    prev = NULL;
    cur = freq->first;
    
    while ((data->v[i])->id < * freq->idl)
      if (! (++i < data->b))
        return;
    if ((data->v[i])->id > * freq->idu)
      return;
    
    while (cur)
    {
      while ((id = (data->v[i])->id) < cur->id)
      {
        if (! prev)
        {
          prev = freq->first = freq_alloc (id, freq->idl + 1, freq->idu + 1, freq->binning + 1, cur);
          freq_accumulate (prev, data->v[i]);
        }
        else if (prev->id == id)
          freq_accumulate (prev, data->v[i]);
        else
        {
          prev = prev->next = freq_alloc (id, freq->idl + 1, freq->idu + 1, freq->binning + 1, cur);
          freq_accumulate (prev, data->v[i]);
        }
        if (! (++i < data->b))
          return;
        if ((data->v[i])->id > * freq->idu)
          return;
      }
      if (! prev)
        prev = cur;
      else
        prev = prev->next = cur;
      while ((data->v[i])->id == cur->id)
      {
        freq_accumulate (cur, data->v[i]);
        if (! (++i < data->b))
          return;
        if ((data->v[i])->id > * freq->idu)
          return;
      }
      cur = cur->next;
    }
    
    for (;;)
    {
      id = (data->v[i])->id;
      if (! prev)
      {
        prev = freq->first = freq_alloc (id, freq->idl + 1, freq->idu + 1, freq->binning + 1, NULL);
        freq_accumulate (prev, data->v[i]);
      }
      else if (prev->id == id)
        freq_accumulate (prev, data->v[i]);
      else
      {
        prev = prev->next = freq_alloc (id, freq->idl + 1, freq->idu + 1, freq->binning + 1, NULL);
        freq_accumulate (prev, data->v[i]);
      }
      if (! (++i < data->b))
        return;
      if ((data->v[i])->id > * freq->idu)
        return;
    }
  }
}

void
freq_dump (
  const freq_t * const freq
)
{
  freq_t * cur, * next;
  
  printf ("id = %li, c = %lu\n", freq->id, freq->c);
  
  next = freq->first;
  while ((cur = next))
  {
    next = cur->next;
    freq_dump (cur);
  }
}

unsigned long int
freq_counter (
  const freq_t * const freq
)
{
  unsigned long int c = 0;
  freq_t * cur, * next;
  
  next = freq->first;
  while ((cur = next))
  {
    next = cur->next;
    c += 1 + freq_counter (cur);
  }
  
  return (c);
}

static void
freq_h5write (
  double * const buf, double * const bufcur, const size_t bufl,
  const hid_t dset,
  const freq_t * const freq, const double * const binning, const long int * const idl, const long int * const idu,
  const double er
)
{
  long int id;
  freq_t * cur, * next;
  
  if (bufcur != & buf[bufl - 1])
  {
    if (freq)
    {
      next = freq->first;
      for (id = * freq->idl; id < * freq->idu; id++)
      {
        if (next)
        {
          while ((cur = next))
          {
            if (cur->id < id)
            {
              next = cur->next;
              continue;
            }
            else if (cur->id == id)
            {
              next = cur->next;
              * bufcur = ((double) cur->id + .5) * (* freq->binning);
              freq_h5write (
                buf, bufcur + 1, bufl,
                dset,
                cur, NULL, NULL, NULL,
                er
              );
              break;
            }
            else
            {
              * bufcur = ((double) id + .5) * (* freq->binning);
              freq_h5write (
                buf, bufcur + 1, bufl,
                dset,
                NULL, freq->binning + 1, freq->idl + 1, freq->idu + 1,
                er
              );
              break;
            }
          }
        }
        else
        {
          * bufcur = ((double) id + .5) * (* freq->binning);
          freq_h5write (
            buf, bufcur + 1, bufl,
            dset,
            NULL, freq->binning + 1, freq->idl + 1, freq->idu + 1,
            er
          );
        }
      }
    }
    else
    {
      for (id = * idl; id < * idu; id++)
      {
        * bufcur = ((double) id + .5) * (* binning);
        freq_h5write (
          buf, bufcur + 1, bufl,
          dset,
          NULL, binning + 1, idl + 1, idu + 1,
          er
        );
      }
    }
  }
  else
  {
    if (freq)
      * bufcur = (double) freq->c * er;
    else
      * bufcur = 0.;
    
    hid_t space, memspace;
    herr_t status;
    hsize_t dims[2],
            extdims[2],
            memdims[2],
            memmaxdims[2],
            start[2],
            count[2];
    
    space = H5Dget_space (dset);
    H5Sget_simple_extent_dims (space, dims, NULL);
    status = H5Sclose (space);
    extdims[0] = dims[0] + 1;
    extdims[1] = dims[1];
    status = H5Dset_extent (dset, extdims);
    
    space = H5Dget_space (dset);
    status = H5Sselect_all (space);
    start[0] = 0;
    start[1] = 0;
    count[0] = dims[0];
    count[1] = dims[1];
    status = H5Sselect_hyperslab (space, H5S_SELECT_NOTB, start, NULL, count, NULL);
    memdims[0] = 1;
    memdims[1] = dims[1];
    memmaxdims[0] = H5S_UNLIMITED;
    memmaxdims[1] = dims[1];
    memspace = H5Screate_simple (2, memdims, memmaxdims);
    status = H5Dwrite (
      dset,
      H5T_NATIVE_DOUBLE,
      memspace, space, H5P_DEFAULT,
      buf
    );
    status = H5Sclose (memspace);
    status = H5Sclose (space);
  }
}

void
freq_save (
  const hid_t dset,
  const freq_t * const freq,
  const size_t dim
)
{
  size_t i;
  const size_t bufl = dim + 1;
  double er, * buf;
  
  /* ensemble ratio */
  er = (double) freq->c;
  for (i = 0; i < dim; i++)
    er *= freq->binning[i];
  er = 1. / er;
  
  /* write to hdf5 dataset */
  buf = calloc (bufl, sizeof (* buf));
  freq_h5write (
    buf, buf, bufl,
    dset,
    freq, NULL, NULL, NULL,
    er
  );
  free (buf);
}
