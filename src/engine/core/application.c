#include "application.h"

#include <stdio.h>
#include <stdlib.h>

static Application application;

int application_initialise()
{
	printf("Initialised\n");
	return 1;
}

void application_destroy()
{
	printf("Destroyed\n");
}

void application_run()
{
	printf("Running\n");
}