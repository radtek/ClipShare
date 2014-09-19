#ifndef CLIP_SHARE_SERVICE_HEADER
#define CLIP_SHARE_SERVICE_HEADER

#include <windows.h>


/* ***DEFINES*** */

#define SERVICE_NAME			TEXT("ClipShareService")
#define SERVICE_INIT_SUCCESS	0x0000000

/* *** */


/* ***Functions*** */

BOOL InitAndStartService();

/* *** */

#endif