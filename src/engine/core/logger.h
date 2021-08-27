#ifndef _LOGGER_H_
#define _LOGGER_H_

/******************************************************************************
 * @name  Verbosity
 * @brief Valid log verbosity levels.
******************************************************************************/
enum Verbosity
{
	VERB_DEBUG   = 0x01,
	VERB_INFO    = 0x02,
	VERB_WARNING = 0x04,
	VERB_ERROR   = 0x08,
	VERB_FATAL   = 0x10
};

/******************************************************************************
 * @name   log_message()
 * @brief  Used to log a message to stdout or a specified file at a certain log
 *         verbosity.
 * @param  verbosity  The verbosity level of the log message.
 * @param  error_code The error code that caused this log message (this should
 *                    be set to 0 with any verbosity less than FATAL).
 * @param  format     The format of the message to be logged.
 * @param  file       The file in which the call to log was made.
 * @param  line       The line at which the call to log was made.
 * @return void
******************************************************************************/
void log_message(enum Verbosity verbosity,
                 const unsigned int exit_code,
                 const char *restrict format,
                 const char *restrict file,
                 const unsigned int line,
                 ...);


/******************************************************************************
 * Easy to use macros to be used to log.
******************************************************************************/

#define LOG_DEBUG(format, ...) \
	log_message(VERB_DEBUG, 0, format, __FILE__, __LINE__, ##__VA_ARGS__);
	
#define LOG_INFO(format, ...) \
	log_message(VERB_INFO, 0, format, __FILE__, __LINE__, ##__VA_ARGS__);

#define LOG_WARNING(format, ...) \
	log_message(VERB_WARNING, 0, format, __FILE__, __LINE__, ##__VA_ARGS__);

#define LOG_ERROR(format, ...) \
	log_message(VERB_ERROR, 0, format, __FILE__, __LINE__, ##__VA_ARGS__);

#define LOG_FATAL(error_code, format, ...) \
	log_message(VERB_FATAL, error_code, format, __FILE__, __LINE__, ##__VA_ARGS__);


#endif /* _LOGGER_H_ */