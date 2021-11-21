/**
 * @file miniargv-example-global.c
 * @brief miniargv example using global variables
 * @author Brecht Sanders
 *
 * This an example of how to use miniargv to set variables with global scope.
 * Note that using global variables is bad practice, so this is not the recommended way.
 */

#include <miniargv.h>
#include <stdlib.h>
#include <stdio.h>

//global values to be set according to command line arguments
static int showhelp = 0;
static int verbose = 0;
static int number = 0;

//callback function to increment an integer
int process_arg_increment (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  int* pvar = (int*)argdef->userdata;
  (*pvar)++;
  return 0;
}

//callback function to increment set an integer value
int process_arg_number (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  int* pvar = (int*)argdef->userdata;
  (*pvar) = atoi(value);
  return 0;
}

//definition of command line arguments
const miniargv_definition argdef[] = {
  {'h', "help", NULL, process_arg_increment, &showhelp, "show command line help"},
  {'v', "verbose", NULL, process_arg_increment, &verbose, "increase verbose mode\n(may be specified multiple times)"},
  {'n', "number", "N", process_arg_number, &number, "set number to N"},
  {0, NULL, NULL, NULL, NULL, NULL}
};

int main (int argc, char *argv[])
{
  //parse command line arguments
  if (miniargv_process(argc, argv, argdef, NULL, NULL) != 0)
    return 1;
  //show help if requested or if no command line arguments were given
  if (showhelp || argc <= 1) {
    int prognamelen;
    const char* progname = miniargv_getprogramname(argv[0], &prognamelen);
    printf("%.*s v%s\nUsage:\n", prognamelen, progname, miniargv_get_version_string());
    miniargv_help(argdef, 0, 0);
    return 0;
  }
  //show values
  printf("verbose = %i\n", verbose);
  printf("number = %i\n", number);
  return 0;
}
