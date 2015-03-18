/* options.c
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

#include "options.h"

void
options_defaults (
  options_t * const options
)
{
  options->ninput = 0;
  options->input = NULL;
  options->output = NULL;
  options->savevery = 1;
  
  options->chunk = 64;
  
  size_t ndataset = 0;
  do
  {
    options->dataset[ndataset] = NULL;
    options->dim[ndataset] = 0;
    options->member[ndataset] = NULL;
    options->binning[ndataset] = NULL;
    options->limit_l[ndataset] = NULL;
    options->limit_u[ndataset] = NULL;
    options->l10[ndataset] = NULL;
  }
  while (++ndataset < NDATASET_MAX);
}

void
options_prep (
  options_t * const options,
  int argc, char * const argv[]
)
{
  int optchar;
  size_t ndataset = 0;
  
  static const char short_options[] = {
    OPT_INPUT, ':',
    OPT_OUTPUT, ':',
    OPT_SAVEVERY, ':',
    
    OPT_DATASET, ':',
    OPT_MEMBER, ':',
    OPT_BINNING, ':',
    OPT_LIMIT, ':',
    OPT_L10, ':',
    
    OPT_HELP, ':',
    OPT_VERSION, ':'
  };
  static const struct option long_options[] = {
    { "input", required_argument, NULL, OPT_INPUT },
    { "output", required_argument, NULL, OPT_OUTPUT },
    { "save-every", required_argument, NULL, OPT_SAVEVERY },
    
    { "dataset", required_argument, NULL, OPT_DATASET },
    { "member", required_argument, NULL, OPT_MEMBER },
    { "binning", required_argument, NULL, OPT_BINNING },
    { "limit", required_argument, NULL, OPT_LIMIT },
    { "l10", required_argument, NULL, OPT_L10 },
    
    { "help", no_argument, NULL, OPT_HELP },    
    { "version", no_argument, NULL, OPT_VERSION },
    
    { NULL, 0, NULL, 0 }
  };
  
  while ((optchar = getopt_long (argc, argv, short_options, long_options, NULL)) != EOF)
    switch (optchar)
    {
      case OPT_INPUT:
        options->ninput = 1;
        options->input = malloc (options->ninput * sizeof (* options->input));
        options->input[0] = optarg;
        break;
      case OPT_OUTPUT:
        options->output = optarg;
        break;
      case OPT_SAVEVERY:
        options->savevery = (size_t) atoi (optarg);
        break;
      
      case OPT_DATASET:
        if (ndataset++ < NDATASET_MAX)
          options->dataset[ndataset - 1] = optarg;
        else
        {
          fprintf (stderr, "fatal: too many datasets.\n"
                           "try '%s --help' for more information\n", PACKAGE_NAME);
          exit (EXIT_FAILURE);
        };       
        break;
      case OPT_MEMBER:
        if (! ndataset)
        {
          fprintf (stderr, "fatal: dataset must be specified as the first argument.\n"
                           "try '%s --help' for more information\n", PACKAGE_NAME);
          exit (EXIT_FAILURE);
        }
        if (! options->dim[ndataset - 1])
          options->dim[ndataset - 1] = countchar (optarg, ':') + 1;
        else
          if (options->dim[ndataset - 1] != countchar (optarg, ':') + 1)
          {
            fprintf (stderr, "fatal: option length mismatch.\n"
                             "try '%s --help' for more information\n", PACKAGE_NAME);
            exit (EXIT_FAILURE);
          }
        options->member[ndataset - 1] = malloc (options->dim[ndataset - 1] * sizeof (* options->member[ndataset - 1]));
        if (parse_member (options->member[ndataset - 1], optarg, options->dim[ndataset - 1]) == EXIT_FAILURE)
        {
          fprintf (stderr, "fatal: parsing of members failed.\n"
                           "try '%s --help' for more information\n", PACKAGE_NAME);
          exit (EXIT_FAILURE);
        }
        break;
      case OPT_BINNING:
        if (! ndataset)
        {
          fprintf (stderr, "fatal: dataset must be specified as the first argument.\n"
                           "try '%s --help' for more information\n", PACKAGE_NAME);
          exit (EXIT_FAILURE);
        }
        if (! options->dim[ndataset - 1])
          options->dim[ndataset - 1] = countchar (optarg, ':') + 1;
        else
          if (options->dim[ndataset - 1] != countchar (optarg, ':') + 1)
          {
            fprintf (stderr, "fatal: option length mismatch.\n"
                             "try '%s --help' for more information\n", PACKAGE_NAME);
            exit (EXIT_FAILURE);
          }
        options->binning[ndataset - 1] = malloc (options->dim[ndataset - 1] * sizeof (* options->binning[ndataset - 1]));
        if (parse_binning (options->binning[ndataset - 1], optarg, options->dim[ndataset - 1]) == EXIT_FAILURE)
        {
          fprintf (stderr, "fatal: parsing of binnings failed.\n"
                           "try '%s --help' for more information\n", PACKAGE_NAME);
          exit (EXIT_FAILURE);
        }
        break;
      case OPT_LIMIT:
        if (! ndataset)
        {
          fprintf (stderr, "fatal: dataset must be specified as the first argument.\n"
                           "try '%s --help' for more information\n", PACKAGE_NAME);
          exit (EXIT_FAILURE);
        }
        if (! options->dim[ndataset - 1])
          options->dim[ndataset - 1] = countchar (optarg, ':') + 1;
        else
          if (options->dim[ndataset - 1] != countchar (optarg, ':') + 1)
          {
            fprintf (stderr, "fatal: option length mismatch.\n"
                             "try '%s --help' for more information\n", PACKAGE_NAME);
            exit (EXIT_FAILURE);
          }
        options->limit_l[ndataset - 1] = malloc (options->dim[ndataset - 1] * sizeof (* options->limit_l[ndataset - 1]));
        options->limit_u[ndataset - 1] = malloc (options->dim[ndataset - 1] * sizeof (* options->limit_u[ndataset - 1]));
        if (parse_limit (options->limit_l[ndataset - 1], options->limit_u[ndataset - 1], optarg, options->dim[ndataset - 1]) == EXIT_FAILURE)
        {
          fprintf (stderr, "fatal: parsing of limits failed.\n"
                           "try '%s --help' for more information\n", PACKAGE_NAME);
          exit (EXIT_FAILURE);
        }
        break;
      case OPT_L10:
        if (! ndataset)
        {
          fprintf (stderr, "fatal: dataset must be specified as the first argument.\n"
                           "try '%s --help' for more information\n", PACKAGE_NAME);
          exit (EXIT_FAILURE);
        }
        if (! options->dim[ndataset - 1])
          options->dim[ndataset - 1] = countchar (optarg, ':') + 1;
        else
          if (options->dim[ndataset - 1] != countchar (optarg, ':') + 1)
          {
            fprintf (stderr, "fatal: option length mismatch.\n"
                             "try '%s --help' for more information\n", PACKAGE_NAME);
            exit (EXIT_FAILURE);
          }
        options->l10[ndataset - 1] = malloc (options->dim[ndataset - 1] * sizeof (* options->l10[ndataset - 1]));
        if (parse_l10 (options->l10[ndataset - 1], optarg, options->dim[ndataset - 1]) == EXIT_FAILURE)
        {
          fprintf (stderr, "fatal: parsing failed.\n"
                           "try '%s --help' for more information\n", PACKAGE_NAME);
          exit (EXIT_FAILURE);
        }
        break;
      
      case OPT_HELP:
        print_usage ();
        exit (EXIT_SUCCESS);
      case OPT_VERSION:
        print_version ();
        exit (EXIT_SUCCESS);

      case '\0':
        break;
      default:
        fprintf (stderr, "try '%s --help' for more information\n", PACKAGE_NAME);
        exit (EXIT_FAILURE);
    }
  
  if (optind < argc)
  {
    if (options->ninput == 1)
    {
      fprintf (stderr, "fatal: too many arguments.\n"
                       "try '%s --help' for more information\n", PACKAGE_NAME);
      exit (EXIT_FAILURE);
    }
    else
    {
      int i;
      
      options->ninput = argc - optind;
      options->input = malloc (options->ninput * sizeof (* options->input));
      for (i = optind; i < argc; i++)
        options->input[i - optind] = argv[i];
    }
  }
  
  if (! options->input)
  {
    fprintf (stderr, "fatal: no input file(s) given.\n"
                     "try '%s --help' for more information\n", PACKAGE_NAME);
    exit (EXIT_FAILURE);
  }
  if (! options->output)
  {
    fprintf (stderr, "fatal: no output filename specified.\n"
                     "try '%s --help' for more information\n", PACKAGE_NAME);
    exit (EXIT_FAILURE);
  }
  
  if (! ndataset)
  {
    fprintf (stderr, "fatal: no dataset given.\n"
                     "try '%s --help' for more information\n", PACKAGE_NAME);
    exit (EXIT_FAILURE);
  }
  else
  {
    size_t i, j, k;
    double l, u;
    
    for (i = 0, options->dim_merged = 0; i < ndataset; i++)
      options->dim_merged += options->dim[i];
    
    options->member_merged = malloc (options->dim_merged * sizeof (* options->member_merged));
    
    options->binning_merged = malloc (options->dim_merged * sizeof (* options->binning_merged));
    
    options->limit_l_merged = malloc (options->dim_merged * sizeof (* options->limit_l_merged));
    options->limit_u_merged = malloc (options->dim_merged * sizeof (* options->limit_u_merged));
    
    options->limit_idl_merged = malloc (options->dim_merged * sizeof (* options->limit_idl_merged));
    options->limit_idu_merged = malloc (options->dim_merged * sizeof (* options->limit_idu_merged));
    
    options->l10_merged = malloc (options->dim_merged * sizeof (* options->l10_merged));
    
    for (i = 0, j = 0; i < ndataset; i++)
    {
      if (! options->member[i])
      {
        fprintf (stderr, "fatal: no dataset members selected.\n"
                         "try '%s --help' for more information\n", PACKAGE_NAME);
        exit (EXIT_FAILURE);
      }
      else
        memcpy (& options->member_merged[j], options->member[i], options->dim[i] * sizeof (* options->member_merged));
      
      if (! options->binning[i])
      {
        fprintf (stderr, "fatal: no binning specified.\n"
                         "try '%s --help' for more information\n", PACKAGE_NAME);
        exit (EXIT_FAILURE);
      }
      else
        memcpy (& options->binning_merged[j], options->binning[i], options->dim[i] * sizeof (* options->binning_merged));
      
      if (! options->limit_l[i] || ! options->limit_u[i])
      {
        options->limit_l[i] = malloc (options->dim[i] * sizeof (* options->limit_l[i]));
        options->limit_u[i] = malloc (options->dim[i] * sizeof (* options->limit_u[i]));
        for (k = 0; k < options->dim[i]; k++)
        {
          options->limit_l[i][k] = - DBL_MAX;
          options->limit_u[i][k] = DBL_MAX;
        }
        memcpy (& options->limit_l_merged[j], options->limit_l[i], options->dim[i] * sizeof (* options->limit_l_merged));
        memcpy (& options->limit_u_merged[j], options->limit_u[i], options->dim[i] * sizeof (* options->limit_u_merged));
        
        for (k = 0; k < options->dim[i]; k++)
        {
          options->limit_idl_merged[j + k] = LONG_MIN;
          options->limit_idu_merged[j + k] = LONG_MAX;
        }
        
        fprintf (stderr, "warning: no limits given, assuming infinite interval.\n");
      }
      else
      {
        memcpy (& options->limit_l_merged[j], options->limit_l[i], options->dim[i] * sizeof (* options->limit_l_merged));
        memcpy (& options->limit_u_merged[j], options->limit_u[i], options->dim[i] * sizeof (* options->limit_u_merged));
        
        for (k = 0; k < options->dim[i]; k++)
        {
          l = options->limit_l[i][k];
          u = options->limit_u[i][k];
          if (options->l10[i])
          {
            if (options->l10[i][k] == 1)
            {
              if (l > 0 && u > 0)
              {
                l = log10 (l);
                u = log10 (u);
              }
              else if (l < 0 && u < 0)
              {
                double tmp = l;
                l = log10 (- u);
                u = log10 (- tmp);
              }
              else
              {
                fprintf (stderr, "fatal: limits have different signs or at least one limit is zero.\n"
                                 "try '%s --help' for more information\n", PACKAGE_NAME);
                exit (EXIT_FAILURE);
              }
            }
          }
          options->limit_idl_merged[j + k] = (long int) floor (l / options->binning[i][k]);
          options->limit_idu_merged[j + k] = (long int) floor (u / options->binning[i][k]);
        }
      }
      
      if (! options->l10[i])
      {
        for (k = 0; k < options->dim[i]; k++)
          options->l10_merged[j + k] = 0;
      }
      else
        memcpy (& options->l10_merged[j], options->l10[i], options->dim[i] * sizeof (* options->l10_merged));
      
      j += options->dim[i];
    }
  }
}

void
options_free (
  options_t * const options
)
{
  free (options->input);
  
  size_t ndataset = 0;
  do
  {
    if (options->member[ndataset])
      free (options->member[ndataset]);
    if (options->binning[ndataset])
      free (options->binning[ndataset]);
    if (options->limit_l[ndataset])
      free (options->limit_l[ndataset]);
    if (options->limit_u[ndataset])
      free (options->limit_u[ndataset]);
    if (options->l10[ndataset])
      free (options->l10[ndataset]);
  }
  while (++ndataset < NDATASET_MAX);
  
  free (options->member_merged);
  free (options->binning_merged);
  free (options->limit_l_merged);
  free (options->limit_u_merged);
  free (options->limit_idl_merged);
  free (options->limit_idu_merged);
  free (options->l10_merged);
  
  free (options);
}

void
options_write (
  const options_t * const options,
  const hid_t dset
)
{
  hid_t strtype, space, attr;
  herr_t status;
  hsize_t dims[1] = {options->dim_merged};
  
  strtype = H5Tcopy (H5T_C_S1);
  status = H5Tset_size (strtype, H5T_VARIABLE);
  space = H5Screate_simple (1, dims, NULL);
  attr = H5Acreate (dset, "members", strtype, space, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Awrite (attr, strtype, options->member_merged);
  status = H5Sclose (space);
  status = H5Aclose (attr);
  status = H5Tclose (strtype);
  
  space = H5Screate_simple (1, dims, NULL);
  attr = H5Acreate (dset, "analyzer binning", H5T_IEEE_F64BE, space, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Awrite (attr, H5T_NATIVE_DOUBLE, options->binning_merged);
  status = H5Sclose (space);
  status = H5Aclose (attr);
  
  space = H5Screate_simple (1, dims, NULL);
  attr = H5Acreate (dset, "analyzer lower limit", H5T_IEEE_F64BE, space, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Awrite (attr, H5T_NATIVE_DOUBLE, options->limit_l_merged);
  status = H5Sclose (space);
  status = H5Aclose (attr);
  
  space = H5Screate_simple (1, dims, NULL);
  attr = H5Acreate (dset, "analyzer upper limit", H5T_IEEE_F64BE, space, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Awrite (attr, H5T_NATIVE_DOUBLE, options->limit_u_merged);
  status = H5Sclose (space);
  status = H5Aclose (attr);
  
  space = H5Screate_simple (1, dims, NULL);
  attr = H5Acreate (dset, "analyzer log10", H5T_NATIVE_HBOOL, space, H5P_DEFAULT, H5P_DEFAULT);
  status = H5Awrite (attr, H5T_NATIVE_HBOOL, options->l10_merged);
  status = H5Sclose (space);
  status = H5Aclose (attr);
}

static size_t
countchar (
  const char * const str, const char what
)
{
  size_t i, count = 0;
  for (i = 0; str[i] != '\0'; i++)
    if (str[i] == what)
      count++;
  return count;
}

static int
parse_member (
  char ** const member, char * str, const size_t dim
)
{
  char * str_tok;
  size_t i = 0;
  
  if (strlen (str) == 0)
    return EXIT_FAILURE;
  
  str_tok = strtok (str, ":\0");
  while (str_tok != NULL && i < dim)
  {
    member[i] = str_tok;
    
    ++i;
    str_tok = strtok (NULL, ":\0");
  }
  
  return EXIT_SUCCESS;
}

static int
parse_binning (
  double * const binning, char * str, const size_t dim
)
{
  char * str_tok, * str_end;
  size_t i = 0;
  
  if (strlen (str) == 0)
    return EXIT_FAILURE;
  
  str_tok = strtok (str, ":\0");
  while (str_tok != NULL && i < dim)
  {
    binning[i] = strtod (str_tok, & str_end);
    // str_end points to the character after the last character used in the conversion
    // or no conversion has been performed and str_end equals str_tok
    if (! str_end)
      return EXIT_FAILURE;
    if (! (str_end - str_tok))
      return EXIT_FAILURE;
    
    ++i;
    str_tok = strtok (NULL, ":\0");
  }
  
  return EXIT_SUCCESS;
}

static int
parse_limit (
  double * const limit_l, double * const limit_u, char * str, const size_t dim
)
{
  char * str_tok, * str_end_l, * str_end_u;
  size_t i = 0;
  
  if (strlen (str) == 0)
    return EXIT_FAILURE;
  
  str_tok = strtok (str, ":\0");
  while (str_tok != NULL && i < dim)
  {
    limit_l[i] = strtod (str_tok, & str_end_l);
    // str_end points to the character after the last character used in the conversion
    // or no conversion has been performed and str_end equals str_tok
    if (str_end_l)
    {
      if (! (str_end_l - str_tok))
        limit_l[i] = - DBL_MAX;
      if (* str_end_l == ',')
      {
        limit_u[i] = strtod (str_end_l + 1, & str_end_u);
        if (! str_end_u)
          return EXIT_FAILURE;
        if (! (str_end_u - str_end_l - 1))
          limit_u[i] = DBL_MAX;
      }
      else
        return EXIT_FAILURE;
    }
    else
      return EXIT_FAILURE;
    
    if (limit_l[i] >= limit_u[i])
      return EXIT_FAILURE;
    
    ++i;
    str_tok = strtok (NULL, ":\0");
  }
  
  return EXIT_SUCCESS;
}

static int
parse_l10 (
  hbool_t * const l10, char * str, const size_t dim
)
{
  char * str_tok;
  size_t i = 0;
  
  if (strlen (str) == 0)
    return EXIT_FAILURE;
  
  str_tok = strtok (str, ":\0");
  while (str_tok != NULL && i < dim)
  {
    l10[i] = (hbool_t) strtobool (str_tok);
    
    ++i;
    str_tok = strtok (NULL, ":\0");
  }
  
  return EXIT_SUCCESS;
}

static bool
strtobool (
  const char * str
)
{
  bool retval = false;
  
  if (strncasecmp ("true", str, 4) == 0 || strncasecmp ("yes", str, 3) == 0 || strtol (str, NULL, 10) != 0)
    retval = true;
  
  return retval;
}

static void
print_usage (
  void
)
{
  printf (
    "%s: create multivariate histograms of continuous data\n\n"
    "Usage: %s -d <dsname1> -m <mname1[:mname2...]>\n"
    "  -b <size1[:size2...]> -l <range1[:range2...]>\n"
    "  [-L <boolean1[:boolean2...]>] [-d <dsname2> ...] [-e <number>]\n"
    "  -o <outfile> <infile1> [<infile2> ...]\n\n"
    "Mandatory options:\n"
    "  -d, --dataset <dsname>     data set(s) must be specified first\n"
    "  -m, --member <mname>       data set member(s)\n"
    "  -b, --binning <size>       histogram binning(s)\n"
    "  -l, --limit <range>        histogram limits\n"
    "  -o, --output <outfile>     name the output file\n\n"
    "Optional options:\n"
    "  -e, --save-every <number>  save every <number> of files\n"
    "                             (default: 1)\n"
    "  -L, --l10 <boolean>        logarithmic transform (default: false)\n\n"
    "Other options:\n"
    "  -h, --help                 print this help message and quit\n"
    "  -V, --version              print version information and quit\n\n"
    "Report bugs to: %s\n"
    "%s home page: <%s>\n",
    PACKAGE_NAME, PACKAGE_NAME, PACKAGE_BUGREPORT, PACKAGE_NAME, PACKAGE_URL
  );

  return;
}

static void
print_version (
  void
)
{
  printf (
    "%s-%s\n"
    "Copyright (C) 2015 Torsten Scholak\n",
    PACKAGE_NAME, PACKAGE_VERSION
  );

  return;
}
