#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])

/******************************************************************************
 * @name   get_verbosity_index()
 * @brief  Transforms a verbosity into a index for an array.
 * @param  verbosity         The verbosity to query.
 * @param  available_indexes The number of indexes to check.
 * @return An integer specifying the index or -1 if the .
******************************************************************************/
static int get_verbosity_index(enum Verbosity verbosity, size_t max_index)
{
	uint8_t verbosity_index = (uint8_t)verbosity;

	for (int i = 0; i < max_index; i++)
	{
		verbosity_index >>= 1;
		if (verbosity_index == 0)
		{
			return i;
		}
	}

	return -1;
}

void log_message(enum Verbosity verbosity,
                 const char *restrict format,
                 const char *restrict file,
                 const unsigned int line,
                 ...)
{
	static const char *const verbosity_colours[] = {
		"\e[0;34m",   /* DEBUG - Blue */
		"\e[0;37m",   /* INFO - White */
		"\e[0;33m",   /* WARNING - Yellow */
		"\e[0;31m",   /* ERROR - Red */
		"\e[1;31m",   /* FATAL - Red */
		"\e[0;0m",    /* Reset */
	};

	static const char *const verbosity_strings[] = {
		"Debug",
		"Info",
		"Warning",
		"Error",
		"Fatal"
	};

	static const uint8_t log_to_stdout_mask = 0xFF; /* Verbosities included in this mask will be logged to stdout. */
	
	const int verbosity_index = 
		get_verbosity_index(verbosity, ARRAY_SIZE(verbosity_colours) - 1);
	char message[256];
	char log_message[512];
	FILE *log_file = fopen("./logs/engine.log", "a");
	va_list args;
	int success = 0;

	va_start(args, line);
	
	if (!(verbosity & log_to_stdout_mask))
	{
		return;
	}

	if (strncpy(message, format, ARRAY_SIZE(message)) == NULL)
	{
		printf("Failed to copy log message\n");
		return;
	}

	success = vsnprintf(message, ARRAY_SIZE(message), format, args);
	if (success > ARRAY_SIZE(message) && success < 0)
	{
		printf("Failed to create log message\n");
		return;
	}

	if (verbosity & log_to_stdout_mask)
	{
		snprintf(log_message,
		         sizeof(log_message),
		         "%s%s: %s%s\n",
		         verbosity_colours[verbosity_index],
		         verbosity_strings[verbosity_index],
		         message,
		         verbosity_colours[ARRAY_SIZE(verbosity_colours) - 1]);
		printf(log_message);
	}

	if (log_file != NULL)
	{
		time_t raw_time;
		char str_time[32];

		time(&raw_time);
		strftime(str_time, sizeof(str_time), "%X", localtime(&raw_time));

		snprintf(log_message,
		         sizeof(log_message),
		         "[%s] %s: %s\n",
		         str_time,
		         verbosity_strings[verbosity_index],
		         message);
		fprintf(log_file, log_message);
		fclose(log_file);
	}

	va_end(args);
}