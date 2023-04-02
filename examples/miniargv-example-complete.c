/**
 * @file miniargv-example-complete.c
 * @brief miniargv example for bash completion
 * @author Brecht Sanders
 *
 * This an example of how to use miniargv with bash completion.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <miniargv.h>

//returns what we already have with 0-9 appended
int complete_number (char *argv[], char *env[], const miniargv_definition* argdef, const miniargv_definition envdef[], const miniargv_definition* currentarg, const char* arg, int argparampos, void* callbackdata)
{
  int i;
  printf("%s\n", arg);
  for (i = 0; i < 10; i++) {
    printf("%s%i\n", arg, i);
  }
  return 0;
}

//predefined set of values
int complete_test (char *argv[], char *env[], const miniargv_definition* argdef, const miniargv_definition envdef[], const miniargv_definition* currentarg, const char* arg, int argparampos, void* callbackdata)
{
  const char* data[] = {
    "123",
    "456",
    "789",
    "aaa",
    "abc",
    "a-z",
    "a b c",
    NULL
  };
  const char** p = data;
  int len = strlen(arg + argparampos);
  while (*p) {
    if (strncmp(*p, arg + argparampos, len) == 0) {
      printf("%.*s%s\n", argparampos, arg, *p);
    }
    p++;
  }
  return 0;
}

int main (int argc, char *argv[], char *envp[])
{
  int showhelp = 0;
  int verbose = 0;
  int number = 0;
  const char* name = NULL;
  const char* filepath = NULL;
  const char* envval = NULL;
  //definition of command line arguments
  const miniargv_definition argdef[] = {
    {'h', "help", NULL, miniargv_cb_increment_int, &showhelp, "show command line help", NULL},
    {'v', "verbose", NULL, miniargv_cb_increment_int, &verbose, "increase verbose mode\n(may be specified multiple times)", NULL},
    {'n', "number", "N", miniargv_cb_set_int, &number, "set number to N", complete_number},
    {'N', "name", "NAME", miniargv_cb_set_const_str, &name, "set name to NAME", complete_test},
    {'f', "file", "PATH", miniargv_cb_set_const_str, &filepath, "set file", miniargv_complete_cb_file},
    {'F', "folder", "PATH", miniargv_cb_set_const_str, &filepath, "set folder", miniargv_complete_cb_folder},
    {'e', "environment", "PATH", miniargv_cb_set_const_str, &envval, "set value (supports environment variables)", miniargv_complete_cb_env},
    {'d', NULL, "VALUE", miniargv_cb_noop, NULL, "dummy, ignore VALUE", NULL},
    {0, "dummy", "VALUE", miniargv_cb_noop, NULL, "dummy, ignore VALUE", NULL},
    {0, NULL, "PARAM", miniargv_cb_noop, NULL, "parameter", complete_test},
    MINIARGV_DEFINITION_END
  };
/*
  //check if we are being called for bash completion (tab key on the command line, configured via: "complete -C"<path> --bash-complete" <command>")
  if (miniargv_completion(argv, argdef, "--bash-complete", NULL)) {
*/
  //check if we are being called for bash completion (tab key on the command line, configured via: "complete -C<path> <command>")
  if (miniargv_completion(argv, envp, argdef, NULL, NULL, NULL)) {
    return 0;
  }
  //parse command line flags
  if (miniargv_process_arg(argv, argdef, NULL, NULL) != 0)
    return 1;
  //show help if requested or if no command line arguments were given
  if (showhelp || argc <= 1) {
    int prognamelen;
    const char* progname = miniargv_getprogramname(argv[0], &prognamelen);
    printf("%.*s v%s\nUsage: %.*s ", prognamelen, progname, miniargv_get_version_string(), prognamelen, progname);
    miniargv_arg_list(argdef, 1);
    printf("\n");
    printf("Program to demonstrate miniargv library bash shell tab completion\n");
    miniargv_help(argdef, NULL, 0, 0);
    return 0;
  }
  //show values
  printf("verbose = %i\n", verbose);
  printf("number = %i\n", number);
  printf("name = %s\n", (name ? name : ""));
  printf("file = %s\n", (filepath ? filepath : ""));
  printf("envval = %s\n", (envval ? envval : ""));
  //step through all command line values
  int i = 0;
  while ((i = miniargv_get_next_arg_param(i, argv, argdef, NULL)) > 0) {
    printf("arg[%i]: %s\n", i, argv[i]);
  }
  return 0;
}
