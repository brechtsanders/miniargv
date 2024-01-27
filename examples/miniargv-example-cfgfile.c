/**
 * @file miniargv-example-cfgfile.c
 * @brief miniargv example using configuration files
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

static const char* cfgfile = "config.cfg";

//global values to be set according to command line arguments
static int verbose = 0;
static int number = 0;
static int bln = 0;
static char* str = NULL;
static char* pw = NULL;

//definition of command line arguments
const miniargv_definition cfgdef[] = {
  {0, "verbose", NULL, miniargv_cb_increment_int, &verbose, "set verbose mode", NULL},
  {0, "number", "N", miniargv_cb_set_int, &number, "set number to N", NULL},
  {0, "boolean", "B", miniargv_cb_set_boolean, &bln, "set boolean to B", NULL},
  {0, "string", "S", miniargv_cb_strdup, &str, "set string to S", NULL},
  {0, "password", "P", miniargv_cb_strdup, &pw, "set password to P", NULL},
  {0, "long_description_value", NULL, miniargv_cb_strdup, &str, "setting with a very long description used for testing wrapping of description in generated config file\nnew line is included in description", NULL},
  MINIARGV_DEFINITION_END
};

int main (int argc, char *argv[])
{
  //parse configuration file
  if (miniargv_process_cfgfile(cfgfile, cfgdef, "App", NULL) != 0) {
    fprintf(stderr, "Error parsing configuration file");
    return 1;
  }
  //generate example config file
  printf("--------------------------------\n");
  miniargv_cfgfile_generate(stdout, cfgdef);
  printf("--------------------------------\n");
  //show values
  printf("verbose = %i\n", verbose);
  printf("number = %i\n", number);
  printf("boolean = %i\n", bln);
  printf("string = \"%s\"\n", (str ? str : "NULL"));
  printf("password = \"%s\"\n", (pw ? pw : "NULL"));
  //clean up
  miniargv_cleanup(cfgdef);
  //free(str);
  return 0;
}
