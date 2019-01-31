#include <exec/types.h>
#include <exec/resident.h>
#include <exec/io.h>
#include <exec/devices.h>
#include <exec/errors.h>
#include <dos/dosextens.h>

#include <proto/exec.h>

#include "settings.h"
#include "device.h"

LONG myAbortIO(struct IORequest *io asm("a1"), struct MyDevice *base asm("a6"))
{
    /* AbortIO is a *wish* call. Someone would like to abort current IORequest */

    /* We cannot abort, sorry... */
    return 0;
}
