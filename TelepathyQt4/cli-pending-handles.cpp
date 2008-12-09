/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2008 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2008 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "cli-pending-handles.h"

#include "cli-pending-handles.moc.hpp"

#include "debug-internal.hpp"

namespace Telepathy
{
namespace Client
{

struct PendingHandles::Private
{
    Connection* connection;
    uint handleType;
    bool isRequest;
    QStringList namesRequested;
    UIntList handlesToReference;
    ReferencedHandles handles;
};

PendingHandles::PendingHandles(Connection* connection, uint handleType, const QStringList& names)
    : PendingOperation(connection), mPriv(new Private)
{
    mPriv->connection = connection;
    mPriv->handleType = handleType;
    mPriv->isRequest = true;
    mPriv->namesRequested = names;
}

PendingHandles::PendingHandles(Connection* connection, uint handleType, const UIntList& handles, bool allHeld)
    : PendingOperation(connection), mPriv(new Private)
{
    mPriv->connection = connection;
    mPriv->handleType = handleType;
    mPriv->isRequest = false;
    mPriv->handlesToReference = handles;

    if (allHeld) {
        mPriv->handles = ReferencedHandles(connection, handleType, handles);
        setFinished();
    }
}

PendingHandles::~PendingHandles()
{
    delete mPriv;
}

Connection* PendingHandles::connection() const
{
    return mPriv->connection;
}

uint PendingHandles::handleType() const
{
    return mPriv->handleType;
}

bool PendingHandles::isRequest() const
{
    return mPriv->isRequest;
}

bool PendingHandles::isReference() const
{
    return !isRequest();
}

const QStringList& PendingHandles::namesRequested() const
{
    return mPriv->namesRequested;
}

const UIntList& PendingHandles::handlesToReference() const
{
    return mPriv->handlesToReference;
}

ReferencedHandles PendingHandles::handles() const
{
    return mPriv->handles;
}

void PendingHandles::onCallFinished(QDBusPendingCallWatcher* watcher)
{
    // Thanks QDBus for this the need for this error-handling code duplication
    if (mPriv->isRequest) {
        QDBusPendingReply<UIntList> reply;

        debug() << "Received reply to RequestHandles";

        if (reply.isError()) {
            debug().nospace() << " Failure: error " << reply.error().name() << ": " << reply.error().message();
            setFinishedWithError(reply.error());
        } else {
            mPriv->handles = ReferencedHandles(connection(), handleType(), reply.value());
            setFinished();
        }
    } else {
        QDBusPendingReply<void> reply;

        debug() << "Received reply to HoldHandles";

        if (reply.isError()) {
            debug().nospace() << " Failure: error " << reply.error().name() << ": " << reply.error().message();
            setFinishedWithError(reply.error());
        } else {
            mPriv->handles = ReferencedHandles(connection(), handleType(), handlesToReference());
            setFinished();
        }
    }

    watcher->deleteLater();
}

} // Telepathy::Client
} // Telepathy
