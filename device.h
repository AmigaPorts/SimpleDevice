#ifndef _MY_DEVICE_H
#define _MY_DEVICE_H

#include <exec/types.h>
#include <exec/devices.h>
#include <exec/execbase.h>

struct MyDevice {
    struct Device       md_Device;
    struct ExecBase *   md_SysBase;
    struct DosLibrary * md_DosBase;
    ULONG               md_SegList;
};

#endif /* _MY_DEVICE_H */
