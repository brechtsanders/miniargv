#include "miniargv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined(_MSC_VER) || (defined(__MINGW32__) && !defined(__MINGW64__))
#define strcasecmp _stricmp
#endif

#define MINIARG_PROCESS_FLAG_FLAGS  0x01
#define MINIARG_PROCESS_FLAG_VALUES 0x02
#define MINIARG_PROCESS_FLAG_BOTH   0x03

int miniargv_process_partial (unsigned int flags, int argc, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  int i;
  size_t l;
  const miniargv_definition* current_argdef;
  int success;
  int standalonevaluedefset = 0;
  const miniargv_definition* standalonevaluedef = NULL;
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
                  if ((flags & MINIARG_PROCESS_FLAG_FLAGS) == 0)
                    success++;
                  else if ((current_argdef->callbackfn)(current_argdef, NULL, callbackdata) == 0)
                    success++;
                }
              } else if (argv[i][2] != 0) {
                //with value and no space
                if ((flags & MINIARG_PROCESS_FLAG_FLAGS) == 0)
                  success++;
                else if ((current_argdef->callbackfn)(current_argdef, argv[i] + 2, callbackdata) == 0)
                  success++;
              } else if (i + 1 < argc) {
                //with value and space
                i++;
                if ((flags & MINIARG_PROCESS_FLAG_FLAGS) == 0)
                  success++;
                else if ((current_argdef->callbackfn)(current_argdef, argv[i], callbackdata) == 0)
                  success++;
              }
              break;
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
                  if ((flags & MINIARG_PROCESS_FLAG_FLAGS) == 0)
                    success++;
                  else if ((current_argdef->callbackfn)(current_argdef, NULL, callbackdata) == 0)
                    success++;
                }
              } else if (argv[i][2 + l] == '=') {
                //with value
                if ((flags & MINIARG_PROCESS_FLAG_FLAGS) == 0)
                  success++;
                else if ((current_argdef->callbackfn)(current_argdef, argv[i] + 3 + l, callbackdata) == 0)
                  success++;
              }
              break;
            }
          }
          current_argdef++;
        }
      }
    } else {
      //standalone value argument
      if (!standalonevaluedefset) {
        current_argdef = argdef;
        while (!success && current_argdef->callbackfn) {
          if (!current_argdef->shortarg && !current_argdef->longarg) {
            standalonevaluedef = current_argdef;
            break;
          }
          current_argdef++;
        }
        standalonevaluedefset = 1;
      }
      if (standalonevaluedef && standalonevaluedef->callbackfn) {
        if ((flags & MINIARG_PROCESS_FLAG_VALUES) == 0)
          success++;
        else if ((standalonevaluedef->callbackfn)(standalonevaluedef, argv[i], callbackdata) == 0)
          success++;
      }
    }
    if (!success && badfn) {
      //bad argument
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

DLL_EXPORT_MINIARGV int miniargv_process (int argc, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  return miniargv_process_partial(MINIARG_PROCESS_FLAG_BOTH, argc, argv, argdef, badfn, callbackdata);
}

DLL_EXPORT_MINIARGV int miniargv_process_flags_only (int argc, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  return miniargv_process_partial(MINIARG_PROCESS_FLAG_FLAGS, argc, argv, argdef, badfn, callbackdata);
}

DLL_EXPORT_MINIARGV int miniargv_process_skip_flags (int argc, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  return miniargv_process_partial(MINIARG_PROCESS_FLAG_VALUES, argc, argv, argdef, badfn, callbackdata);
}

DLL_EXPORT_MINIARGV int miniargv_process_environment (const char* env[], const miniargv_definition envdef[], void* callbackdata)
{
  const char* s;
  const char** current_env;
  const miniargv_definition* current_envdef = envdef;
  int success = 0;
  while (!success && current_envdef->callbackfn) {
    if (current_envdef->longarg) {
      current_env = env;
      while (*current_env) {
        if ((s = strchr(*current_env, '=')) != NULL) {
          if (strncmp(*current_env, current_envdef->longarg, s - *current_env) == 0) {
            if ((current_envdef->callbackfn)(current_envdef, s + 1, callbackdata) == 0)
              success++;
          }
        }
        current_env++;
      }
    }
    current_envdef++;
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

DLL_EXPORT_MINIARGV void miniargv_list_args (const miniargv_definition argdef[], int shortonly)
{
  const miniargv_definition* current_argdef = argdef;
  while (current_argdef->callbackfn) {
    if (argdef != current_argdef)
      printf(" ");
    if (current_argdef->shortarg  || current_argdef->longarg) {
      printf("[");
      if (current_argdef->shortarg) {
        printf("-%c", current_argdef->shortarg);
        if (current_argdef->argparam)
          printf(" %s", current_argdef->argparam);
      }
      if (current_argdef->longarg && !(shortonly && current_argdef->shortarg)) {
        if (current_argdef->shortarg)
          printf("|");
        printf("--%s", current_argdef->longarg);
        if (current_argdef->argparam)
          printf("=%s", current_argdef->argparam);
      }
      printf("]");
    } else {
      printf("%s", (current_argdef->argparam ? current_argdef->argparam : "param"));
    }
    current_argdef++;
  }
}

void indent_and_wrap_text (const char* text, int currentpos, int descindent, int wrapwidth)
{
  const char* p;
  const char* q;
  const char* r;
  p = text;
  while (p) {
    q = p;
    r = p;
    do {
      while (*r && !isspace(*r))
        r++;
      if (r - p + currentpos > wrapwidth) {
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
      currentpos = descindent;
      p = q;
      while (isspace(*p))
        p++;
    }
  }
}

DLL_EXPORT_MINIARGV void miniargv_help (const miniargv_definition argdef[], int descindent, int wrapwidth)
{
  int pos;
  const miniargv_definition* current_argdef = argdef;
  if (!descindent)
    descindent = 25;
  if (!wrapwidth)
    wrapwidth = 79;
  while (current_argdef->callbackfn) {
    pos = printf("  ");
    if (!current_argdef->shortarg && !current_argdef->longarg) {
      pos += printf(" %s", (current_argdef->argparam ? current_argdef->argparam : "param"));
    } else {
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
    }
    printf("%*s", (pos < descindent ? descindent - pos : 2), "");
    indent_and_wrap_text(current_argdef->help, descindent, descindent, wrapwidth);
    printf("\n");
    current_argdef++;
  }
}

DLL_EXPORT_MINIARGV void miniargv_environment_help (const miniargv_definition envdef[], int descindent, int wrapwidth)
{
  int pos;
  const miniargv_definition* current_envdef = envdef;
  if (!descindent)
    descindent = 25;
  if (!wrapwidth)
    wrapwidth = 79;
  while (current_envdef->callbackfn) {
    pos = printf("  ");
    if (!current_envdef->shortarg && !current_envdef->longarg) {
      pos += printf(" %s", (current_envdef->argparam ? current_envdef->argparam : "param"));
    } else {
      if (current_envdef->longarg) {
        if (current_envdef->shortarg)
          pos += printf(", ");
        pos += printf("%s", current_envdef->longarg);
        if (current_envdef->argparam)
          pos += printf("=%s", current_envdef->argparam);
      }
    }
    printf("%*s", (pos < descindent ? descindent - pos : 2), "");
    indent_and_wrap_text(current_envdef->help, descindent, descindent, wrapwidth);
    printf("\n");
    current_envdef++;
  }
}

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

