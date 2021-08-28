#include <stdio.h>
#include <stdlib.h>

#include "engine/core/application.h"

int main(int argc, char **argv)
{
	int success;
	
	success = application_initialise();
	if (success)
	{
		application_run();
		application_destroy();
	}

	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}