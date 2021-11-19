#include <miniargv.h>
#include <stdlib.h>
#include <stdio.h>

static int showhelp = 0;
static int verbose = 0;
static int number = 0;

int process_arg_increment (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  int* pvar = (int*)argdef->userdata;
  (*pvar)++;
  return 0;
}

int process_arg_number (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  int* pvar = (int*)argdef->userdata;
  (*pvar) = atoi(value);
  return 0;
}

const miniargv_definition argdef[] = {
  {'v', "verbose", NULL, process_arg_increment, &verbose, "increase verbose mode\n(may be specified multiple times)"},
  {'n', "number", "N", process_arg_number, &number, "set number to N"},
  {'h', "help", NULL, process_arg_increment, &showhelp, "show command line help"},
  {0, NULL, NULL, NULL, NULL, NULL}
};

int main (int argc, char *argv[])
{
  if (miniargv_process(argc, argv, argdef, NULL, NULL, NULL) != 0)
    return 1;
  if (showhelp || argc <= 1) {
    int prognamelen;
    const char* progname = miniargv_getprogramname(argv[0], &prognamelen);
    printf("%.*s v%s\nUsage:\n", prognamelen, progname, miniargv_get_version_string());
    miniargv_help(argdef, 0, 0);
    return 0;
  }
  printf("verbose = %i\n", verbose);
  printf("number = %i\n", number);
  return 0;
}
