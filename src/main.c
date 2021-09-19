#include <stdio.h>
#include <stdlib.h>

#include "engine/core/application.h"
#include "engine/core/debug.h"

int main(int argc, char **argv)
{
	ENGINE_ERROR error;
	
	error = application_initialise();
	ENGINE_RETURN_IF_ERROR(error);

	application_run();
	application_destroy();

	return EXIT_SUCCESS;
}