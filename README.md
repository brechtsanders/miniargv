# miniargv

Lightweight cross-platform C/C++ library for processing command line arguments and displaying command line help. Support to load settings from environment variables is also included.

Features:
 * cross-platform (tested on Windows, Linux and macOS)
 * low footprint (small code footprint and very low memory requirements)
 * no external dependencies (requires no other libraries)
 * zero copy approach (no memory is allocated or copied)
 * data passed to `main()` is processed using callback functions
   * `argv` as passed to `int main (int argc, char *argv[])`
   * `envp` as passed to `int main (int argc, char *argv[], char *envp[])`
 * values passed to callback functions live as long as `main()`, so no copying or duplication (like `strdup()`) needed
 * no cleanup functions needed before exiting the program
 * definitions for processing and displaying help are done in a common data structure
 * basic error checking to report unknown arguments
 * does not include constraints checking (e.g. mandatory arguments or arguments that may not occur multiple times)

Example:
```C
#include <stdlib.h>
#include <stdio.h>
#include <miniargv.h>

int main (int argc, char *argv[], char *envp[])
{
  int showhelp = 0;
  int number = 0;
  //definition of command line arguments
  const miniargv_definition argdef[] = {
    {'h', "help", NULL, miniargv_cb_increment_int, &showhelp, "show command line help"},
    {'n', "number", "N", miniargv_cb_set_int, &number, "set number to N"},
    {0, NULL, NULL, NULL, NULL, NULL}
  };
  //definition of environment variables
  const miniargv_definition envdef[] = {
    {0, "NUMBER", "N", miniargv_cb_set_int, &number, "set number to N"},
    {0, NULL, NULL, NULL, NULL, NULL}
  };
  //parse program arguments
  if (miniargv_process(argv, envp, argdef, envdef, NULL, NULL) != 0)
    return 1;
  //show help if requested or if no command line arguments were given
  if (showhelp || argc <= 1) {
    printf("Usage: ");
    miniargv_arg_list(argdef, 1);
    printf("\n");
    miniargv_help(argdef, envdef, 0, 0);
    return 0;
  }
  //show values
  printf("number = %i\n", number);
  return 0;
}
```

## GitHub Actions CI
[![GitHub-CI for miniargv](https://github.com/brechtsanders/miniargv/workflows/GitHub-CI%20for%20miniargv/badge.svg)](https://github.com/brechtsanders/miniargv/actions)
[![Doxygen Action for miniargv](https://github.com/brechtsanders/miniargv/actions/workflows/miniargv-doxygen.yml/badge.svg)](https://github.com/brechtsanders/miniargv/actions/workflows/miniargv-doxygen.yml)

## Links
The official repository can be found at: https://github.com/brechtsanders/miniargv

The documentation can be found at: https://brechtsanders.github.io/miniargv/miniargv_8h.html

## License

[MIT](LICENSE)
