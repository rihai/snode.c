/*
 * snode.c - a slim toolkit for network communication
 * Copyright (C) 2020, 2021 Volker Christian <me@vchrist.at>
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

#include "SocketContext.h"

#include "web/websocket/client/SubProtocol.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#define CLOSE_SOCKET_TIMEOUT 10

namespace web::websocket::client {

    SocketContext::SocketContext(net::socket::stream::SocketConnection* socketConnection, web::websocket::client::SubProtocol* subProtocol)
        : web::websocket::SocketContext(socketConnection, subProtocol, Role::CLIENT) {
    }

    SocketContext::~SocketContext() {
        //        web::websocket::client::SubProtocolFactorySelector::instance()->destroy(subProtocol);

        //        SubProtocolFactorySelector::instance()->select(subProtocol->getName())->destroy(subProtocol);
    }

} // namespace web::websocket::client
