
#include <libsystem/CString.h>

#include "kernel/filesystem/Filesystem.h"

static Result zero_read(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);
    __unused(handle);

    memset(buffer, 0, size);

    *read = size;

    return SUCCESS;
}

static Result zero_write(FsNode *node, FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    __unused(node);
    __unused(handle);
    __unused(buffer);

    *written = size;

    return SUCCESS;
}

void zero_initialize(void)
{
    FsNode *zero_device = __create(FsNode);
    fsnode_init(zero_device, FILE_TYPE_DEVICE);

    FSNODE(zero_device)->read = (FsNodeReadCallback)zero_read;
    FSNODE(zero_device)->write = (FsNodeWriteCallback)zero_write;

    Path *zero_device_path = path_create("/dev/zero");
    filesystem_link_and_take_ref(zero_device_path, zero_device);
    path_destroy(zero_device_path);
}
