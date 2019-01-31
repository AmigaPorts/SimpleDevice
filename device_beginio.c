#include <exec/types.h>
#include <exec/resident.h>
#include <exec/io.h>
#include <exec/devices.h>
#include <exec/errors.h>
#include <dos/dosextens.h>

#include <proto/exec.h>

#include "settings.h"
#include "device.h"

void myBeginIO(struct IORequest *io asm("a1"), struct MyDevice *base asm("a6"))
{
    struct ExecBase *SysBase = base->md_SysBase;
    io->io_Error = 0;
    io->io_Message.mn_Node.ln_Type = NT_MESSAGE;

    Disable();
    /* 
        If command is slow, clear IOF_QUICK flag and put it to some internal queue. When
        done with slow command, use ReplyMsg to notify exec that the command completed.
        In such case *do not* reply the command now.

        When modifying IoRequest, do it in disabled state.

        In case of a quick command, handle it now.
    */
    if (0 /* If command is slow */)
    {
        io->io_Flags &= ~IOF_QUICK;
        Enable();
        /* Push the command to a queue, process it maybe in another task/process, reply when
        completed */
    }
    else
    {
        Enable();
        switch(io->io_Command)
        {
            default:
                io->io_Error = IOERR_NOCMD;
                break;
        }

        /* 
            The IOF_QUICK flag was cleared. It means the caller was going to wait for command 
            completion. Therefore, reply the command now.
        */
        if (!(io->io_Flags & IOF_QUICK))
            ReplyMsg(&io->io_Message);
    }
}