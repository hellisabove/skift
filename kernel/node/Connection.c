#include <libsystem/cstring.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Handle.h"

void connection_FsOperationAccept(FsConnection *connection)
{
    connection->accepted = true;
}

bool connection_FsOperationIsAccepted(FsConnection *connection)
{
    return connection->accepted;
}

error_t connection_FsOperationSend(FsConnection *connection, FsHandle *handle, Message *message)
{
    List *inbox;

    if (fshandle_has_flag(handle, OPEN_CLIENT))
    {
        inbox = connection->message_to_server;
    }
    else
    {
        inbox = connection->message_to_client;
    }

    if (list_count(inbox) > 64)
    {
        return ERR_NO_BUFFER_SPACE_AVAILABLE;
    }

    Message *message_copy = malloc(message->size);
    memcpy(message_copy, message, message->size);

    list_pushback(inbox, message_copy);

    return ERR_SUCCESS;
}

bool connection_FsOperationCanReceive(FsConnection *connection, FsHandle *handle)
{
    if (fshandle_has_flag(handle, OPEN_CLIENT))
    {
        return list_any(connection->message_to_client);
    }
    else
    {
        return list_any(connection->message_to_server);
    }
}

error_t connection_FsOperationReceive(FsConnection *connection, FsHandle *handle, Message **message)
{
    List *inbox;

    if (fshandle_has_flag(handle, OPEN_CLIENT))
    {
        inbox = connection->message_to_client;
    }
    else
    {
        inbox = connection->message_to_server;
    }

    if (list_count(inbox) > 64)
    {
        return ERR_NO_BUFFER_SPACE_AVAILABLE;
    }

    if (!list_pop(inbox, (void **)message))
    {
        return ERR_NO_MESSAGE;
    }

    return ERR_SUCCESS;
}

void connection_FsOperationDestroy(FsConnection *connection)
{
    list_destroy(connection->message_to_client, LIST_FREE_VALUES);
    list_destroy(connection->message_to_server, LIST_FREE_VALUES);
}

FsNode *connection_create(void)
{
    FsConnection *connection = __create(FsConnection);

    fsnode_init(FSNODE(connection), FSNODE_CONNECTION);

    FSNODE(connection)->accept = (FsOperationAccept)connection_FsOperationAccept;
    FSNODE(connection)->is_accepted = (FsOperationIsAccepted)connection_FsOperationIsAccepted;
    FSNODE(connection)->send = (FsOperationSend)connection_FsOperationSend;
    FSNODE(connection)->can_receive = (FsOperationCanReceive)connection_FsOperationCanReceive;
    FSNODE(connection)->receive = (FsOperationReceive)connection_FsOperationReceive;
    FSNODE(connection)->destroy = (FsOperationDestroy)connection_FsOperationDestroy;

    connection->message_to_client = list_create();
    connection->message_to_server = list_create();

    return FSNODE(connection);
}
