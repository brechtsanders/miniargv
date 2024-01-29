/**
 * @file miniargv-example-specifiedcfgfile.c
 * @brief miniargv example using a configuration file specified at runtime
 * @author Brecht Sanders
 *
 * This an example of how to use miniargv to set variables based on a configuration file.
 * Configuration file format specifications:
 *  - lines starting (after skipping spaces) with has (#) or semicolon (;) are skipped (can be used for comments)
 *  - variable/value pairs are specified as variable=value or variable:value
 *  - spaces are stripped from the beginning and end of variable names and values
 */

#include <miniargv.h>
#include <stdlib.h>
#include <stdio.h>

#define PROGRAM_VERSION MINIARGV_VERSION_STRING
#define PROGRAM_NAME "miniargv-example-specifiedcfgfile"
#define PROGRAM_DESC "example miniargv program using a configuration file specified at runtime"

int main (int argc, char** argv, char *envp[])
{
  //values to be set according to command line arguments
  const char* cfgfile = NULL;
  int showversion = 0;
  int showhelp = 0;
  int number = 0;

  //definition of command line argument for specifying configuration file
  const miniargv_definition cfgenvdef[] = {
    {0, "CONFIGFILE", "FILE", miniargv_cb_set_const_str, &cfgfile, "use FILE as configuration file", NULL},
    MINIARGV_DEFINITION_END
  };
  const miniargv_definition cfgargdef[] = {
    {'c', "config", "FILE", miniargv_cb_set_const_str, &cfgfile, "use FILE as configuration file", NULL},
    MINIARGV_DEFINITION_END
  };
  //definition of command line arguments
  const miniargv_definition argdef[] = {
    {'n', "number", "N", miniargv_cb_set_int, &number, "set number to N", NULL},
    MINIARGV_DEFINITION_INCLUDE(cfgargdef),
    {'v', "version", NULL, miniargv_cb_increment_int, &showversion, "show version", NULL},
    {'h', "help", NULL, miniargv_cb_increment_int, &showhelp, "show help", NULL},
    MINIARGV_DEFINITION_END
  };

  //determine configuration file
  miniargv_process(argv, envp, cfgargdef, cfgenvdef, MINIARGV_IGNORE_ERRORS, NULL);

  //show help if requested
  if (showhelp || argc <= 1) {
    printf(
      PROGRAM_NAME " - Version " PROGRAM_VERSION "\n"
      PROGRAM_DESC "\n"
      "Usage: " PROGRAM_NAME " "
    );
    miniargv_arg_list(argdef, 1);
    printf("\n");
    miniargv_help(argdef, NULL, 0, 0);
#ifdef PORTCOLCON_VERSION
    printf(WINLIBS_HELP_COLOR);
#endif
    return 0;
  }

  //show version if requested
  if (showversion) {
    printf(PROGRAM_NAME " v" PROGRAM_VERSION "\n");
    return 0;
  }

  //process environment
  miniargv_process_env(envp, argdef, NULL);
  //process configuration file if specified
  if (cfgfile)
    miniargv_process_cfgfile(cfgfile, argdef, NULL, NULL);
  //process command line arguments
  miniargv_process_arg(argv, argdef, NULL, NULL);

  //show information
  printf("Configuration file: %s\n", (cfgfile ? cfgfile : "(none)"));
  printf("Number: %i\n", number);

  //clean up (not really needed if nothing was allocated)
  miniargv_cleanup(argdef);
  return 0;
}
