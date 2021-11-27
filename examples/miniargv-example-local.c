/**
 * @file miniargv-example-local.c
 * @brief miniargv example using global variables
 * @author Brecht Sanders
 *
 * This an example of how to use miniargv to set variables by passing a pointer to them as userdata.
 */

#include <miniargv.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//callback function to increment an integer
int process_arg_increment (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  int* param = (int*)argdef->userdata;
  (*param)++;
  return 0;
}

//callback function to increment set an integer value
int process_arg_number (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  int* param = (int*)argdef->userdata;
  *param = atoi(value);
  return 0;
}

//callback function for standalone parameter
int process_arg_param (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  printf("Parameter: %s\n", value);
  return 0;
}

int main (int argc, char *argv[])
{
  int showhelp = 0;
  int verbose = 0;
  int number = 0;
  //definition of command line arguments
  const miniargv_definition argdef[] = {
    {'h', "help", NULL, process_arg_increment, &showhelp, "show command line help"},
    {'v', "verbose", NULL, process_arg_increment, &verbose, "increase verbose mode\n(may be specified multiple times)"},
    {'n', "number", "N", process_arg_number, &number, "set number to N"},
    {0, NULL, "PARAM", process_arg_param, NULL, "parameter"},
    {0, NULL, NULL, NULL, NULL, NULL}
  };
  const miniargv_definition envdef[] = {
    {0, "NUMBER", "N", process_arg_number, &number, "set number to N"},
    {0, "N", NULL, process_arg_number, &number, "set number"},
    {0, NULL, NULL, NULL, NULL, NULL}
  };
  //parse environment variables
  if (miniargv_process_environment((const char**)environ, envdef, NULL) != 0)
    return 1;
  //parse command line arguments
  if (miniargv_process(argc, argv, argdef, NULL, NULL) != 0)
    return 1;
  //show help if requested or if no command line arguments were given
  if (showhelp || argc <= 1) {
    int prognamelen;
    const char* progname = miniargv_getprogramname(argv[0], &prognamelen);
    printf("%.*s v%s\nUsage: %.*s ", prognamelen, progname, miniargv_get_version_string(), prognamelen, progname);
    miniargv_list_args(argdef, 1);
    printf("\n");
    miniargv_help(argdef, 0, 0);
    printf("Environment variables:\n");
    miniargv_environment_help(envdef, 0, 0);
    return 0;
  }
  //show values
  printf("verbose = %i\n", verbose);
  printf("number = %i\n", number);
  return 0;
}
