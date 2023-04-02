/**
 * @file miniargv-example-local.c
 * @brief miniargv example using global variables
 * @author Brecht Sanders
 *
 * This an example of how to use miniargv to set variables by passing a pointer to them as userdata.
 * It also shows how to set values via environment variables.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <miniargv.h>

//callback function for standalone parameter
int process_arg_param (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  printf("Parameter: %s\n", value);
  return 0;
}

int main (int argc, char *argv[], char *envp[])
{
  int showhelp = 0;
  int verbose = 0;
  int number = 0;
  //definition of command line arguments
  const miniargv_definition argdef[] = {
    {'h', "help", NULL, miniargv_cb_increment_int, &showhelp, "show command line help", NULL},
    {'v', "verbose", NULL, miniargv_cb_increment_int, &verbose, "increase verbose mode\n(may be specified multiple times)", NULL},
    {'n', "number", "N", miniargv_cb_set_int, &number, "set number to N", NULL},
    {0, NULL, "PARAM", process_arg_param, NULL, "parameter", NULL},
    MINIARGV_DEFINITION_END
  };
  //definition of environment variables
  const miniargv_definition envdef[] = {
    {0, "NUMBER", "N", miniargv_cb_set_int, &number, "set number to N", NULL},
    {0, "N", NULL, miniargv_cb_set_int, &number, "set number", NULL},
    MINIARGV_DEFINITION_END
  };
  //parse program arguments
  //if (miniargv_process(environ, envp, argdef, envdef, NULL, NULL) != 0)
  if (miniargv_process(argv, envp, argdef, envdef, NULL, NULL) != 0)
    return 1;
  //show help if requested or if no command line arguments were given
  if (showhelp || argc <= 1) {
    int prognamelen;
    const char* progname = miniargv_getprogramname(argv[0], &prognamelen);
    printf("%.*s v%s\nUsage: %.*s ", prognamelen, progname, miniargv_get_version_string(), prognamelen, progname);
    miniargv_arg_list(argdef, 1);
    printf("\n");
    printf("Program to demonstrate miniargv library with local variables\n");
    miniargv_help(argdef, envdef, 0, 0);
    return 0;
  }
  //show values
  printf("verbose = %i\n", verbose);
  printf("number = %i\n", number);
  //step through all command line values
  int i = 0;
  while ((i = miniargv_get_next_arg_param(i, argv, argdef, NULL)) > 0) {
    printf("arg[%i]: %s\n", i, argv[i]);
  }
  return 0;
}
