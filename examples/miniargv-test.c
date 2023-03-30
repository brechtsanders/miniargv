#include <miniargv.h>
#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////

struct parameters_struct {
  int verbose;
  int number;
};

int process_arg_verbose (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  printf("Encountered -%c/--%s flag\n", argdef->shortarg, argdef->longarg);
  ((struct parameters_struct*)callbackdata)->verbose++;
  return 0;
}

int process_arg_number (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  printf("Encountered -%c/--%s flag, value=%s\n", argdef->shortarg, argdef->longarg, (value ? value : "NULL"));
  ((struct parameters_struct*)callbackdata)->number = atoi(value);
  return 0;
}

int process_arg_general_without_value (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  printf("Encountered -%c/--%s flag\n", argdef->shortarg, argdef->longarg);
  return 0;
}

int process_arg_general_with_value (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  printf("Encountered -%c/--%s flag, value=%s\n", argdef->shortarg, argdef->longarg, (value ? value : "NULL"));
  return 0;
}

int process_arg (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  printf("Encountered normal argument, value=%s\n", value);
  return 0;
}

int process_arg_error (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  printf("Encountered standalone value argument, value=%s\n", value);
  return 1;
}

const miniargv_definition argsubdef[] = {
  {'d', "digit", "D", process_arg_number, NULL, "set number to D", NULL},
  MINIARGV_DEFINITION_END
};

const miniargv_definition argdef[] = {
  {'v', "verbose", NULL, process_arg_verbose, NULL, "verbose mode", NULL},
  {'n', "number", "N", process_arg_number, NULL, "set number to N", NULL},
  MINIARGV_DEFINITION_INCLUDE(argsubdef),
  {'f', "file", "FILE", process_arg_number, NULL, "specify file", NULL},
  {'s', "string", "STR", process_arg_number, NULL, "specify string", NULL},
  {'a', NULL, NULL, process_arg_general_without_value, NULL, "general parameter without value (short)", NULL},
  {0,   "general-no-value", NULL, process_arg_general_without_value, NULL, "general parameter without value (long)", NULL},
  {'b', NULL, "VAL", process_arg_general_with_value, NULL, "general parameter with value (short)", NULL},
  {0,   "general-value", "VAL", process_arg_general_with_value, NULL, "general parameter with value (long)", NULL},
  {0,   "very-long-command-line-option", NULL, process_arg_general_without_value, NULL, "very long command line option", NULL},
  {'l', "long", NULL, process_arg_verbose, NULL, "This is a very long description line in the command line help, so it should be wrapped across multiple lines. If all goes well this should take up 3 lines in the command line help. ====================================================================================================", NULL},
  {0, NULL, "param", process_arg, NULL, "standalone value argument", NULL},
  MINIARGV_DEFINITION_END
};

////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{
  struct parameters_struct params = {
    .verbose = 0,
    .number = 0,
  };
  if (miniargv_process_arg(argv, argdef, process_arg_error, &params) != 0)
    return 1;
  printf("verbose = %i\n", params.verbose);
  printf("number = %i\n", params.number);
  //show help
  int prognamelen;
  const char* progname = miniargv_getprogramname(argv[0], &prognamelen);
  printf("%.*s v%s\nUsage: %.*s ", prognamelen, progname, miniargv_get_version_string(), prognamelen, progname);
  miniargv_arg_list(argdef, 1);
  printf("\n");
  miniargv_arg_help(argdef, 24, 79);
  return 0;
}
