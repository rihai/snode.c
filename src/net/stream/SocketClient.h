/*
 * snode.c - a slim toolkit for network communication
 * Copyright (C) 2020, 2021, 2022 Volker Christian <me@vchrist.at>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NET_STREAM_SOCKETCLIENT_H
#define NET_STREAM_SOCKETCLIENT_H

#include "net/SocketConfig.h" // IWYU pragma: export

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <functional>
#include <memory>
#include <string>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace net::stream {

    template <typename SocketT, typename ConfigT>
    class SocketClient : public net::SocketConfig<ConfigT> {
    protected:
        using Super = net::SocketConfig<ConfigT>;

    public:
        using Super::Super;

        using Config = ConfigT;
        using Socket = SocketT;
        using SocketAddress = typename Socket::SocketAddress;

        SocketClient() = default;
        SocketClient(const SocketClient&) = default;

        virtual ~SocketClient() = default;

        virtual void connect(const std::function<void(const SocketAddress&, int)>& onError) const = 0;

        void connect(const SocketAddress& remoteAddress,
                     const SocketAddress& localAddress,
                     const std::function<void(const SocketAddress&, int)>& onError) const;

        void connect(const SocketAddress& remoteAddress, const std::function<void(const SocketAddress&, int)>& onError) const;
    };

} // namespace net::stream

#endif // NET_STREAM_SOCKETCLIENT_H
