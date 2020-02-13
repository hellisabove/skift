#include "kernel/sheduling/TaskBlockerRead.h"
#include "kernel/tasking.h"

bool blocker_read_can_unblock(TaskBlockerRead *blocker, Task *task)
{
    __unused(task);

    return !fsnode_is_acquire(blocker->handle->node) && fsnode_can_read(blocker->handle->node, blocker->handle);
}

void blocker_read_unblock(TaskBlockerRead *blocker, Task *task)
{
    fsnode_acquire_lock(blocker->handle->node, task->id);
}

TaskBlocker *blocker_read_create(FsHandle *handle)
{
    TaskBlockerRead *read_blocker = __create(TaskBlockerRead);

    read_blocker->blocker = (TaskBlocker){
        (TaskBlockerCanUnblock)blocker_read_can_unblock,
        (TaskBlockerUnblock)blocker_read_unblock,
    };

    read_blocker->handle = handle;

    return (TaskBlocker *)read_blocker;
}