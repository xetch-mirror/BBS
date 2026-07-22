#include "fs/vfs.h"
#include "Clib/Xlibary/Xbool.h"
#include "init/include/sys_io.h"

static xbool fhs_initialized = XFALSE;

void fhs_init(void) {
    // safety
    if (fhs_initialized) {
        return;
    }

    io_print("[fhs] Constructing core directory hierarchy...\n");

    // standard root nodes
    vfs_mkdir("/bin");
    vfs_mkdir("/dev");
    vfs_mkdir("/etc");
    vfs_mkdir("/home");
    vfs_mkdir("/lib");
    vfs_mkdir("/mnt");
    vfs_mkdir("/opt");
    vfs_mkdir("/proc");
    vfs_mkdir("/root");
    vfs_mkdir("/sbin");

    // storage
    vfs_mkdir("/disk");
    vfs_mkdir("/disk/sda1");

    // /usr Tree
    vfs_mkdir("/usr");
    vfs_mkdir("/usr/bin");
    vfs_mkdir("/usr/include");
    vfs_mkdir("/usr/lib");
    vfs_mkdir("/usr/share");

    // /var Tree
    vfs_mkdir("/var");
    vfs_mkdir("/var/lib");
    vfs_mkdir("/var/log");
    vfs_mkdir("/var/tmp");

    // we use boomerang 
    fhs_initialized = XTRUE;
    io_print("[fhs] launched directory tree driver (VFS)\n");
}

xbool fhs_is_ready(void) {
    return fhs_initialized;
}
