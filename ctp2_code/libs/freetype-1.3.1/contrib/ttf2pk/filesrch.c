/*
 *   filesrch.c
 *
 *   This file is part of the ttf2pk package.
 *
 *   Copyright 1997-1999 by
 *     Frederic Loyer <loyer@ensta.fr>
 *     Werner Lemberg <wl@gnu.org>
 */

/*
 *   Interface to the system specific TeX file search routines.
 */

#include <stddef.h>         /* for size_t */
#include <stdlib.h>
#include <string.h>

#include "filesrch.h"
#include "newobj.h"


/****************************
 * kpathsea library support *
 ****************************/

#if defined(HAVE_LIBKPATHSEA)

#ifdef VERY_OLD_KPATHSEA
#include "kpathsea/proginit.h"
#include "kpathsea/progname.h"
#include "kpathsea/tex-glyph.h"
#else
#include "kpathsea/kpathsea.h"
#endif

#ifdef KPSEDLL
/* this is kpathsea 3.3 and newer */
extern KPSEDLL char *kpathsea_version_string;
#else
extern DllImport char *kpathsea_version_string;
#endif

/*
 *   Initialize kpathsea library; arguments are the full name of the
 *   executable (e.g. `/usr/bin/ttf2pk') and two identification strings;
 *   the former for the program itself (to have texmf.cnf constructs like
 *   `TFMFONTS.ttf2pk'), the latter for the environment (to have environment
 *   constructs like `TTF2PKMAKEPK').
 */

void
TeX_search_init(char *exec_name,
                char *program_identifier,
                char *env_identifier)
{
#ifdef OLD_KPATHSEA
  kpse_set_progname(exec_name);
#else
  kpse_set_program_name(exec_name, program_identifier);
#endif

#ifdef VERY_OLD_KPATHSEA
  kpse_init_prog(env_identifier, 300, "cx", true, "cmr10");
#else
  kpse_init_prog(env_identifier, 300, "cx", "cmr10");
#endif
}


char *
TeX_search_version(void)
{
  return kpathsea_version_string;
}


char *
TeX_search_tfm(char **name)
{
  /* no extra extension handling necessary */

  return kpse_find_file(*name, kpse_tfm_format, True);
}


char *
TeX_search_encoding_file(char **name)
{
  handle_extension(name, ".enc");

#ifdef OLD_KPATHSEA
#ifdef VERY_OLD_KPATHSEA
  return kpse_find_file(*name, kpse_dvips_header_format, True);
#else
  return kpse_find_file(*name, kpse_tex_ps_header_format, True);
#endif
#else
  return kpse_find_file(*name, kpse_program_text_format, True);
#endif
}


char *
TeX_search_replacement_file(char **name)
{
  handle_extension(name, ".rpl");

#ifdef OLD_KPATHSEA
#ifdef VERY_OLD_KPATHSEA
  return kpse_find_file(*name, kpse_dvips_header_format, True);
#else
  return kpse_find_file(*name, kpse_tex_ps_header_format, True);
#endif
#else
  return kpse_find_file(*name, kpse_program_text_format, True);
#endif
}


char *
TeX_search_sfd_file(char **name)
{
  handle_extension(name, ".sfd");

#ifdef OLD_KPATHSEA
#ifdef VERY_OLD_KPATHSEA
  return kpse_find_file(*name, kpse_dvips_header_format, True);
#else
  return kpse_find_file(*name, kpse_tex_ps_header_format, True);
#endif
#else
  return kpse_find_file(*name, kpse_program_text_format, True);
#endif
}


char *
TeX_search_config_file(char **name)
{
  /* no extra extension handling necessary */

#ifdef OLD_KPATHSEA
  return kpse_find_file(*name, kpse_dvips_config_format, True);
#else
  return kpse_find_file(*name, kpse_program_text_format, True);
#endif
}


char *
TeX_search_ttf_file(char **name)
{
#ifdef OLD_KPATHSEA

  size_t l;
  char* real_name;


  l = strlen(*name);
  handle_extension(name, ".ttf");
#ifdef VERY_OLD_KPATHSEA
  real_name = kpse_find_file(*name, kpse_dvips_header_format, True);
#else
  real_name = kpse_find_file(*name, kpse_type1_format, True);
#endif

  /* test for .ttc, but only if no extension was given initially */
  if (!real_name && l != strlen(*name))
  {
    (*name)[strlen(*name) - 1] = 'c';
#ifdef VERY_OLD_KPATHSEA
    real_name = kpse_find_file(*name, kpse_dvips_header_format, True);
#else
    real_name = kpse_find_file(*name, kpse_type1_format, True);
#endif
  }

  return real_name;

#else /* OLD_KPATHSEA */

  /* no extra extension handling necessary */

  return kpse_find_file(*name, kpse_truetype_format, True);

#endif
}


/****************************
 * emtexdir library support *
 ****************************/

#elif defined(HAVE_EMTEXDIR)

#include "emtexdir.h"
#include "errormsg.h"

extern int setup_list(struct emtex_dir *dst, char *path,
                      const char *list, unsigned flags);

char emtex_version_string[] = "emTeXdir";

struct emtex_dir tfm_path,
                 enc_path,
                 rpl_path,
                 sfd_path,
                 cfg_path,
                 ttf_path;


/*
 *   We slightly modify emtex_dir_setup() to output a warning in case
 *   the environment variable isn't set properly.
 */

static int
dir_setup(struct emtex_dir *ed,
          const char *env,
          const char *dir,
          unsigned flags)
{
  const char *val;
  char path[260];


  ed->alloc = 0;
  ed->used = 0;
  ed->list = NULL;

  if (env != NULL && (val = getenv(env)) != NULL)
    return setup_list(ed, path, val, flags);
  else
    warning("Environment variable %s not set; use current directory.", env);

  return True;
}


static char *
file_find(char *name, struct emtex_dir *list)
{
  char buffer[1024];


  if (emtex_dir_find(buffer, sizeof (buffer), list, name, EDF_CWD))
    return newstring(buffer);

  return NULL;
}


/*
 *   Initialize emtexdir library; arguments are the full name of the
 *   executable (e.g. `c:\bin\ttf2pk.exe') and two identification strings;
 *   the former for the program itself, the latter for the environment.
 *   We ignore all of them.
 */

void
TeX_search_init(char *exec_name,
                char *program_identifier,
                char *env_identifier)
{
  if (!dir_setup(&tfm_path, "TEXTFM", NULL, EDS_BANG))
    oops("Cannot setup search path for tfm files");
  if (!dir_setup(&enc_path, "TTFCFG", NULL, EDS_BANG))
    oops("Cannot setup search path for encoding files");
#if 0
  if (!dir_setup(&rpl_path, "TTFCFG", NULL, EDS_BANG))
    oops("Cannot setup search path for replacement files");
  if (!dir_setup(&sfd_path, "TTFCFG", NULL, EDS_BANG))
    oops("Cannot setup search path for subfont definition files");
  if (!dir_setup(&cfg_path, "TTFCFG", NULL, EDS_BANG))
    oops("Cannot setup search path for configuration file");
#else
  rpl_path = sfd_path = cfg_path = enc_path;
#endif
  if (!dir_setup(&ttf_path, "TTFONTS", NULL, EDS_BANG))
    oops("Cannot setup search path for TrueType font files");
}


char *
TeX_search_version(void)
{
  return emtex_version_string;
}


char *
TeX_search_tfm(char **name)
{
  handle_extension(name, ".tfm");
  return file_find(*name, &tfm_path);
}


char *
TeX_search_encoding_file(char **name)
{
  handle_extension(name, ".enc");
  return file_find(*name, &enc_path);
}


char *
TeX_search_replacement_file(char **name)
{
  handle_extension(name, ".rpl");
  return file_find(*name, &rpl_path);
}


char *
TeX_search_sfd_file(char **name)
{
  handle_extension(name, ".sfd");
  return file_find(*name, &sfd_path);
}


char *
TeX_search_config_file(char **name)
{
  /* no extra extension handling necessary */
  return file_find(*name, &cfg_path);
}


char *
TeX_search_ttf_file(char **name)
{
  size_t l;
  char* real_name;


  l = strlen(*name);
  handle_extension(name, ".ttf");
  real_name = file_find(*name, &ttf_path);

  /* test for .ttc, but only if no extension was given initially */
  if (!real_name && l != strlen(*name))
  {
    (*name)[strlen(*name) - 1] = 'c';
    real_name = file_find(*name, &ttf_path);
  }

  return real_name;
}


/**************************
 * MiKTeX library support *
 **************************/

#elif defined(MIKTEX)

#include "miktex.h"

void
TeX_search_init(char *exec_name,
                char *program_identifier,
                char *env_identifier)
{
  /* empty */
}


char *
TeX_search_version(void)
{
  char buf[200];


  strcpy(buf, "MiKTeX ");
  miktex_get_miktex_version_string_ex(buf + 7, sizeof (buf) - 7);

  return buf;
}


char *
TeX_search_tfm(char **name)
{
  char result[_MAX_PATH];


  if (!miktex_find_tfm_file(*name, result))
    return 0;

  return strdup(result);
}


char *
TeX_search_encoding_file(char **name)
{
  char result[_MAX_PATH];


  if (!miktex_find_enc_file(*name, result))
    return 0;

  return strdup(result);
}


char *
TeX_search_replacement_file(char **name)
{
  char result[_MAX_PATH];


  handle_extension(name, ".rpl");
  if (!miktex_find_input_file("ttf2tfm", *name, result))
    return 0;

  return strdup(result);
}


char *
TeX_search_sfd_file(char **name)
{
  char result[_MAX_PATH];


  handle_extension(name, ".sfd");
  if (!miktex_find_input_file("ttf2tfm", *name, result))
    return 0;
  return strdup(result);
}


char *
TeX_search_config_file(char **name)
{
  char result[_MAX_PATH];


  if (!miktex_find_input_file("ttf2tfm", *name, result))
    return 0;
  return strdup(result);
}


char *
TeX_search_ttf_file(char **name)
{
  char result[_MAX_PATH];


  if (!miktex_find_ttf_file(*name, result))
    return 0;
  return strdup(result);
}


/**********************
 * no library support *
 **********************/

#else

#include <stdio.h>

char version_string[] = "no search library";

void
TeX_search_init(char *exec_name,
                char *program_identifier,
                char *env_identifier)
{
  /* empty */
}


char *
TeX_search_version(void)
{
  return version_string;
}


char *
TeX_search_tfm(char **name)
{
  handle_extension(name, ".tfm");
  return *name;
}


char *
TeX_search_encoding_file(char **name)
{
  handle_extension(name, ".enc");
  return *name;
}


char *
TeX_search_replacement_file(char **name)
{
  handle_extension(name, ".rpl");
  return *name;
}


char *
TeX_search_sfd_file(char **name)
{
  handle_extension(name, ".sfd");
  return *name;
}


char *
TeX_search_config_file(char **name)
{
  /* no extra extension handling necessary */
  return *name;
}


/* we only handle .ttf extension */

char *
TeX_search_ttf_file(char **name)
{
  handle_extension(name, ".ttf");
  return *name;
}

#endif



void
get_tfm_fullname(Font *fnt)
{
  size_t len = 0;


  if (fnt->fullname)
    free(fnt->fullname);

  if (fnt->outname)
    len += strlen(fnt->outname);
  if (fnt->subfont_name)
    len += strlen(fnt->subfont_name);
  if (fnt->outname_postfix)
    len += strlen(fnt->outname_postfix);
  len++;
  
  fnt->fullname = (char *)mymalloc(len);
  fnt->fullname[0] = '\0';

  if (fnt->outname)
    strcat(fnt->fullname, fnt->outname);
  if (fnt->subfont_name)
    strcat(fnt->fullname, fnt->subfont_name);
  if (fnt->outname_postfix)
    strcat(fnt->fullname, fnt->outname_postfix);
}


/*
 *   This function takes the address of a pointer to a string allocated
 *   with malloc() and checks whether it has an extension.  If not, a default
 *   extension given as a second argument will be appended using first
 *   realloc() and then strcat().
 *
 *   '/', ':', and '\\' will be recognized as directory separators.
 */

void
handle_extension(char **stringp,
                 char *extension)
{
  int i, lastext = -1;


  for (i = 0; (*stringp)[i]; i++)
    if ((*stringp)[i] == '.')
      lastext = i;
    else if ((*stringp)[i] == '/' ||
             (*stringp)[i] == ':' ||
             (*stringp)[i] == '\\')
      lastext = -1;

  if (lastext == -1)
  {
    *stringp = (char *)myrealloc((void *)*stringp,
                                 strlen(*stringp) + strlen(extension) + 1);
    strcat(*stringp, extension);
  }
}


/* end */
