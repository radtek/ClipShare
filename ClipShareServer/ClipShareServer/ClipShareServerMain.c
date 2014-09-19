#include <stdlib.h>
#include <stdio.h>

#include "ClipShareService.h"

int main()
{
	if(!InitAndStartService())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

