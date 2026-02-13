#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include "log.h"

/**
 * @brief Global state variable for logging framework, defaulted to level INFO.
 */

static log_level _current_log_level = INFO;

/**
 * @brief Text labels for logging levels supported by the logging framework.
 */
static const char* _error = "[ERROR]";
static const char* _warn = "[WARN]";
static const char* _info = "[INFO]";
static const char* _debug = "[DEBUG]";
static const char* _invalid = "[INVALID]";

/**
 * @brief Convert an enum to the corresponding text label.
 * @param log_level enum to be converted to text. 
 * Notes:
 * - If an invalid logging level enum is supplied, returns "INVALID".
 */
static const char* log_level_to_string(const log_level level)
{
    switch (level)
    {
        case ERROR: return _error;
        case WARN: return _warn;
        case INFO: return _info;
        case DEBUG: return _debug;
        default: return _invalid; 
    }
}

/**
 * @brief Set the application logging level to the desired value.
 * @param new_level the desired new logging level.
 * Notes:
 * - The framework defaults to INFO, unless the user calls this function to set desired level.
 * - Calling this function with a new logging level, updates the level to new value.
 */
void set_log_level(const log_level new_level)
{
    const char* current_level = log_level_to_string(_current_log_level);
    fprintf(stdout, "%s Logging level changed from %s to %s.\n", 
            current_level,
            current_level, log_level_to_string(new_level));
    _current_log_level = new_level;
}

/**
 * @brief Check if the current the message log level is within the current log level.
 * @param message_log_level log level of the message.
 * @return 1 (true) if message log level is within current log level.
 */
static int is_message_within_log_level(const log_level message_log_level)
{
    return _current_log_level >= message_log_level; 
}

/**
 * @brief Standard message logging function of the logging framework to be used for producing logs.
 * @param level logging level of the message.
 * @param file name of the file from which the log is generated.
 * @param function name of the function from which the log is generated.
 * @param line line number of the file from which the log is generated.
 * @param formatter the format string to be applied against the variadic arguments.
 * @param ... the variadic list of arguments to be logged.
 */
void log_message(const log_level level, const char* file, const char* function, 
        const int line, const char* formatter, ...)
{
    // If message level is outside current logging level, do nothing
    if (!is_message_within_log_level(level))
        return;
    
    fprintf(stderr, "%s %s.%s.%d: ", log_level_to_string(level), file, function, line);
    va_list args;
    va_start(args, formatter);
    vfprintf(stderr, formatter, args);
    va_end(args);
}
