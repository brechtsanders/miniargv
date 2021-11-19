#include <miniargv.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//indices for integer values
#define INT_PARAM_SHOWHELP 0
#define INT_PARAM_VERBOSE  1
#define INT_PARAM_NUMBER   2
#define INT_PARAMS         3

//callback function to increment an integer
int process_arg_increment (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  int* params = (int*)callbackdata;
  int paramindex = *(int*)argdef->userdata;
  params[paramindex]++;
  return 0;
}

//callback function to increment set an integer value
int process_arg_number (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  int* params = (int*)callbackdata;
  int paramindex = *(int*)argdef->userdata;
  params[paramindex] = atoi(value);
  return 0;
}

int main (int argc, char *argv[])
{
  //definition of command line arguments
  const miniargv_definition argdef[] = {
    {'v', "verbose", NULL, process_arg_increment, (int[1]){INT_PARAM_VERBOSE}, "increase verbose mode\n(may be specified multiple times)"},
    {'n', "number", "N", process_arg_number, (int[1]){INT_PARAM_NUMBER}, "set number to N"},
    {'h', "help", NULL, process_arg_increment, (int[1]){INT_PARAM_SHOWHELP}, "show command line help"},
    {0, NULL, NULL, NULL, NULL, NULL}
  };
  //local values to be set according to command line arguments
  int params[INT_PARAMS];
  memset(params, 0, sizeof(params));
  //parse command line arguments
  if (miniargv_process(argc, argv, argdef, NULL, NULL, &params) != 0)
    return 1;
  //show help if requested or if no command line arguments were given
  if (params[INT_PARAM_SHOWHELP] || argc <= 1) {
    int prognamelen;
    const char* progname = miniargv_getprogramname(argv[0], &prognamelen);
    printf("%.*s v%s\nUsage:\n", prognamelen, progname, miniargv_get_version_string());
    miniargv_help(argdef, 0, 0);
    return 0;
  }
  //show values
  printf("verbose = %i\n", params[INT_PARAM_VERBOSE]);
  printf("number = %i\n", params[INT_PARAM_NUMBER]);
  return 0;
}
