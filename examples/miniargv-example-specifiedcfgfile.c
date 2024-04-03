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

int handle_miniargv_error (const miniargv_definition* argdef, const char* value, void* callbackdata)
{
  //show information about processing errors
  fprintf(stderr, "Error processing %s\n", (const char*)callbackdata);
  if (argdef) {
    if (argdef->shortarg)
      fprintf(stderr, " - Short argument: -%c\n", (const char)argdef->shortarg);
    if (argdef->longarg)
      fprintf(stderr, " - Long argument: --%s\n", (const char*)argdef->longarg);
    if (argdef->help)
      fprintf(stderr, " - Description: %s\n", (const char*)argdef->help);
  }
  if (value)
    fprintf(stderr, " - Value: %s\n", value);
  //ignore error and continue
  return 0;
}

int main (int argc, char** argv, char *envp[])
{
  //values to be set according to command line arguments
  const char* cfgfile = NULL;
  int showversion = 0;
  int showhelp = 0;
  int num = 0;
  int bln = 0;
  char* str = NULL;

  //definition of command line argument and environment variable for specifying configuration file
  const miniargv_definition cfgenvdef[] = {
    {0, "CONFIGFILE", "FILE", miniargv_cb_set_const_str, &cfgfile, "use FILE as configuration file", NULL},
    MINIARGV_DEFINITION_END
  };
  const miniargv_definition cfgargdef[] = {
    {'c', "config", "FILE", miniargv_cb_set_const_str, &cfgfile, "use FILE as configuration file", NULL},
    MINIARGV_DEFINITION_END
  };
  //definition of configuration parameters
  const miniargv_definition argdef[] = {
    {'n', "number", "N", miniargv_cb_set_int, &num, "set number to N", NULL},
    {'s', "string", "S", miniargv_cb_strdup, &str, "set string to S", NULL},
    {'b', "boolean", NULL, miniargv_cb_set_int_to_one, &bln, "set boolean to true", NULL},
    MINIARGV_DEFINITION_INCLUDE(cfgargdef),
    MINIARGV_DEFINITION_END
  };
  //definition of command line arguments (note: showing version information or help should not be triggered from environment variables or configuration file)
  const miniargv_definition cmdargdef[] = {
    MINIARGV_DEFINITION_INCLUDE(argdef),
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
  miniargv_process_env(envp, argdef, handle_miniargv_error, (void*)"environment");
  //process configuration file if specified
  if (cfgfile) {
    if (miniargv_process_cfgfile(cfgfile, argdef, "app", handle_miniargv_error, (void*)"configuration file") != 0)
      fprintf(stderr, "Error processing configuration file: %s\n", cfgfile);
  }
  //process configuration data from variable
  const char* cfgdata = "string=number of the beast\nnumber=666\n\n";
  if (miniargv_process_cfgdata(cfgdata, argdef, "app", handle_miniargv_error, (void*)"configuration file") != 0)
    fprintf(stderr, "Error processing configuration data from memory: %s\n", cfgdata);
  //process command line arguments
  miniargv_process_arg(argv, cmdargdef, handle_miniargv_error, (void*)"command line arguments");

  //show information
  printf("Configuration file: %s\n", (cfgfile ? cfgfile : "(none)"));
  printf("Number: %i\n", num);
  printf("String: %s\n", str);
  printf("Boolean: %s\n", (bln ? "true" : "false"));

  //clean up
  miniargv_cleanup(argdef);
  return 0;
}

/*
  TO DO:
  - support quoted strings
  - support comments after configuration file line
  - detect invalid characters after configuration file section
*/
