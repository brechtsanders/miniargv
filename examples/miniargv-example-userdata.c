/**
 * @file miniargv-example-userdata.c
 * @brief miniargv example using global variables
 * @author Brecht Sanders
 *
 * This an example of how to use miniargv to set variables in an array by passing a pointer to the index in the array as userdata.
 */

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
    {'h', "help", NULL, process_arg_increment, (int[]){INT_PARAM_SHOWHELP}, "show command line help"},
    {'v', "verbose", NULL, process_arg_increment, (int[]){INT_PARAM_VERBOSE}, "increase verbose mode\n(may be specified multiple times)"},
    {'n', "number", "N", process_arg_number, (int[]){INT_PARAM_NUMBER}, "set number to N"},
    {0, NULL, NULL, NULL, NULL, NULL}
  };
  //local values to be set according to command line arguments
  int params[INT_PARAMS];
  memset(params, 0, sizeof(params));
  //parse command line arguments
  if (miniargv_process(argc, argv, argdef, NULL, &params) != 0)
    return 1;
  //show help if requested or if no command line arguments were given
  if (params[INT_PARAM_SHOWHELP] || argc <= 1) {
    int prognamelen;
    const char* progname = miniargv_getprogramname(argv[0], &prognamelen);
    printf("%.*s v%s\nUsage: %.*s ", prognamelen, progname, miniargv_get_version_string(), prognamelen, progname);
    miniargv_list_args(argdef, 1);
    printf("\n");
    miniargv_help(argdef, 0, 0);
    return 0;
  }
  //show values
  printf("verbose = %i\n", params[INT_PARAM_VERBOSE]);
  printf("number = %i\n", params[INT_PARAM_NUMBER]);
  return 0;
}
