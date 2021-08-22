#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

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
                 const unsigned int exit_code,
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

	static const uint8_t log_verbosity_mask = 0xFF; /* Allows all verbosities to be logged to stdout */
	
	const int verbosity_index = 
		get_verbosity_index(verbosity, ARRAY_SIZE(verbosity_colours) - 1);
	char message[512];
	va_list args;
	int success = 0;

	va_start(args, line);
	
	if (verbosity_index == ARRAY_SIZE(verbosity_colours) || verbosity_index == -1)
	{
		printf("Somethings wrong\n");
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

	if (verbosity & log_verbosity_mask)
	{
		if (verbosity & ~FATAL)
		{
			printf("%s%s(%d) %s: %s%s\n",
			       verbosity_colours[verbosity_index],
			       file,
			       line,
			       verbosity_strings[verbosity_index],
			       message,
			       verbosity_colours[ARRAY_SIZE(verbosity_colours) - 1]);
		}
		else
		{
			printf("%s%s(%d) %s [0x%08x]: %s%s\n",
			       verbosity_colours[verbosity_index],
			       file,
			       line,
			       verbosity_strings[verbosity_index],
			       exit_code,
			       message,
			       verbosity_colours[ARRAY_SIZE(verbosity_colours) - 1]);

			exit(exit_code);
		}
	}

	va_end(args);
}