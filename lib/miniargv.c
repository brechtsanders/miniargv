#include "miniargv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined(_MSC_VER) || (defined(__MINGW32__) && !defined(__MINGW64__))
#define strcasecmp _stricmp
#endif

DLL_EXPORT_MINIARGV void miniargv_get_version (int* pmajor, int* pminor, int* pmicro)
{
  if (pmajor)
    *pmajor = MINIARGV_VERSION_MAJOR;
  if (pminor)
    *pminor = MINIARGV_VERSION_MINOR;
  if (pmicro)
    *pmicro = MINIARGV_VERSION_MICRO;
}

DLL_EXPORT_MINIARGV const char* miniargv_get_version_string ()
{
  return MINIARGV_VERSION_STRING;
}

DLL_EXPORT_MINIARGV int miniargv_process (int argc, char *argv[], const miniargv_definition argdef[], miniargv_handler_fn defaultfn, miniargv_handler_fn badfn, void* callbackdata)
{
  int i;
  size_t l;
  const miniargv_definition* current_argdef;
  int success;
  for (i = 1; i < argc; i++) {
    success = 0;
    if (argv[i][0] == '-' && argv[i][1]) {
      if (argv[i][1] != '-') {
        //short argument
        current_argdef = argdef;
        while (!success && current_argdef->callbackfn) {
          if (current_argdef->shortarg) {
            if (argv[i][1] == current_argdef->shortarg) {
              if (!current_argdef->argparam) {
                //without value
                if (argv[i][2] == 0) {
                  if ((current_argdef->callbackfn)(current_argdef, NULL, callbackdata) == 0)
                    success++;
                }
              } else if (argv[i][2] != 0) {
                //with value and no space
                if ((current_argdef->callbackfn)(current_argdef, argv[i] + 2, callbackdata) == 0)
                  success++;
              } else if (i + 1 < argc) {
                //with value and space
                if ((current_argdef->callbackfn)(current_argdef, argv[++i], callbackdata) == 0)
                  success++;
              }
            }
          }
          current_argdef++;
        }
      } else {
        //long argument
        current_argdef = argdef;
        while (!success && current_argdef->callbackfn) {
          if (current_argdef->longarg) {
            l = strlen(current_argdef->longarg);
            if (strncmp(argv[i] + 2, current_argdef->longarg, l) == 0 && (argv[i][2 + l] == 0 || argv[i][2 + l] == '=')) {
              if (!current_argdef->argparam) {
                //without value
                if (argv[i][2 + l] == 0) {
                  if ((current_argdef->callbackfn)(current_argdef, NULL, callbackdata) == 0)
                    success++;
                }
              } else if (argv[i][2 + l] == '=') {
                //with value
                if ((current_argdef->callbackfn)(current_argdef, argv[i] + 3 + l, callbackdata) == 0)
                  success++;
              }
            }
          }
          current_argdef++;
        }
      }
    } else {
      if (defaultfn && (defaultfn)(NULL, argv[i], callbackdata) == 0)
        success++;
    }
    if (!success && badfn) {
      if (badfn && (badfn)(NULL, argv[i], callbackdata) == 0)
        success++;
      else
        return i;
    }
    if (!success) {
      fprintf(stderr, "Invalid command line argument: %s\n", argv[i]);
      return i;
    }
  }
  return 0;
}

DLL_EXPORT_MINIARGV const char* miniargv_getprogramname (const char* argv0, int* length)
{
  int pos;
  int len;
  if (!argv0 || !*argv0) {
    if (length)
      *length = 0;
    return NULL;
  }
  //find beginning of filename in path
  len = strlen(argv0);
  pos = len;
  while (pos > 1 && argv0[pos - 1] != '/'
#ifdef _WIN32
    && argv0[pos - 1] != '\\' && argv0[pos - 1] != ':'
#endif
  ) {
    pos--;
  }
  //skip extension
  if (length) {
#ifdef _WIN32
    if (len >= 4 && strcasecmp(argv0 + len - 4, ".exe") == 0)
      len -= 4;
#endif
    len = len - pos;
    *length = len;
  }
  return argv0 + pos;
}

DLL_EXPORT_MINIARGV void miniargv_help (const miniargv_definition argdef[], int descindent, int wrapwidth)
{
  int pos;
  const miniargv_definition* current_argdef = argdef;
  if (!descindent)
    descindent = 25;
  if (!wrapwidth)
    wrapwidth = 79;
  wrapwidth -= descindent;
  while (current_argdef->callbackfn) {

    pos = printf("  ");
    if (current_argdef->shortarg) {
      pos += printf("-%c", current_argdef->shortarg);
      if (current_argdef->argparam)
        pos += printf(" %s", current_argdef->argparam);
    }
    if (current_argdef->longarg) {
      if (current_argdef->shortarg)
        pos += printf(", ");
      pos += printf("--%s", current_argdef->longarg);
      if (current_argdef->argparam)
        pos += printf("=%s", current_argdef->argparam);
    }
    printf("%*s", (pos < descindent ? descindent - pos : 2), "");
    if (wrapwidth <= 0) {
      printf("%s", current_argdef->help);
    } else {
      const char* p;
      const char* q;
      const char* r;
      p = current_argdef->help;
      while (p) {
        q = p;
        r = p;
        do {
          while (*r && !isspace(*r))
            r++;
          if (r - p > wrapwidth) {
            if (q == p)
              q = r;
            break;
          }
          q = r;
          if (*r == '\n')
            break;
          while (*r && isspace(*r))
            r++;
        } while (*r);
        if (!*q) {
          printf("%s", p);
          p = NULL;
        } else {
          printf("%.*s\n%*s", (int)(q - p), p, descindent, "");
          p = q;
          while (isspace(*p))
            p++;
        }
      }
    }
    printf("\n");
    current_argdef++;
  }
}

