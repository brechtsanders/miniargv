#include "miniargv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#if defined(_MSC_VER) || (defined(__MINGW32__) && !defined(__MINGW64__))
#define strcasecmp _stricmp
#endif

#define MINIARG_PROCESS_MASK_FLAGS      0x01
#define MINIARG_PROCESS_MASK_VALUES     0x02
#define MINIARG_PROCESS_MASK_BOTH       (MINIARG_PROCESS_MASK_FLAGS | MINIARG_PROCESS_MASK_VALUES)
#define MINIARG_PROCESS_MASK_FIND_ONLY  0x08
#define MINIARG_PROCESS_MASK_FIND_VALUE (MINIARG_PROCESS_MASK_FIND_ONLY | MINIARG_PROCESS_MASK_VALUES)

/* find short argument definition */
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

/* find long argument definition */
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

/* find standalone argument definition */
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

#if 1
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
#else
int miniargv_process_partial_single_arg (int* index, int* success, unsigned int flags, char* argv[], const miniargv_definition argdef[], miniargv_handler_fn badfn, void* callbackdata)
{
  int result;
  size_t l;
  const miniargv_definition* current_argdef;
  const miniargv_definition* standalonevaluedef = NULL;
  (*success) = 0;
  if (argv[*index][0] == '-' && argv[*index][1]) {
    if (argv[*index][1] != '-') {
      //find short argument in argument definitions
      current_argdef = argdef;
      while (!*success && current_argdef->callbackfn) {
        if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
          //process included argument definitions
          if ((result = miniargv_process_partial_single_arg(index, success, flags, argv, (struct miniargv_definition_struct*)(current_argdef->callbackfn), badfn, callbackdata)) != 0)
            return result;
        } else if (current_argdef->shortarg) {
          if (argv[*index][1] == current_argdef->shortarg) {
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
        }
        current_argdef++;
      }
    } else {
      //find long argument in argument definitions
      current_argdef = argdef;
      while (!*success && current_argdef->callbackfn) {
        if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
          //process included argument definitions
          if ((result = miniargv_process_partial_single_arg(index, success, flags, argv, (struct miniargv_definition_struct*)(current_argdef->callbackfn), badfn, callbackdata)) != 0)
            return result;
        } else if (current_argdef->longarg) {
          l = strlen(current_argdef->longarg);
          if (strncmp(argv[*index] + 2, current_argdef->longarg, l) == 0 && (argv[*index][2 + l] == 0 || argv[*index][2 + l] == '=')) {
            if (!current_argdef->argparam) {
              //without value
              if (argv[*index][2 + l] == 0) {
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
            } else if (argv[*index][2 + l] == '=') {
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
            }
            return 2;
          }
        }
        current_argdef++;
      }
    }
  } else {
    //standalone value argument
    current_argdef = argdef;
    while (!*success && current_argdef->callbackfn) {
      //process included argument definitions
      if (current_argdef->shortarg == MINIARGV_DEFINITION_INCLUDE_SHORTARG) {
        if ((result = miniargv_process_partial_single_arg(index, success, flags, argv, (struct miniargv_definition_struct*)(current_argdef->callbackfn), badfn, callbackdata)) != 0)
          return result;
      } else if (!current_argdef->shortarg && !current_argdef->longarg) {
        //standalone value argument definition found
        standalonevaluedef = current_argdef;
        (*success)++;
        if (standalonevaluedef->callbackfn) {
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
            if ((standalonevaluedef->callbackfn)(standalonevaluedef, argv[*index], callbackdata) == 0)
              (*success)++;
          } else {
            (*success)++;
          }
        }
        return 3;
      }
      current_argdef++;
    }
  }
  return 0;
}
#endif

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

DLL_EXPORT_MINIARGV int miniargv_process_cfgfile (const char* cfgfile, const miniargv_definition cfgdef[], void* callbackdata)
{
  FILE* src;
  char* line;
  char* p;
  char* varname;
  size_t varnamelen;
  char separator;
  char* value;
  const miniargv_definition* current_cfgdef;
  int status = 0;
  //open file for reading
  if ((src = fopen(cfgfile, "rb")) != NULL) {
    //read lines
    while (status == 0 && (line = miniargv_readline(src)) != NULL) {
      varname = line;
      //skip spaces preceding varname
      while (*varname && isspace(*varname))
        varname++;
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
        if (*varname)
          status = miniargv_process_cfgfile(varname, cfgdef, callbackdata);
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
      free(line);
    }
    fclose(src);
  }
  return 0;
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

