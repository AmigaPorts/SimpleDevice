#include <exec/types.h>
#include <exec/resident.h>
#include <exec/io.h>
#include <exec/devices.h>
#include <exec/errors.h>
#include <dos/dosextens.h>

#include <proto/exec.h>

#include "settings.h"
#include "device.h"

/* 
    Put the function at the very beginning of the file in order to avoid
    unexpected results when user executes the device by mistake
*/

static int __attribute__((used)) doNotExecute()
{
    return -1;
}

/*
    Put this marker at the very end of your executable. It's not absolutely
    mandatory but will let rom tag scanner of exec.library work better/faster.
*/
extern const UBYTE    endOfCode;

/*
    Const fields containing name of device and ID string. Note! It's not the 
    version string as in case of executables (i.e. the $VER:), but rather 
    "name version.revision (date)" string.
*/
static const char deviceName[] = MY_DEVICE_NAME;
static const char deviceIdString[] = MY_DEVICE_IDSTRING;
static const APTR initTable[4];

/*
    Resident structure describing the object. RTF_AUTOINIT means the rt_Init field
    points to the initializer table defined below. RTF_COLDSTART defines when the
    object will be initialized (coldstart means, before dos.library, after scheduler
    is started)
*/
static struct Resident const myDeviceResident __attribute__((used)) = {
    RTC_MATCHWORD,
    (struct Resident *)&myDeviceResident,
    (APTR)&endOfCode,
    RTF_AUTOINIT | RTF_COLDSTART,
    MY_DEVICE_VERSION,
    NT_DEVICE,
    MY_DEVICE_PRIORITY,
    (APTR)&deviceName,
    (APTR)&deviceIdString,
    (APTR)&initTable
};

/*
    Initializer table. First field is the size of structure describing the object,
    can be sizeof(struct Library), sizeof(struct Device) or any size necessary to
    store user defined object extending the Device structure.
*/
static const APTR initTable[4] = {
    (APTR)sizeof(struct MyDevice),
    NULL,
    NULL,
    NULL
};

void openLib(struct IORequest * io asm("a1"), LONG unitNumber asm("d0"),
    ULONG flags asm("d1"), struct MyDevice * base asm("a6"));
ULONG closeLib(struct IORequest * io asm("a1"), struct MyDevice * base asm("a6"));
ULONG expungeLib(struct MyDevice * base asm("a6"));
APTR extFunc(struct MyDevice * base asm("a6"));

static const APTR funcTable[] = {
    (APTR)openLib,
    (APTR)closeLib,
    (APTR)expungeLib,
    (APTR)extFunc,
//    (APTR)myBeginIO,
//    (APTR)myAbortIO,
    (APTR)-1
};

void openLib(struct IORequest * io asm("a1"), LONG unitNumber asm("d0"),
    ULONG flags asm("d1"), struct MyDevice * base asm("a6"))
{
    int ok = 1;

    /* 
        Do whatever necessary to open given unit number with flags, set NT_REPLYMSG if 
        opening device shall complete with success, set io_Error otherwise
    */

    if (io->io_Error == 0)
    {
        base->md_Device.dd_Library.lib_OpenCnt++;
        base->md_Device.dd_Library.lib_Flags &= ~LIBF_DELEXP;

        io->io_Message.mn_Node.ln_Type = NT_REPLYMSG;
    }
    else
    {
        io->io_Error = IOERR_OPENFAIL;
    }
    
    /* In contrast to normal library there is no need to return anything */
    return;
}

ULONG closeLib(struct IORequest * io asm("a1"), struct MyDevice * base asm("a6"))
{
    base->md_Device.dd_Library.lib_OpenCnt--;

    if (base->md_Device.dd_Library.lib_OpenCnt == 0)
    {
        if (base->md_Device.dd_Library.lib_Flags & LIBF_DELEXP)
        {
            return expungeLib(base);
        }
    }
    
    return 0;
}

ULONG expungeLib(struct MyDevice * base asm("a6"))
{
    if (base->md_Device.dd_Library.lib_OpenCnt > 0)
    {
        base->md_Device.dd_Library.lib_Flags |= LIBF_DELEXP;
        return 0;
    }
    else
    {
        struct ExecBase *SysBase = base->md_SysBase;
        ULONG size;

        /* Remove yourself from list of devices */
        Remove((struct Node *)base);

        /* If dos was open close it now */
        if (base->md_DosBase)
        {
            CloseLibrary((struct Library *)base->md_DosBase);
        }

        /* Calculate size of device base and deallocate memory */
        size = base->md_Device.dd_Library.lib_NegSize + base->md_Device.dd_Library.lib_PosSize;
        APTR pointer = (APTR)((ULONG)base - base->md_Device.dd_Library.lib_NegSize);

        FreeMem(pointer, size);
    }
}


APTR extFunc(struct MyDevice * base asm("a6"))
{
    return base;
}
