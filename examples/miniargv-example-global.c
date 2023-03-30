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

//definition of command line arguments
const miniargv_definition argdef[] = {
  {'h', "help", NULL, miniargv_cb_increment_int, &showhelp, "show command line help", NULL},
  {'v', "verbose", NULL, miniargv_cb_increment_int, &verbose, "increase verbose mode\n(may be specified multiple times)", NULL},
  {'n', "number", "N", miniargv_cb_set_int, &number, "set number to N", NULL},
  MINIARGV_DEFINITION_END
};

int main (int argc, char *argv[])
{
  //parse command line arguments
  if (miniargv_process_arg(argv, argdef, NULL, NULL) != 0)
    return 1;
  //show help if requested or if no command line arguments were given
  if (showhelp || argc <= 1) {
    int prognamelen;
    const char* progname = miniargv_getprogramname(argv[0], &prognamelen);
    printf("%.*s v%s\nUsage:\n", prognamelen, progname, miniargv_get_version_string());
    miniargv_arg_help(argdef, 0, 0);
    return 0;
  }
  //show values
  printf("verbose = %i\n", verbose);
  printf("number = %i\n", number);
  return 0;
}
