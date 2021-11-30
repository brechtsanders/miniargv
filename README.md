# miniargv

Lightweight cross-platform C/C++ library for processing command line arguments and displaying command line help. Support to load settings from environment variables is also included.

Features:
 * cross platform (tested on Windows, Linux and macOS)
 * low footprint
 * no external dependencies (requires no other libraries)
 * zero copy approach (no memory is allocated or copied)
 * data passed to `main()` is processed using callback functions
   * `argv` as passed to `int main (int argc, char *argv[])`
   * `envp` as passed to `int main (int argc, char *argv[], char *envp[])`
 * values passed to callback functions live as long as `main()`, so no copying (e.g. `strdup()`) is needed
 * as no data is allocated no

## GitHub Actions CI
[![GitHub-CI for miniargv](https://github.com/brechtsanders/miniargv/workflows/GitHub-CI%20for%20miniargv/badge.svg)](https://github.com/brechtsanders/miniargv/actions)
[![Doxygen Action for miniargv](https://github.com/brechtsanders/miniargv/actions/workflows/miniargv-doxygen.yml/badge.svg)](https://github.com/brechtsanders/miniargv/actions/workflows/miniargv-doxygen.yml)

## Links
The official repository can be found at: https://github.com/brechtsanders/miniargv

The documentation can be found at: https://brechtsanders.github.io/miniargv/miniargv_8h.html

## License

[MIT](LICENSE)
