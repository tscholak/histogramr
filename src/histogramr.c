/* main.c
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

#include "global.h"

#include "hdf5.h"

#include <stdio.h>

#ifdef TIMING
#include <sys/time.h>
#include <time.h>
#define EMA_SMOOTHING .1
#endif

#include <math.h>

typedef enum {FALSE = 0, TRUE} boolean;

#include "structs.h"
#include "options.h"
#include "data.h"
#include "freq.h"

char
load (
  size_t * const, size_t * const, double **** const, const hid_t, const options_t * const
);

void
commit (
  freq_t * const, const size_t, const size_t, const double * const * const * const * const, const options_t * const
);

void
save (
  const hid_t, const hid_t, const freq_t * const, const options_t * const
);

void
copy_attr (
  const hid_t, const hid_t, const char * const
);


int
main (
  int argc, char * argv[]
)
{  
  options_t * const options = malloc (sizeof (* options));
  options_defaults (options);
  options_prep (options, argc, argv);
  
  size_t i, j;
  
  freq_t * freq;
  freq = freq_alloc (
           0,
           options->limit_idl_merged, options->limit_idu_merged,
           options->binning_merged,
           NULL
         );

#ifdef TIMING
  struct timeval * const tv = malloc (sizeof (* tv));
  double begin, now, speed_ema, speed_cur;
  gettimeofday (tv, NULL);
  begin = speed_cur = (double) tv->tv_sec + (double) tv->tv_usec / 1e6;
#endif
  
  for (i = 0; i < options->ninput; i++)
  {
    size_t dataset_length = 0,
           compound_member_length = 0;
#ifdef TIMING
    double t;
#endif
    double *** raw[NDATASET_MAX];
    hid_t file_in, file_out;
    herr_t status;
    herr_t h5_error = -1;
    
    /* open file */
    if ((file_in = H5Fopen (options->input[i], H5F_ACC_RDONLY, H5P_DEFAULT)) == h5_error)
    {
      fprintf (stderr, "warning: file `%s' could not be opened, skipping.\n", options->input[i]);
      continue;
    }
   
    /* read from file */
#ifdef TIMING
    gettimeofday (tv, NULL);
    t = (double) tv->tv_sec + (double) tv->tv_usec / 1e6;
#endif
    if (! load (& dataset_length, & compound_member_length, raw, file_in, options))
    {
      status = H5Fclose (file_in);
      continue;
    }
#ifdef TIMING
    gettimeofday (tv, NULL);
    printf ("loaded: %s, time: %g s\n", options->input[i], (double) tv->tv_sec + (double) tv->tv_usec / 1e6 - t);
#else
    printf ("loaded: %s\n", options->input[i]);
#endif

    /* accumulate statistics */
#ifdef TIMING
    gettimeofday (tv, NULL);
    t = (double) tv->tv_sec + (double) tv->tv_usec / 1e6;
#endif
    if (dataset_length && compound_member_length)
    {
      commit (freq, dataset_length, compound_member_length, raw, options);
#ifdef TIMING
      gettimeofday (tv, NULL);
      printf ("committed: %s, time: %g s\n", options->input[i], (double) tv->tv_sec + (double) tv->tv_usec / 1e6 - t);
#else
      printf ("committed: %s\n", options->input[i]);
#endif
      /* free buffers */
      for (j = 0; j < NDATASET_MAX;j++)
      {
        if (options->dim[j])
        {
          free (raw[j][0][0]);
          free (raw[j][0]);
          free (raw[j]);
        }
      }
    }

    /* save statistics and attributes to hdf5 file */
    if ((i > 0 && i % options->savevery == 0) || i + 1 == options->ninput)
    {
#ifdef TIMING
      double t;
#endif

#ifdef TIMING
      gettimeofday (tv, NULL);
      t = (double) tv->tv_sec + (double) tv->tv_usec / 1e6;
#endif
      file_out = H5Fcreate (options->output, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
      save (file_out, file_in, freq, options);
      status = H5Fclose (file_out);
#ifdef TIMING
      gettimeofday (tv, NULL);
      printf ("saved: %s, time: %g s\n", options->output, (double) tv->tv_sec + (double) tv->tv_usec / 1e6 - t);
#else
      printf ("saved: %s\n", options->output);
#endif
    }
    
    status = H5Fclose (file_in);
    
    /* print feedback */
#ifdef TIMING
    gettimeofday (tv, NULL);
    now = (double) tv->tv_sec + (double) tv->tv_usec / 1e6;
    speed_cur = now - speed_cur;
    if (! i)
      speed_ema = speed_cur;
    else
      speed_ema = (speed_ema * (1. - EMA_SMOOTHING)) + (speed_cur * EMA_SMOOTHING);
    printf (
      "done: %s, freq charge: %lu, freq structure count: %lu, time elapsed: %g s, currently: %g s per file, to go: %lu files, eta: %g s\n\n",
      options->input[i],
      freq->c,
      freq_counter (freq),
      now - begin,
      speed_cur,
      options->ninput - i - 1,
      speed_ema * (double) (options->ninput - i - 1)
    );
    speed_cur = now;
#else
    printf (
      "done: %s, freq charge: %lu, freq structure count: %lu, to go: %lu files\n\n",
      options->input[i],
      freq->c,
      freq_counter (freq),
      options->ninput - i - 1
    );
#endif
  }
  
  freq_free (freq);
#ifdef TIMING
  free (tv);
#endif
  options_free (options);
  
  return (EXIT_SUCCESS);
}

char
load (
  size_t * const dataset_length_p,
  size_t * const compound_member_length_p,
  double **** const raw,
  const hid_t file,
  const options_t * const options
)
{
  size_t i, k, l;
  H5T_class_t compound_member_class = 0;
  hid_t compound_member_type = 0;
  
  for (i = 0; i < NDATASET_MAX; i++)
    if (options->dim[i])
    {
      size_t rank;
      
      hid_t dset, dtype, native_type, space, memtype;
      hsize_t dims[1];
      H5T_class_t class;
      herr_t status;
      
      if(! H5Lexists (file, options->dataset[i], H5P_DEFAULT))
      {
        fprintf (stderr, "warning: dataset `%s' does not exist, skipping file.\n", options->dataset[i]);
        return FALSE;
      }

      dset = H5Dopen (file, options->dataset[i], H5P_DEFAULT);
      
      dtype = H5Dget_type (dset);
      native_type = H5Tget_native_type (dtype, H5T_DIR_DEFAULT);
      
      if ((class = H5Tget_class (dtype)) != H5T_COMPOUND)
      {
        fprintf (stderr, "warning: dataset type is not compound, skipping file.\n");
        status = H5Dclose (dset);
        status = H5Tclose (dtype);
        status = H5Tclose (native_type);
        return FALSE;
      }
      
      space = H5Dget_space (dset);
      rank = H5Sget_simple_extent_ndims (space);
      if (rank != 1)
      {
        fprintf (stderr, "warning: dataspace rank has to be 1, skipping file.\n");
        status = H5Dclose (dset);
        status = H5Tclose (dtype);
        status = H5Tclose (native_type);
        status = H5Sclose (space);
        return FALSE;
      }
      
      H5Sget_simple_extent_dims (space, dims, NULL);
      if (! (* dataset_length_p))
        * dataset_length_p = dims[0];
      else if ((* dataset_length_p) != dims[0])
      {
        fprintf (stderr, "warning: content of dataspaces must have the same length, skipping file.\n");
        status = H5Dclose (dset);
        status = H5Tclose (dtype);
        status = H5Tclose (native_type);
        status = H5Sclose (space);
        return FALSE;
      }

      if (dims[0])
      {
        for (l = 0; l < options->dim[i]; l++)
        {
          int field_id;
          size_t member_length;
          hid_t member_type;
          H5T_class_t member_class;
        
          field_id = H5Tget_member_index (native_type, options->member[i][l]);
          member_type = H5Tget_member_type (dtype, field_id);

          member_length = H5Tget_size (member_type) / sizeof (double);
          if (! (* compound_member_length_p))
            * compound_member_length_p = member_length;
          else if ((* compound_member_length_p) != member_length)
          {
            fprintf (stderr, "fatal: content of compound members must have the same length.\n");
            exit (EXIT_FAILURE);
          }

          member_class = H5Tget_class (member_type);
          if (! compound_member_class)
          {
            compound_member_class = member_class;

            if (compound_member_class == H5T_FLOAT)
            {
              memtype = H5Tcreate (H5T_COMPOUND, options->dim[i] * sizeof (double));

              compound_member_type = H5T_NATIVE_DOUBLE;
            }
            else if (compound_member_class == H5T_ARRAY)
            {
              hsize_t adim[1] = {(* compound_member_length_p)};

              memtype = H5Tcreate (H5T_COMPOUND, options->dim[i] * (* compound_member_length_p) * sizeof (double));

              compound_member_type = H5Tarray_create (H5T_NATIVE_DOUBLE, 1, adim);
            }
            else
            {
              fprintf (stderr, "fatal: compound member class must be either of float or array type.\n");
              exit (EXIT_FAILURE);
            }
          }
          else if (compound_member_class != member_class)
          {
            fprintf (stderr, "fatal: compound members must belong to the same class.\n");
            exit (EXIT_FAILURE);
          }

          status = H5Tinsert (memtype, options->member[i][l], l * (* compound_member_length_p) * sizeof (double), compound_member_type);
        
          status = H5Tclose (member_type);
        }

        for (k = 0; k < (* dataset_length_p); k++)
        {
          for (l = 0; l < options->dim[i]; l++)
          {
            if (k == 0 && l == 0)
            {
              raw[i] = malloc ((* dataset_length_p) * sizeof (double **));
              raw[i][0] = malloc ((* dataset_length_p) * options->dim[i] * sizeof (double *));
              raw[i][0][0] = malloc ((* dataset_length_p) * options->dim[i] * (* compound_member_length_p) * sizeof (double));
            }
            else if (k > 0 && l == 0)
            {
              raw[i][k] = raw[i][0] + k * options->dim[i];
              raw[i][k][0] = raw[i][0][0] + k * options->dim[i] * (* compound_member_length_p);
            }
            else
            {
              raw[i][k][l] = raw[i][k][0] + l * (* compound_member_length_p);
            }
          }
        }

        status = H5Dread (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, raw[i][0][0]);
    
        status = H5Tclose (memtype);
      }
  
      status = H5Dclose (dset);
      status = H5Tclose (dtype);
      status = H5Tclose (native_type);
      status = H5Sclose (space);

      if (! dims[0])
      {
        fprintf (stderr, "warning: dataspace is empty, skipping.\n");
        return FALSE;
      }
    }

  if (compound_member_class == H5T_ARRAY)
    H5Tclose (compound_member_type);

  return TRUE;
}

void
commit (
  freq_t * const freq,
  const size_t dataset_length,
  const size_t compound_member_length,
  const double * const * const * const * const raw,
  const options_t * const options
)
{
  size_t i, j, k, l, m;
  double r;
  
  const size_t bc = options->dim_merged;
  size_t bv[bc], dv[bc];
  bv[0] = dataset_length * compound_member_length;
  for (i = 1; i < bc; i++)
  {
    bv[i] = 1;
    dv[i] = 0;
  }
  
  data_t * data, * descendant;
  data = data_alloc (bc, bv);
  
  for (i = 0, j = 0; i < NDATASET_MAX; i++)
  {
    if (options->dim[i])
    {
      for (k = 0; k < dataset_length; k++)
      {
        for (l = 0; l < options->dim[i]; l++)
        {
          for (m = 0; m < compound_member_length; m++)
          {
            dv[0] = k * compound_member_length + m;
            descendant = descend (data, j + l + 1, dv);
            r = raw[i][k][l][m];
            if (options->l10[i])
            {
              if (options->l10[i][l] == 1)
              {
                if (options->limit_l[i][l] > 0 && options->limit_u[i][l] > 0)
                  r = log10 (r);
                else if (options->limit_l[i][l] < 0 && options->limit_u[i][l] < 0)
                  r = log10 (- r);
                else
                  // can't happen: if this were the case program would have quitted earlier
                  exit (EXIT_FAILURE);
              }
            }
            descendant->id = (long int) floor (r / options->binning[i][l]);
          }
        }
      }
    }
    j += options->dim[i];
  }
  
  data_sort (data);
  
  freq_accumulate (freq, data);
  
  data_free (data);
}

void
save (
  const hid_t file_out, const hid_t file_in,
  const freq_t * const freq,
  const options_t * const options
)
{
  size_t i;
  hid_t grp_in, grp_out, dcpl, dset_out, space_out, space_charge, attr_charge;
  hsize_t dims_out[2] = {0, options->dim_merged + 1},
          maxdims_out[2] = {H5S_UNLIMITED, options->dim_merged + 1},
          chunk[2] = {options->chunk, options->dim_merged + 1},
          dims_charge[1] = {1};
  herr_t status;
  
  /* copy group attributes */
  grp_in = H5Gopen (file_in, "/", H5P_DEFAULT);
  grp_out = H5Gopen (file_out, "/", H5P_DEFAULT);
  copy_attr (grp_in, grp_out, NULL);
  status = H5Gclose (grp_in);
  status = H5Gclose (grp_out);
  
  /* set compression */
  dcpl = H5Pcreate (H5P_DATASET_CREATE);
  //status = H5Pset_deflate (dcpl, 9);
  status = H5Pset_chunk (dcpl, 2, chunk);
  
  /* create dataset */    
  space_out = H5Screate_simple (2, dims_out, maxdims_out);
  dset_out = H5Dcreate (file_out, "probability density", H5T_IEEE_F64BE, space_out, H5P_DEFAULT, dcpl, H5P_DEFAULT);
  
  /* copy attributes */
  for (i = 0; i < NDATASET_MAX; i++)
  {
    if (options->dim[i])
    {
      hid_t dset_in;
      
      dset_in = H5Dopen (file_in, options->dataset[i], H5P_DEFAULT);
      copy_attr (dset_in, dset_out, options->dataset[i]);
      status = H5Dclose (dset_in);
    }
  }
  
  /* write options */
  options_write (options, dset_out);
  
  space_charge = H5Screate_simple (1, dims_charge, NULL);
  attr_charge = H5Acreate (dset_out, "charge", H5T_STD_U64BE, space_charge, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Awrite (attr_charge, H5T_NATIVE_ULONG, & freq->c);
  status = H5Sclose (space_charge);
  status = H5Aclose (attr_charge);
  
  freq_save (
    dset_out,
    freq,
    options->dim_merged
  );
  
  status = H5Dclose (dset_out);
  status = H5Sclose (space_out);
  status = H5Pclose (dcpl);
}

void
copy_attr (
  const hid_t loc_in, const hid_t loc_out,
  const char * const suffix
)
{
  hid_t attr_id, attr_out, space_id, ftype_id, wtype_id;
  size_t msize; /* size of type */
  void * buf = NULL; /* data buffer */
  hsize_t nelmts; /* number of elements in dataset */
  int rank; /* rank of dataset */
  htri_t is_named; /* whether the datatype is named */
  hsize_t dims[H5S_MAX_RANK]; /* dimensions of dataset */
  char name[255];
  H5O_info_t oinfo; /* object info */
  int j;
  unsigned u;
  
  H5Oget_info (loc_in, & oinfo);
  
  /* copy all attributes */
  for (u = 0; u < (unsigned) oinfo.num_attrs; u++)
  {
    buf = NULL;
    
    /* open attribute */
    attr_id = H5Aopen_by_idx (loc_in, ".", H5_INDEX_CRT_ORDER, H5_ITER_INC, (hsize_t) u, H5P_DEFAULT, H5P_DEFAULT);
    
    /* get name */
    H5Aget_name (attr_id, (size_t) 255, name);
    if (suffix)
      sprintf (& name[strlen (name)], " (%s)", suffix);
    
    /* get the file datatype  */
    ftype_id = H5Aget_type (attr_id);
    
    /* get the dataspace handle  */
    space_id = H5Aget_space (attr_id);
    
    /* get dimensions  */
    rank = H5Sget_simple_extent_dims (space_id, dims, NULL);
    for (j = 0, nelmts=1; j < rank; j++)
      nelmts *= dims[j];
    
    wtype_id = H5Tcopy (ftype_id);
    
    msize = H5Tget_size (wtype_id);
    
    if (H5T_REFERENCE == H5Tget_class (wtype_id))
      ;
    else 
    {
      /* read to memory */
      buf = malloc ((size_t) (nelmts * msize));
      H5Aread (attr_id, wtype_id, buf);
      
      /* copy */
      attr_out = H5Acreate2 (loc_out, name, wtype_id, space_id, H5P_DEFAULT, H5P_DEFAULT);
      H5Awrite (attr_out, wtype_id, buf);
      
      /*close*/
      H5Aclose (attr_out);
      
      free (buf);
    }
    
    /* close */
    H5Tclose (ftype_id);
    H5Tclose (wtype_id);
    H5Sclose (space_id);
    H5Aclose (attr_id);
  }
}
