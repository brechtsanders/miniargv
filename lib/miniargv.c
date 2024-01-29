#include "miniargv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stddef.h>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif
#if defined(_MSC_VER) || (defined(__MINGW32__) && !defined(__MINGW64__))
#define strcasecmp _stricmp
#endif
//extern char** environ;

#define MINIARG_PROCESS_MASK_FLAGS      0x01
#define MINIARG_PROCESS_MASK_VALUES     0x02
#define MINIARG_PROCESS_MASK_BOTH       (MINIARG_PROCESS_MASK_FLAGS | MINIARG_PROCESS_MASK_VALUES)
#define MINIARG_PROCESS_MASK_FIND_ONLY  0x08
#define MINIARG_PROCESS_MASK_FIND_VALUE (MINIARG_PROCESS_MASK_FIND_ONLY | MINIARG_PROCESS_MASK_VALUES)

/* maximum levels of include in configuration files, to protect against infinite recursion */
#define MINIARGV_CFG_MAX_INCLUDE_DEPTH  32

DLL_EXPORT_MINIARGV int miniargv_handler_ignore (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  return 0;
}

/* process single command line argument, returns non-zero if argument was processed */
int miniargv_process_partial_single_arg (int* index, int* success, unsigned int flags, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  size_t l;
  const char* arg;
  const miniargv_definition* current_argdef;
  (*success) = 0;
  if (argv[*index][0] == '-' && argv[*index][1]) {
    if (argv[*index][1] != '-') {
      //find short argument in argument definitions
      if ((current_argdef = miniargv_find_shortarg(argv[*index][1], argdef)) != NULL) {
        if (!current_argdef->argparam) {
          //without value
          if (argv[*index][2] == 0) {
/*
            //abort with error if not looking for flags
            if ((flags & MINIARG_PROCESS_MASK_FLAGS) == 0)
              return 1;
*/
            //if only looking for flag return index
            if ((flags & MINIARG_PROCESS_MASK_FIND_ONLY) != 0) {
              if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
                (*success)++;
                return *index;
              }
            } else
            //process flag by calling callback function
            if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
              if ((current_argdef->callbackfn)(current_argdef, NULL, callbackdata) == 0)
                (*success)++;
            } else {
              (*success)++;
            }
          }
        } else if (argv[*index][2] != 0) {
          //with value and no space
/*
          //abort with error if not looking for flags
          if ((flags & MINIARG_PROCESS_MASK_FLAGS) == 0)
            return 1;
*/
          //if only looking for flag return index
          if ((flags & MINIARG_PROCESS_MASK_FIND_ONLY) != 0) {
            if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
              (*success)++;
              return *index;
            }
          } else
          //process flag by calling callback function
          if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
            if ((current_argdef->callbackfn)(current_argdef, argv[*index] + 2, callbackdata) == 0)
              (*success)++;
          } else {
            (*success)++;
          }
        } else if (argv[*index + 1]) {
          //with value and space
          (*index)++;
/*
          //abort with error if not looking for flags
          if ((flags & MINIARG_PROCESS_MASK_FLAGS) == 0)
            return 1;
*/
          //if only looking for flag return index
          if ((flags & MINIARG_PROCESS_MASK_FIND_ONLY) != 0) {
            if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
              (*success)++;
              return *index;
            }
          } else
          //process flag by calling callback function
          if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
            if ((current_argdef->callbackfn)(current_argdef, argv[*index], callbackdata) == 0)
              (*success)++;
          } else {
            (*success)++;
          }
        }
        return 1;
      }
    } else {
      //find long argument in argument definitions
      l = 0;
      arg = argv[*index] + 2;
      while (arg[l] && arg[l] != '=')
        l++;
      if ((current_argdef = miniargv_find_longarg(arg, l, argdef)) != NULL) {
        if (!current_argdef->argparam) {
          //without value
          if (arg[l] == 0) {
/*
            //abort with error if not looking for flags
            if ((flags & MINIARG_PROCESS_MASK_FLAGS) == 0)
              return 2;
*/
            //if only looking for flag return index
            if ((flags & MINIARG_PROCESS_MASK_FIND_ONLY) != 0) {
              if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
                (*success)++;
                return *index;
              }
            } else
            //process flag by calling callback function
            if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
              if ((current_argdef->callbackfn)(current_argdef, NULL, callbackdata) == 0)
                (*success)++;
            } else {
              (*success)++;
            }
          }
        } else if (arg[l] == '=') {
          //with value
/*
          //abort with error if not looking for flags
          if ((flags & MINIARG_PROCESS_MASK_FLAGS) == 0)
            return 2;
*/
          //if only looking for flag return index
          if ((flags & MINIARG_PROCESS_MASK_FIND_ONLY) != 0) {
            if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
              (*success)++;
              return *index;
            }
          } else
          //process flag by calling callback function
          if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
            if ((current_argdef->callbackfn)(current_argdef, argv[*index] + 3 + l, callbackdata) == 0)
              (*success)++;
          } else {
            (*success)++;
          }
        } else if (argv[*index + 1]) {
          //with value and space
          (*index)++;
/*
          //abort with error if not looking for flags
          if ((flags & MINIARG_PROCESS_MASK_FLAGS) == 0)
            return 1;
*/
          //if only looking for flag return index
          if ((flags & MINIARG_PROCESS_MASK_FIND_ONLY) != 0) {
            if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
              (*success)++;
              return *index;
            }
          } else
          //process flag by calling callback function
          if ((flags & MINIARG_PROCESS_MASK_FLAGS) != 0) {
            if ((current_argdef->callbackfn)(current_argdef, argv[*index], callbackdata) == 0)
              (*success)++;
          } else {
            (*success)++;
          }
        }
        return 2;
      }
    }
  } else {
    //standalone value argument
    if ((current_argdef = miniargv_find_standalonearg(argdef)) != NULL) {
      //standalone value argument definition found
      (*success)++;
      if (current_argdef->callbackfn) {
/*
        //abort with error if not looking for value arguments
        if ((flags & MINIARG_PROCESS_MASK_VALUES) == 0)
          return 3;
*/
        //if only looking for standalone value argument return index
        if ((flags & MINIARG_PROCESS_MASK_FIND_ONLY) != 0) {
          if ((flags & MINIARG_PROCESS_MASK_VALUES) != 0) {
            (*success)++;
            return *index;
          }
        } else
        //process standalone value argument by calling callback function
        if ((flags & MINIARG_PROCESS_MASK_VALUES) != 0) {
          if ((current_argdef->callbackfn)(current_argdef, argv[*index], callbackdata) == 0)
            (*success)++;
        } else {
          (*success)++;
        }
      }
      return 3;
    }
  }
  return 0;
}

/* partially process argv */
int miniargv_process_partial (unsigned int flags, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  int i;
  int success;
  for (i = ((flags & MINIARG_PROCESS_MASK_FIND_ONLY) == 0 ? 1 : *(int*)callbackdata + 1); argv[i]; i++) {
    miniargv_process_partial_single_arg(&i, &success, flags, argv, argdef, badfn, callbackdata);
    if (success && (flags & MINIARG_PROCESS_MASK_FIND_ONLY) != 0) {
      return i;
    }
    if (!success && badfn) {
      //bad argument
      if (badfn && (badfn)(NULL, argv[i], callbackdata) == 0)
        success++;
      else if ((flags & MINIARG_PROCESS_MASK_FIND_ONLY) != 0)
        return -1;
      else
        return i;
    }
    if (!success) {
      if ((flags & MINIARG_PROCESS_MASK_FIND_ONLY) != 0)
        continue;
      fprintf(stderr, "Invalid command line argument: %s\n", argv[i]);
      return i;
    }
  }
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_process (char* argv[], char* env[], const miniargv_definition argdef[], const miniargv_definition envdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  int result = 0;
  if (env)
    result = miniargv_process_env(env, envdef, callbackdata);
  if (argv) {
    if (result == 0)
      result = miniargv_process_partial(MINIARG_PROCESS_MASK_FLAGS, argv, argdef, badfn, callbackdata);
    if (result == 0)
      result = miniargv_process_partial(MINIARG_PROCESS_MASK_VALUES, argv, argdef, badfn, callbackdata);
  }
  return result;
}

DLL_EXPORT_MINIARGV int miniargv_process_ltr (char* argv[], char* env[], const miniargv_definition argdef[], const miniargv_definition envdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  int result = 0;
  if (env)
    result = miniargv_process_env(env, envdef, callbackdata);
  if (result == 0 && argv)
    result = miniargv_process_arg(argv, argdef, badfn, callbackdata);
  return result;
}

DLL_EXPORT_MINIARGV int miniargv_process_arg (char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  return miniargv_process_partial(MINIARG_PROCESS_MASK_BOTH, argv, argdef, badfn, callbackdata);
}

DLL_EXPORT_MINIARGV int miniargv_process_arg_flags (char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  return miniargv_process_partial(MINIARG_PROCESS_MASK_FLAGS, argv, argdef, badfn, callbackdata);
}

DLL_EXPORT_MINIARGV int miniargv_process_arg_params (char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  return miniargv_process_partial(MINIARG_PROCESS_MASK_VALUES, argv, argdef, badfn, callbackdata);
}

DLL_EXPORT_MINIARGV int miniargv_get_next_arg_param (int argindex, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn)
{
  return miniargv_process_partial(MINIARG_PROCESS_MASK_FIND_VALUE, argv, argdef, badfn, &argindex);
}

DLL_EXPORT_MINIARGV int miniargv_process_env (char* env[], const miniargv_definition envdef[], void* callbackdata)
{
  char* s;
  char** current_env;
  const miniargv_definition* current_envdef = envdef;
  int result;
  if (current_envdef) {
    while (current_envdef->callbackfn) {
      if (current_envdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
        if ((result = miniargv_process_env(env, (struct miniargv_definition_struct*)(current_envdef->callbackfn), callbackdata)) != 0)
          return result;
      } else if (current_envdef->longarg) {
        current_env = env;
        while (*current_env) {
          if ((s = strchr(*current_env, '=')) != NULL) {
            if (strncmp(*current_env, current_envdef->longarg, s - *current_env) == 0) {
              if ((result = (current_envdef->callbackfn)(current_envdef, s + 1, callbackdata)) != 0)
                return result;
            }
          }
          current_env++;
        }
      }
      current_envdef++;
    }
  }
  return 0;
}

//initial block size and increment steps for reading/allocating line data
#define MINIARGV_READLINE_BLOCK_SIZE 128

//read line from file, NULL when no more lines, caller must call free()
char* miniargv_readline (FILE* src)
{
  int datalen;
  char data[MINIARGV_READLINE_BLOCK_SIZE];
  int resultlen = 0;
  char* result = NULL;
  //read next data
  while (fgets(data, sizeof(data), src)) {
    datalen = strlen(data);
    //allocate memory and store the result
    result = (char*)realloc(result, resultlen + datalen + 1);
    memcpy(result + resultlen, data, datalen + 1);
    resultlen += datalen;
    //check for line end, if found remove it and return result
    if (resultlen > 0 && result[resultlen - 1] == '\n') {
      result[--resultlen] = 0;
      if (resultlen > 0 && result[resultlen - 1] == '\r')
        result[--resultlen] = 0;
      break;
    }
  }
  return result;
}

void set_section_name (char** section, const char* name, size_t namelen)
{
  if (*section)
    free(*section);
  if (name == NULL || namelen == 0 || (namelen == 7 && strncmp(name, "default", 7) == 0)) {
    *section = NULL;
  } else {
    if ((*section = (char*)malloc(namelen + 1)) != NULL) {
      int i;
      char* p = *section;
      for (i = 0; i < namelen; i++) {
        *p++ = tolower(name[i]);
      }
      //memcpy(*section, name, namelen);
      (*section)[namelen] = 0;
    }
  }
}

int process_cfgfile (const char* cfgfile, const miniargv_definition cfgdef[], const char* loadsection, const char* currentsection, size_t maxincludedepth, void* callbackdata)
{
  FILE* src;
  char* line;
  char* p;
  char* varname;
  size_t varnamelen;
  char separator;
  char* value;
  const miniargv_definition* current_cfgdef;
  char* load_section = NULL;
  char* current_section = NULL;
  int status = 0;
  set_section_name(&load_section, loadsection, (loadsection ? strlen(loadsection) : 0));
  set_section_name(&current_section, currentsection, (currentsection ? strlen(currentsection) : 0));
  //open file for reading
  if ((src = fopen(cfgfile, "rb")) != NULL) {
    //read lines
    while (status == 0 && (line = miniargv_readline(src)) != NULL) {
      varname = line;
      //skip spaces preceding varname
      while (*varname && isspace(*varname))
        varname++;
      if (*varname == '[') {
        if ((p = strchr(++varname, ']')) != NULL)  {
          set_section_name(&current_section, varname, p - varname);
        }
      }
      //only process default section or selected selection
      if (!current_section || (load_section && strcmp(current_section, load_section) == 0)) {
        //include specified file if line starts with @
        if (*varname == '@') {
          //skip spaces preceding include file
          varname++;
          while (*varname && isspace(*varname))
            varname++;
          //skip spaces following include file
          if ((p = strchr(varname, 0)) != NULL) {
            while (p != varname && isspace(*(p - 1)))
              p--;
            *p = 0;
          }
          if (*varname) {
            if (maxincludedepth > 0)
              status = process_cfgfile(varname, cfgdef, loadsection, current_section, maxincludedepth - 1, callbackdata);
            else
              status = -1;
          }
        } else if (*varname) {
          //find starting position of value
          p = varname;
          while (*p && *p != '=' && *p != ':' && *p != '@' && *p != '#' && *p != ';')
            p++;
          separator = *p;
          if (separator == '=' || separator == ':' || separator == '@') {
            value = p + 1;
            //skip spaces following varname
            while (p != varname && isspace(*(p - 1)))
              p--;
            if (p != varname) {
              varnamelen = p - varname;
              //skip spaces preceding value
              while (*value && isspace(*value))
                value++;
              //skip spaces following value
              if ((p = strchr(value, 0)) != NULL) {
                while (p != value && isspace(*(p - 1)))
                  p--;
                *p = 0;
              }
              //process variable
              if ((current_cfgdef = miniargv_find_longarg(varname, varnamelen, cfgdef)) != NULL) {
                if (separator == '@') {
                  //process contents of another file
                  FILE* valuesrc;
                  int datalen;
                  char data[MINIARGV_READLINE_BLOCK_SIZE];
                  int loadedvaluelen = 0;
                  char* loadedvalue = NULL;
                  if ((valuesrc = fopen(value, "rb")) != NULL) {
                    //read next data
                    while ((datalen = fread(data, 1, sizeof(data), valuesrc)) > 0) {
                      //allocate memory and store the result
                      if ((loadedvalue = (char*)realloc(loadedvalue, loadedvaluelen + datalen + 1)) == NULL)
                        break;
                      memcpy(loadedvalue + loadedvaluelen, data, datalen + 1);
                      loadedvaluelen += datalen;
                    }
                    fclose(valuesrc);
                    if (loadedvalue) {
                      loadedvalue[loadedvaluelen] = 0;
                      status = (current_cfgdef->callbackfn)(current_cfgdef, loadedvalue, callbackdata);
                      free(loadedvalue);
                    }
                  }
                } else {
                  //process variable value
                  status = (current_cfgdef->callbackfn)(current_cfgdef, value, callbackdata);
                }
              } else {
                //variable name not found
                //printf("Error: unknown variable: %.*s\n", (int)varnamelen, varname);/////
              }
            }
          }
        }
      }
      free(line);
    }
    fclose(src);
  }
  if (load_section)
    free(load_section);
  if (current_section)
    free(current_section);
  return status;
}

DLL_EXPORT_MINIARGV int miniargv_process_cfgfile (const char* cfgfile, const miniargv_definition cfgdef[], const char* section, void* callbackdata)
{
  if (!cfgfile || !*cfgfile)
    return -1;
  return process_cfgfile(cfgfile, cfgdef, section, NULL, MINIARGV_CFG_MAX_INCLUDE_DEPTH, callbackdata);
}

DLL_EXPORT_MINIARGV void miniargv_cfgfile_generate (FILE* cfgfile, const miniargv_definition cfgdef[])
{
  const miniargv_definition* current_cfgdef = cfgdef;
  static const char* help_indent = "\n;   ";
  while (current_cfgdef->callbackfn) {
    if (current_cfgdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
      miniargv_cfgfile_generate(cfgfile, (struct miniargv_definition_struct*)(current_cfgdef->callbackfn));
    } else if (current_cfgdef->longarg) {
      fprintf(cfgfile, "; %s:%s", current_cfgdef->longarg, help_indent);
      miniargv_wrap_and_indent_text(cfgfile, current_cfgdef->help, 0, 0, 79 - 4, help_indent);
      fprintf(cfgfile, "\n%s = %s\n\n", current_cfgdef->longarg, (current_cfgdef->argparam ? current_cfgdef->argparam : ""));
    }
    current_cfgdef++;
  }
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
  while (pos > 0 && argv0[pos - 1] != '/'
#ifdef _WIN32
    && argv0[pos - 1] != '\\' && argv0[pos - 1] != ':'
#endif
  ) {
    pos--;
  }
  //skip extension
  if (length) {
#ifdef _WIN32
    if (len && argv0[len] == '\r')
      len--;
    if (len >= 4 && strcasecmp(argv0 + len - 4, ".exe") == 0)
      len -= 4;
#endif
    len = len - pos;
    *length = len;
  }
  return argv0 + pos;
}

DLL_EXPORT_MINIARGV unsigned int miniargv_arg_list (const miniargv_definition argdef[], int shortonly)
{
  unsigned int count = 0;
  const miniargv_definition* current_argdef = argdef;
  while (current_argdef->callbackfn) {
    if (count > 0)
      printf(" ");
    if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
      //note: if the next command prints nothing and it's the last entry there will be an extra space at the end
      count += miniargv_arg_list((struct miniargv_definition_struct*)(current_argdef->callbackfn), shortonly);
    } else {
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
      count++;
    }
    current_argdef++;
  }
  return count;
}

DLL_EXPORT_MINIARGV unsigned int miniargv_env_list (const miniargv_definition envdef[], int noparam)
{
  unsigned int count = 0;
  const miniargv_definition* current_envdef = envdef;
  while (current_envdef->callbackfn) {
    if (count > 0)
      printf(" ");
    if (current_envdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
      //note: if the next command prints nothing and it's the last entry there will be an extra space at the end
      count += miniargv_env_list((struct miniargv_definition_struct*)(current_envdef->callbackfn), noparam);
    } else {
      if (current_envdef->longarg) {
        printf("%s", current_envdef->longarg);
        if (!noparam && current_envdef->argparam)
          printf("=%s", current_envdef->argparam);
        count++;
      }
    }
    current_envdef++;
  }
  return count;
}

DLL_EXPORT_MINIARGV void miniargv_arg_help (const miniargv_definition argdef[], int descindent, int wrapwidth)
{
  int pos;
  const miniargv_definition* current_argdef = argdef;
  if (!descindent)
    descindent = 25;
  while (current_argdef->callbackfn) {
    if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
      //note: if the next command prints nothing and it's the last entry there will be an extra space at the end
      miniargv_arg_help((struct miniargv_definition_struct*)(current_argdef->callbackfn), descindent, wrapwidth);
    } else {
      pos = printf("  ");
      if (!current_argdef->shortarg && !current_argdef->longarg) {
        pos += printf("%s", (current_argdef->argparam ? current_argdef->argparam : "param"));
      } else {
        if (current_argdef->shortarg) {
          pos += printf("-%c", current_argdef->shortarg);
          if (current_argdef->argparam && !current_argdef->longarg)
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
      if (pos > descindent - 2)
        printf("\n%*s", descindent, "");
      else
        printf("%*s", (pos < descindent ? descindent - pos : 2), "");
      miniargv_wrap_and_indent_text(stdout, current_argdef->help, descindent, descindent, wrapwidth, NULL);
      printf("\n");
    }
    current_argdef++;
  }
}

DLL_EXPORT_MINIARGV void miniargv_env_help (const miniargv_definition envdef[], int descindent, int wrapwidth)
{
  int pos;
  const miniargv_definition* current_envdef = envdef;
  if (!descindent)
    descindent = 25;
  while (current_envdef->callbackfn) {
    if (current_envdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
      //note: if the next command prints nothing and it's the last entry there will be an extra space at the end
      miniargv_env_help((struct miniargv_definition_struct*)(current_envdef->callbackfn), descindent, wrapwidth);
    } else {
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
      if (pos > descindent - 2)
        printf("\n%*s", descindent, "");
      else
        printf("%*s", (pos < descindent ? descindent - pos : 2), "");
      miniargv_wrap_and_indent_text(stdout, current_envdef->help, descindent, descindent, wrapwidth, NULL);
      printf("\n");
    }
    current_envdef++;
  }
}

DLL_EXPORT_MINIARGV void miniargv_help (const miniargv_definition argdef[], const miniargv_definition envdef[], int descindent, int wrapwidth)
{
  if (argdef) {
    printf("Command line arguments:\n");
    miniargv_arg_help(argdef, descindent, wrapwidth);
  }
  if (envdef) {
    printf("Environment variables:\n");
    miniargv_env_help(envdef, descindent, wrapwidth);
  }
}

/* define COMPLETE_ADD_SPACE if bash completion is configured via "complete -o nospace -C<path> <command>" */
//#define COMPLETE_ADD_SPACE

const miniargv_definition* miniargv_complete_arg (char *argv[], char* env[], int index, const miniargv_definition argdef[], const miniargv_definition envdef[], void* callbackdata)
{
  char* partialargend;
  size_t partialarglen;
  const miniargv_definition* current_argdef;
  const miniargv_definition* result;
  const char* partialarg;
  const char* previousarg;
  const miniargv_definition* last_argdef = NULL;
  int multipleresults = 0;
  if ((partialarg = argv[index]) == NULL || (previousarg = argv[index + 1]) == NULL)
    return NULL;
  //on Windows set console output to binary mode (to avoid showing ^M in bash completion output)
#ifdef _WIN32
  setmode(fileno(stdout), O_BINARY);
#endif
/*
  //check if previous argument is reported as just the equals sign
  const char* commandline;
  if (previousarg[0] == '=' && !previousarg[1]) {
    //search for argument preceding the the equals sign
    const char* commandlinepos;
    int commandlineposval;
    commandlinepos = getenv("COMP_POINT");
    commandlineposval = atoi(commandlinepos ? commandlinepos : NULL) - strlen(partialarg);
    commandline = getenv("COMP_LINE");
    while (commandlineposval > 0 && (commandline[commandlineposval - 1] == '=' || isspace(commandline[commandlineposval - 1]))) {
      commandlineposval--;
    }
    while (commandlineposval > 0 && !isspace(commandline[commandlineposval - 1])) {
      commandlineposval--;
    }
    previousarg = commandline + commandlineposval;
  }
*/
  //check previous argument
  if (previousarg && previousarg[0] == '-' && (
       (previousarg[1] && previousarg[1] != '-' && !previousarg[2] && (current_argdef = miniargv_find_arg(previousarg, argdef)) != NULL) ||
       (previousarg[1] == '-' && (current_argdef = miniargv_find_arg(previousarg, argdef)) != NULL)
      )) {
    if (current_argdef->argparam) {
      if (current_argdef->completefn) {
        (current_argdef->completefn)(argv + 1, env, argdef, envdef, current_argdef, partialarg, 0, callbackdata);
      }
      return current_argdef;
    }
  }
  //find standalone value argument definition
  if (!partialarg[0] || partialarg[0] != '-') {
    if ((current_argdef = miniargv_find_standalonearg(argdef)) != NULL) {
      if (current_argdef->completefn) {
        if ((current_argdef->completefn)(argv + 1, env, argdef, envdef, current_argdef, partialarg, 0, callbackdata) != 0)
          return current_argdef;
      }
    }
  }
  //loop through short argument definitions
  if (!partialarg[0] || (partialarg[0] == '-' && partialarg[1] != '-')) {
    current_argdef = argdef;
    while (current_argdef->callbackfn) {
      if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
        if ((result = miniargv_complete_arg(argv, env, index, (struct miniargv_definition_struct*)(current_argdef->callbackfn), envdef, callbackdata)) != NULL)
          return result;
      } else if (current_argdef->shortarg && (!partialarg[0] || !partialarg[1] || partialarg[1] == current_argdef->shortarg)) {
        if (current_argdef->argparam && partialarg[0] == '-' && partialarg[1] == current_argdef->shortarg) {
          if (current_argdef->completefn) {
            (current_argdef->completefn)(argv + 1, env, argdef, envdef, current_argdef, partialarg, 2, callbackdata);
          }
          return current_argdef;
        }
#ifdef COMPLETE_ADD_SPACE
        printf("-%c%s\n", current_argdef->shortarg, (current_argdef->argparam ? "" : " "));
#else
        printf("-%c\n", current_argdef->shortarg);
#endif
        multipleresults++;
      }
      current_argdef++;
    }
  }
  //loop through long argument definitions
  if (!partialarg[0] || (partialarg[0] == '-' && (!partialarg[1] || partialarg[1] == '-'))) {
    //determine length of long argument without value
#if 0
    partialargend = strchrnul(partialarg, '=');
    partialarglen = (partialargend - partialarg);
#else
    if ((partialargend = strchr(partialarg, '=')) == NULL)
      partialarglen = strlen(partialarg);
    else
      partialarglen = (partialargend - partialarg);
#endif
    //loop through long argument definitions
    current_argdef = argdef;
    while (current_argdef->callbackfn) {
      if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
        if ((result = miniargv_complete_arg(argv, env, index, (struct miniargv_definition_struct*)(current_argdef->callbackfn), envdef, callbackdata)) != NULL)
          return result;
      } else if (current_argdef->longarg && (partialarglen < 2 || (strncmp(partialarg + 2, current_argdef->longarg, partialarglen - 2) == 0 && (!partialarg[partialarglen] || partialarg[partialarglen] == '=')))) {
        if (current_argdef->argparam && partialarg[partialarglen] == '=') {
          if (current_argdef->completefn) {
            (current_argdef->completefn)(argv + 1, env, argdef, envdef, current_argdef, partialarg, partialarglen + 1, callbackdata);
          }
          return current_argdef;
        }
#ifdef COMPLETE_ADD_SPACE
        printf("--%s%s\n", current_argdef->longarg, (current_argdef->argparam ? "=" : " "));
#else
        if (!current_argdef->argparam) {
          printf("--%s\n", current_argdef->longarg);
        } else {
          printf("--%s=\n", current_argdef->longarg);
        }
        last_argdef = current_argdef;
        multipleresults++;
#endif
      }
      current_argdef++;
    }
    //if only long argument found display a seperate entry for it so no space is appended on completion
    if (multipleresults == 1 && last_argdef->argparam) {
      printf("--%s=%s\n", last_argdef->longarg, last_argdef->argparam);
    }
  }
  return NULL;
}

/* when bash completion occurs the program specified via "complete -o nospace -C" is called as follows
     3 arguments are passed to this application:
      - the command whose arguments are being completed
      - the prefix of the word where the cursor is, up to the cursor
      - the word before the cursor
     environment variables passed to the application:
      - COMP_LINE contains the complete line in which completion is performed.
      - COMP_POINT contains the cursor position inside COMP_LINE (counting from 0).
      - COMP_TYPE is 9 for normal completion, 33 when listing alternatives on ambiguous completions, 37 for menu completion, 63 when tabbing between ambiguous completions, 64 to list completions after a partial completion.
      - COMP_KEY contains the key that triggered completion (e.g. a tab character if the user pressed Tab).
*/

DLL_EXPORT_MINIARGV int miniargv_completion (char *argv[], char* env[], const miniargv_definition argdef[], const miniargv_definition envdef[], const char* completionparam, void* callbackdata)
{
  int index = 0;
  //check if called from bash completion
  if (completionparam) {
    //abort if a special parameter was provided and the first argument does not match it or if not enough paremeters
    if (!argv[0] || !argv[1] || !argv[2] || !argv[3] || !argv[4] || strcmp(argv[1], completionparam) != 0)
      return 0;
    index = 3;
  } else {
    //abort if no special parameter was provided and the first argument is not the application or if not enough paremeters
    const char* prg;
    int prglen;
    const char* cmd;
    int cmdlen;
    if (!argv[0] || !argv[1] || !argv[2] || !argv[3])
      return 0;
    prg = miniargv_getprogramname(argv[0], &prglen);
    cmd = miniargv_getprogramname(argv[1], &cmdlen);
    if (!prg || !cmd || cmdlen != prglen ||
#ifdef _WIN32
        strnicmp(cmd, prg, cmdlen) != 0
#else
        strncmp(cmd, prg, cmdlen) != 0
#endif
        )
      return 0;
    index = 2;
  }
  miniargv_complete_arg(argv, env, index, argdef, envdef, callbackdata);
  return 1;
}

DLL_EXPORT_MINIARGV const miniargv_definition* miniargv_find_shortarg (char shortarg, const miniargv_definition argdef[])
{
  const miniargv_definition* result;
  const miniargv_definition* current_argdef = argdef;
  if (!shortarg || !current_argdef)
    return NULL;
  while (current_argdef->callbackfn) {
    if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
      if ((result = miniargv_find_shortarg(shortarg, (struct miniargv_definition_struct*)(current_argdef->callbackfn))) != NULL)
        return result;
    } else if (shortarg == current_argdef->shortarg) {
      return current_argdef;
    }
    current_argdef++;
  }
  return NULL;
}

DLL_EXPORT_MINIARGV const miniargv_definition* miniargv_find_longarg (const char* longarg, size_t longarglen, const miniargv_definition argdef[])
{
  const miniargv_definition* result;
  const miniargv_definition* current_argdef = argdef;
  if (!longarg || !current_argdef)
    return NULL;
  if (longarglen <= 0)
    longarglen = strlen(longarg);
  while (current_argdef->callbackfn) {
    if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
      if ((result = miniargv_find_longarg(longarg, longarglen, (struct miniargv_definition_struct*)(current_argdef->callbackfn))) != NULL)
        return result;
    } else if (current_argdef->longarg) {
      if (strncmp(longarg, current_argdef->longarg, longarglen) == 0) {
        return current_argdef;
      }
    }
    current_argdef++;
  }
  return NULL;
}

DLL_EXPORT_MINIARGV const miniargv_definition* miniargv_find_standalonearg (const miniargv_definition argdef[])
{
  const miniargv_definition* result;
  const miniargv_definition* current_argdef = argdef;
  while (current_argdef->callbackfn) {
    if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
      if ((result = miniargv_find_standalonearg((struct miniargv_definition_struct*)(current_argdef->callbackfn))) != NULL)
        return result;
    } else if (!current_argdef->shortarg && !current_argdef->longarg) {
      return current_argdef;
    }
    current_argdef++;
  }
  return NULL;
}

DLL_EXPORT_MINIARGV const miniargv_definition* miniargv_find_arg (const char* arg, const miniargv_definition argdef[])
{
  char* argend;
  size_t arglen;
  const miniargv_definition* current_argdef;
  const miniargv_definition* result;
  if (!arg || arg[0] != '-')
    return NULL;
  current_argdef = argdef;
  if (arg[1] != '-') {
    //not a valid short argument not hyphen followed by only one character
    if (!arg[1] || arg[2])
      return NULL;
    //loop through short arguments
    while (current_argdef->callbackfn) {
      if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
        if ((result = miniargv_find_arg(arg, (struct miniargv_definition_struct*)(current_argdef->callbackfn))) != NULL)
          return result;
      } else if (current_argdef->shortarg && arg[1] == current_argdef->shortarg) {
        return current_argdef;
      }
      current_argdef++;
    }
  } else {
    //not a valid long argument if the 2 hyphens are not followed by any text
    if (!arg[2])
      return NULL;
    //determine length of long argument without value
#if 0
    argend = strchrnul(arg + 2, '=');
    arglen = (argend - arg);
#else
    if ((argend = strchr(arg + 2, '=')) == NULL)
      arglen = strlen(arg);
    else
      arglen = (argend - arg);
#endif
    //loop through long arguments
    while (current_argdef->callbackfn) {
      if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
        if ((result = miniargv_find_arg(arg, (struct miniargv_definition_struct*)(current_argdef->callbackfn))) != NULL)
          return result;
      } else if (current_argdef->longarg && strncmp(arg + 2, current_argdef->longarg, arglen - 2) == 0 && arglen - 2 == strlen(current_argdef->longarg)) {
        return current_argdef;
      }
      current_argdef++;
    }
  }
  return NULL;
}

DLL_EXPORT_MINIARGV void miniargv_wrap_and_indent_text (FILE* dst, const char* text, int currentpos, int indentpos, int wrapwidth, const char* newline)
{
  const char* p;
  const char* q;
  const char* r;
  //set default wrap with if not specified
  if (!wrapwidth)
    wrapwidth = 79;
  p = text;
  //process text
  while (p) {
    q = p;
    r = p;
    //find wrap position
    do {
      //find first blank space
      while (*r && !isspace(*r))
        r++;
      //check if wrap position was exceeded
      if (currentpos + (r - p) > wrapwidth) {
        //if single word exceeds maximum width show it as a whole anyway
        if (q == p)
          q = r;
        break;
      }
      q = r;
      //wrap if line break was found
      if (*r == '\n')
        break;
      //skip blank space
      while (*r && isspace(*r))
        r++;
    } while (*r);
    //print line
    if (!*q) {
      //print as a whole if last section of text
      fprintf(dst, "%s", p);
      p = NULL;
    } else {
      //print partial text, go to new line and indent
      fprintf(dst, "%.*s%s%*s", (int)(q - p), p, (newline ? newline : "\n"), indentpos, "");
      currentpos = indentpos;
      //skip blank space
      p = q;
      while (isspace(*p))
        p++;
    }
  }
}

DLL_EXPORT_MINIARGV int miniargv_cleanup (const miniargv_definition argdef[])
{
  const miniargv_definition* current_argdef = argdef;
  int result;
  while (current_argdef->callbackfn) {
    if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
      if ((result = miniargv_cleanup((struct miniargv_definition_struct*)(current_argdef->callbackfn))) != 0)
        return result;
    } else if (current_argdef->callbackfn == miniargv_cb_strdup) {
      if (*(char**)current_argdef->userdata) {
        free(*(char**)current_argdef->userdata);
        *(char**)current_argdef->userdata = NULL;
      }
    }
    current_argdef++;
  }
  return 0;
}



DLL_EXPORT_MINIARGV int miniargv_cb_set_const_str (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(const char**)argdef->userdata = value;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_strdup (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  if (*(char**)argdef->userdata)
    free(*(char**)argdef->userdata);
  *(char**)argdef->userdata = (value ? strdup(value) : NULL);
  return 0;
}

#define BOOLEAN_VALUES_LISTS_ENTRIES 6
static const char* boolean_values_lists[2][BOOLEAN_VALUES_LISTS_ENTRIES] = {
  {"0", "no",  "off", "false", "disable", "disabled"},
  {"1", "yes", "on",  "true",  "enable",  "enabled"}
};

DLL_EXPORT_MINIARGV int miniargv_cb_set_boolean (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  int i;
  int j;
  if (!value)
    return 1;
  if (!*value)
    return 0;
  for (i = 0; i < 2; i++) {
    for (j = 0; j < BOOLEAN_VALUES_LISTS_ENTRIES; j++) {
      if (strcasecmp(value, boolean_values_lists[i][j]) == 0) {
        *(int*)argdef->userdata = i;
        return 0;
      }
    }
  }
  i = 0;
  while (value[i]) {
    if (!isspace(value[i])) {
      //not an acceptable value, empty or whitespace
      return 2;
    }
    i++;
  }
  *(int*)argdef->userdata = 0;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_int (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  long intval;
  char* end = NULL;
  if (!value || !*value)
    return 1;
  intval = strtol(value, &end, 10);
  if (!end || *end)
    return 2;
  if (intval == LONG_MIN)
    return 3;
  if (intval == LONG_MAX)
    return 4;
  *(int*)argdef->userdata = (int)intval;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_int_to_zero (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(int*)argdef->userdata = 0;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_int_to_one (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(int*)argdef->userdata = 1;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_int_to_minus_one (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(int*)argdef->userdata = -1;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_increment_int (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  (*((int*)argdef->userdata))++;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_decrement_int (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  (*((int*)argdef->userdata))--;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_long (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  long intval;
  char* end = NULL;
  if (!value || !*value)
    return 1;
  intval = strtol(value, &end, 10);
  if (!end || *end)
    return 2;
  if (intval == LONG_MIN)
    return 3;
  if (intval == LONG_MAX)
    return 4;
  *(long*)argdef->userdata = intval;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_long_to_zero (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(long*)argdef->userdata = 0;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_long_to_one (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(long*)argdef->userdata = 1;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_set_long_to_minus_one (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  *(long*)argdef->userdata = -1;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_increment_long (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  (*((long*)argdef->userdata))--;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_decrement_long (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  (*((long*)argdef->userdata))++;
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_noop (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_cb_error (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  if (argdef->userdata)
    fprintf(stderr, "%s\n", (const char*)argdef->userdata);
  return -1;
}



DLL_EXPORT_MINIARGV int miniargv_complete_cb_noop (char *argv[], char* env[], const miniargv_definition* argdef, const miniargv_definition envdef[], const miniargv_definition* currentarg, const char* arg, int argparampos, void* callbackdata)
{
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_complete_cb_env (char *argv[], char* env[], const miniargv_definition* argdef, const miniargv_definition envdef[], const miniargv_definition* currentarg, const char* arg, int argparampos, void* callbackdata)
{
  size_t pos;
  size_t len;
  char** currentenv;
  char* p;
  //get environment variable name from argument (starting at the end)
  pos = strlen(arg);
  len = 0;
  while (pos > argparampos && arg[pos - 1] != '$') {
    pos--;
    len++;
  }
  //abort if dollar sign not found
  if (pos <= argparampos || arg[pos - 1] != '$')
    return 0;
  //loop through environment variables
  currentenv = env;
  while (*currentenv) {
    if (strncmp(*currentenv, arg + pos, len) == 0) {
      if ((p = strchr(*currentenv, '=')) != NULL) {
        printf("%.*s%.*s\n", (int)pos, arg, (int)(p - *currentenv), *currentenv);
      }
    }
    currentenv++;
  }
  return 0;
}

int miniargv_complete_file_or_folder (char *argv[], const miniargv_definition* argdef, const miniargv_definition* currentarg, const char* arg, int argparampos, void* callbackdata, int hidefiles)
{
  DIR* dir;
  struct dirent* direntry;
  struct stat fileinfo;
  char* path;
  size_t pathlen;
  char* filepath;
  size_t filepathlen;
  size_t len;
  size_t pos;
  size_t direntrylen;
  char* lastdirpath = NULL;
  int multipleresults = 0;
  //get folder path from argument
  pos = strlen(arg);
  len = 0;
  while (pos > argparampos && arg[pos - 1] != '/'
#ifdef _WIN32
         && arg[pos - 1] != '\\' && arg[pos - 1] != ':'
#endif
  ) {
    pos--;
    len++;
  }
  pathlen = pos - argparampos;
  if ((path = (char*)malloc(pathlen + 2)) == NULL) {
    fprintf(stderr, "memory allocation error\n");
    return 1;
  }
  memcpy(path, arg + argparampos, pathlen);
/*
  if (pathlen > 0 && path[pathlen - 1] != '/'
#ifdef _WIN32
      && path[pathlen - 1] != '\\'
#endif
  ) {
    path[pathlen++] = '/';
  }
*/
  path[pathlen] = 0;
  //create placeholder for full file path
  filepathlen = pathlen;
  if ((filepath = (char*)malloc(filepathlen + 1)) == NULL) {
    fprintf(stderr, "memory allocation error\n");
    return 1;
  }
  memcpy(filepath, path, pathlen);
  //check directory entries
  if ((dir = opendir(*path ? path : "./")) != NULL) {
    while ((direntry = readdir(dir)) != NULL) {
      //only show matches and skip "." (current folder) and ".." (parent folder)
      if (
#ifdef _WIN32
          strnicmp(direntry->d_name, arg + pos, len) == 0
#else
          strncmp(direntry->d_name, arg + pos, len) == 0
#endif
          && (!*path || (strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0))
      ) {
        //determine full path
        direntrylen = strlen(direntry->d_name);
        if (filepathlen < pathlen + direntrylen) {
          filepathlen = pathlen + direntrylen;
          if ((filepath = (char*)realloc(filepath, filepathlen + 1)) == NULL) {
            fprintf(stderr, "memory allocation error\n");
            return 1;
          }
        }
        strcpy(filepath + pathlen, direntry->d_name);
        //get file/folder attributes
        stat(filepath, &fileinfo);
        //show result as needed
        if (!S_ISDIR(fileinfo.st_mode)) {
          if (!hidefiles)
            printf("%.*s%s\n", argparampos, arg, filepath);
        } else {
          printf("%.*s%s/\n", argparampos, arg, filepath);
          if (!lastdirpath)
            lastdirpath = strdup(filepath);
        }
        if (multipleresults < 2) {
          multipleresults++;
        } else if (lastdirpath) {
          free(lastdirpath);
          lastdirpath = NULL;
        }
      }
    }
    closedir(dir);
  }
  //if only one result and it is a directory entry repeat it with "/." appended to avoid adding a space on completion
  if (lastdirpath) {
    if (multipleresults == 1)
      printf("%.*s%s/.\n", argparampos, arg, lastdirpath);
    free(lastdirpath);
  }
  free(filepath);
  free(path);
  return 0;
}

DLL_EXPORT_MINIARGV int miniargv_complete_cb_file (char *argv[], char* env[], const miniargv_definition* argdef, const miniargv_definition envdef[], const miniargv_definition* currentarg, const char* arg, int argparampos, void* callbackdata)
{
  int result;
  if ((result = miniargv_complete_cb_env(argv, env, argdef, envdef, currentarg, arg, argparampos, callbackdata)) != 0)
    return result;
  return miniargv_complete_file_or_folder(argv, argdef, currentarg, arg, argparampos, callbackdata, 0);
}

DLL_EXPORT_MINIARGV int miniargv_complete_cb_folder (char *argv[], char* env[], const miniargv_definition* argdef, const miniargv_definition envdef[], const miniargv_definition* currentarg, const char* arg, int argparampos, void* callbackdata)
{
  int result;
  if ((result = miniargv_complete_cb_env(argv, env, argdef, envdef, currentarg, arg, argparampos, callbackdata)) != 0)
    return result;
  return miniargv_complete_file_or_folder(argv, argdef, currentarg, arg, argparampos, callbackdata, 1);
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

