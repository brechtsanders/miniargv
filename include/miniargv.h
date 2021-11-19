/**
 * @file miniargv.h
 * @brief miniargv library header file with main functions
 * @author Brecht Sanders
 *
 * This header file defines the functions for processing command line arguments
 */

#ifndef INCLUDED_MINIARGV_H
#define INCLUDED_MINIARGV_H

/*! \cond PRIVATE */
#if !defined(DLL_EXPORT_MINIARGV)
# if defined(_WIN32) && (defined(BUILD_MINIARGV_DLL) || defined(MINIARGV_EXPORTS))
#  define DLL_EXPORT_MINIARGV __declspec(dllexport)
# elif defined(_WIN32) && !defined(STATIC) && !defined(BUILD_MINIARGV_STATIC) && !defined(BUILD_MINIARGV)
#  define DLL_EXPORT_MINIARGV __declspec(dllimport)
# else
#  define DLL_EXPORT_MINIARGV
# endif
#endif
/*! \endcond */



/*! \addtogroup MINIARGV_VERSION_
 *  \brief version number constants
 * \sa     MINIARGV_VERSION
 * \sa     miniargv_get_version()
 * \sa     miniargv_get_version_string()
 * @{
 */
/*! \brief major version number \hideinitializer */
#define MINIARGV_VERSION_MAJOR 0
/*! \brief minor version number \hideinitializer */
#define MINIARGV_VERSION_MINOR 1
/*! \brief micro version number \hideinitializer */
#define MINIARGV_VERSION_MICRO 0
/** @} */

/*! \brief packed version number (bits 24-31: major version, bits 16-23: minor version, bits 8-15: micro version)
 * \sa     MINIARGV_VERSION_MAJOR
 * \sa     MINIARGV_VERSION_MINOR
 * \sa     MINIARGV_VERSION_MICRO
 * \sa     MINIARGV_VERSION_
 * \hideinitializer
 */
#define MINIARGV_VERSION (MINIARGV_VERSION_MAJOR * 0x01000000 + MINIARGV_VERSION_MINOR * 0x00010000 + MINIARGV_VERSION_MICRO * 0x00000100)

/*! \cond PRIVATE */
#define MINIARGV_VERSION_STRINGIZE_(major, minor, micro) #major"."#minor"."#micro
#define MINIARGV_VERSION_STRINGIZE(major, minor, micro) MINIARGV_VERSION_STRINGIZE_(major, minor, micro)
/*! \endcond */

/*! \brief string with dotted version number
 * \sa     MINIARGV_FULLNAME
 * \sa     MINIARGV_VERSION_MAJOR
 * \sa     MINIARGV_VERSION_MINOR
 * \sa     MINIARGV_VERSION_MICRO
 * \sa     MINIARGV_VERSION_
 * \hideinitializer
 */
#define MINIARGV_VERSION_STRING MINIARGV_VERSION_STRINGIZE(MINIARGV_VERSION_MAJOR, MINIARGV_VERSION_MINOR, MINIARGV_VERSION_MICRO)

/*! \brief string with name of miniargv library */
#define MINIARGV_NAME "miniargv"

/*! \brief string with name and version of miniargv library
 * \sa     MINIARGV_VERSION_STRING
 * \sa     MINIARGV_VERSION_MAJOR
 * \sa     MINIARGV_VERSION_MINOR
 * \sa     MINIARGV_VERSION_MICRO
 * \sa     MINIARGV_VERSION_
 * \hideinitializer
 */
#define MINIARGV_FULLNAME MINIARGV_NAME " " MINIARGV_VERSION_STRING



#ifdef __cplusplus
extern "C" {
#endif

/*! \brief get miniargv library version string
 * \param  pmajor        pointer to integer that will receive major version number
 * \param  pminor        pointer to integer that will receive minor version number
 * \param  pmicro        pointer to integer that will receive micro version number
 * \sa     miniargv_get_version_string()
 * \sa     MINIARGV_VERSION
 * \sa     MINIARGV_VERSION_MAJOR
 * \sa     MINIARGV_VERSION_MINOR
 * \sa     MINIARGV_VERSION_MICRO
 * \sa     MINIARGV_VERSION_STRING
 */
DLL_EXPORT_MINIARGV void miniargv_get_version (int* pmajor, int* pminor, int* pmicro);

/*! \brief get miniargv library version string
 * \return version string
 * \sa     miniargv_get_version()
 * \sa     MINIARGV_VERSION_STRING
 * \sa     MINIARGV_VERSION
 * \sa     MINIARGV_VERSION_MAJOR
 * \sa     MINIARGV_VERSION_MINOR
 * \sa     MINIARGV_VERSION_MICRO
 */
DLL_EXPORT_MINIARGV const char* miniargv_get_version_string ();

/*! \brief data type for command line argument definition
 * \sa     miniargv_definition_struct
 * \sa     miniargv_process()
 * \sa     miniargv_help()
 * \sa     miniargv_handler_fn
 */
typedef struct miniargv_definition_struct miniargv_definition;

/*! \brief callback function called by miniargv_process for each argument encountered
 * \param  argdef                command line argument definition, or NULL for default argument
 * \param  value                 value if specified, otherwise NULL (always specified for default arguments or if \a argdef->argparam is not NULL)
 * \param  callbackdata          user data as passed to \a miniargv_process()
 * \return 0 to continue processing or non-zero to abort
 * \sa     miniargv_process
 * \sa     miniargv_definition
 * \sa     miniargv_definition_struct
 */
typedef int (*miniargv_handler_fn)(const miniargv_definition* argdef, const char* value, void* callbackdata);

/*! \brief structure for command line argument definition
 * \sa     miniargv_definition
 * \sa     miniargv_process()
 * \sa     miniargv_help()
 * \sa     miniargv_handler_fn
 */
struct miniargv_definition_struct {
  char shortarg;                    /**< short argument (used as argument with one hyphen) */
  const char* longarg;              /**< long argument (used as argument with two hyphens) */
  const char* argparam;             /**< NULL if argument takes no value or name of value to be used in help generated by \a miniargv_help() */
  miniargv_handler_fn callbackfn;   /**< callback function to be called when argument is encountered in \a miniargv_process() */
  const void* userdata;             /**< user data specific for this argument, can be used in callback functions */
  const char* help;                 /**< description of what this command line argument is for, used by \a miniargv_help() */
};

/*! \brief process command line arguments and call the appropriate callback function for each one (except the first one which is the application name)
 * \param  argc          number of arguments in \a argv
 * \param  argv          array of arguments (first one is the application itself)
 * \param  argdef        definitions
 * \param  defaultfn     callback function for default arguments (not starting with "-" or "--"), note that "-" by itself is also treated as a default argument
 * \param  badfn         callback function for bad arguments
 * \param  callbackdata  user data passed to callback functions
 * \return zero on success or index of argument that caused processing to abort
 * \sa     miniargv_get_version_string()
 */
DLL_EXPORT_MINIARGV int miniargv_process (int argc, char *argv[], const miniargv_definition argdef[], miniargv_handler_fn defaultfn, miniargv_handler_fn badfn, void* callbackdata);

DLL_EXPORT_MINIARGV const char* miniargv_getprogramname (const char* argv0, int* length);

/*! \brief display help text explaining command line arguments
 * \param  argdef                array of command line argument definitions
 * \param  descindent            indent where description starts, defaults to 25 if set to 0
 * \param  wrapwidth             maximum line length, defaults to 79 if set to 0
 * \sa     miniargv_definition
 * \sa     miniargv_definition_struct
 */
DLL_EXPORT_MINIARGV void miniargv_help (const miniargv_definition argdef[], int descindent, int wrapwidth);

#ifdef __cplusplus
}
#endif

#endif
