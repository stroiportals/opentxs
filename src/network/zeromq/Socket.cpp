/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#include "opentxs/stdafx.hpp"

#include "opentxs/network/zeromq/implementation/Socket.hpp"

#include "opentxs/core/Log.hpp"
#include "opentxs/network/zeromq/Context.hpp"

#include <zmq.h>

#define OT_METHOD "opentxs::network::zeromq::implementation::Socket::"

namespace opentxs::network::zeromq::implementation
{
const std::map<SocketType, int> Socket::types_{
    {SocketType::Request, ZMQ_REQ},
    {SocketType::Reply, ZMQ_REP},
};

Socket::Socket(const Context& context, const SocketType type)
    : context_(context)
    , socket_(zmq_socket(context, types_.at(type)))
    , lock_()
    , type_(type)
{
    OT_ASSERT(nullptr != socket_);
}

Socket::operator void*() { return socket_; }

bool Socket::Close()
{
    OT_ASSERT(nullptr != socket_);

    Lock lock(lock_);

    return (0 == zmq_close(socket_));
}

bool Socket::SetTimeouts(
    const std::chrono::milliseconds& linger,
    const std::chrono::milliseconds& send,
    const std::chrono::milliseconds& receive)
{
    OT_ASSERT(nullptr != socket_);

    Lock lock(lock_);
    int value(linger.count());
    auto set = zmq_setsockopt(socket_, ZMQ_LINGER, &value, sizeof(value));

    if (0 != set) {
        otErr << OT_METHOD << __FUNCTION__ << ": Failed to set ZMQ_LINGER"
              << std::endl;

        return false;
    }

    value = send.count();
    set = zmq_setsockopt(socket_, ZMQ_SNDTIMEO, &value, sizeof(value));

    if (0 != set) {
        otErr << OT_METHOD << __FUNCTION__ << ": Failed to set ZMQ_SNDTIMEO"
              << std::endl;

        return false;
    }

    value = receive.count();
    set = zmq_setsockopt(socket_, ZMQ_RCVTIMEO, &value, sizeof(value));

    if (0 != set) {
        otErr << OT_METHOD << __FUNCTION__ << ": Failed to set ZMQ_RCVTIMEO"
              << std::endl;

        return false;
    }

    return true;
}

bool Socket::SetTimeouts(
    const std::uint64_t& lingerMilliseconds,
    const std::uint64_t& sendMilliseconds,
    const std::uint64_t& receiveMilliseconds)
{
    return SetTimeouts(
        std::chrono::milliseconds(lingerMilliseconds),
        std::chrono::milliseconds(sendMilliseconds),
        std::chrono::milliseconds(receiveMilliseconds));
}

SocketType Socket::Type() const { return type_; }

Socket::~Socket()
{
    Lock lock(lock_);

    if (nullptr != socket_) {
        zmq_close(socket_);
    }
}
}  // namespace opentxs::network::zeromq::implementation
