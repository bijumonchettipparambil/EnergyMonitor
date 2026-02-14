#ifndef LOG_LOG_H_
#define LOG_LOG_H_

/**

  Enum defining the different logging levels supported by the logging framework.

*/
typedef enum
{
    ERROR = 0,
    WARN = 1,
    INFO = 2,
    DEBUG = 3

} log_level;

/**

  Set the application logging level to the desired value.
  
  @param new_level the desired new logging level.

Notes:
    - The framework defaults to INFO, unless the user calls this function to set desired level.
    - Calling this function with a new logging level, updates the level to new value.
*/
void set_log_level(const log_level new_level);

/**

   Standard message logging function of the logging framework to be used for producing logs.
   
   @param level logging level of the message.
   @param file name of the file from which the log is generated.
   @param function name of the function from which the log is generated.
   @param line line number of the file from which the log is generated.
   @param formatter the formatter string to be applied against the variadic arguments.
   @param ... the variadic list of arguments to be logged.

*/
void log_message(const log_level level, const char* file, const char* function, const int line, const char* formatter, ...);

/**

  Macro for logging.

*/
#define LOG(level,...) \
   log_message(level, __FILE__, __func__, __LINE__, __VA_ARGS__)

#endif
