#include "ClipShareService.h"

int main()
{
	ClipShareService cService;

	if(!cService.InitAndStartService())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

