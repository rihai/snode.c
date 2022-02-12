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

#ifndef CORE_SOCKET_STREAM_SOCKETACCEPTOR_H
#define CORE_SOCKET_STREAM_SOCKETACCEPTOR_H

#include "core/AcceptEventReceiver.h"
#include "core/InitAcceptEventReceiver.h"

namespace core::socket {
    class SocketContextFactory;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "core/system/socket.h"
#include "core/system/unistd.h"
#include "log/Logger.h"

#include <any>
#include <functional>
#include <map>
#include <memory>
#include <string>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#define MAX_ACCEPTS_PER_TICK 20

namespace core::socket::stream {

    template <typename ServerConfigT, typename SocketConnectionT>
    class SocketAcceptor
        : protected SocketConnectionT::Socket
        , protected InitAcceptEventReceiver
        , protected AcceptEventReceiver {
        SocketAcceptor() = delete;
        SocketAcceptor(const SocketAcceptor&) = delete;
        SocketAcceptor& operator=(const SocketAcceptor&) = delete;

    protected:
        using ServerConfig = ServerConfigT;
        using SocketConnection = SocketConnectionT;

    private:
        using Socket = typename SocketConnection::Socket;

    public:
        using SocketAddress = typename Socket::SocketAddress;

        /** Sequence diagramm of res.upgrade(req).
        @startuml
        !include core/socket/stream/pu/SocketAcceptor.pu!0
        @enduml
        */
        SocketAcceptor(const std::shared_ptr<core::socket::SocketContextFactory>& socketContextFactory,
                       const std::function<void(SocketConnection*)>& onConnect,
                       const std::function<void(SocketConnection*)>& onConnected,
                       const std::function<void(SocketConnection*)>& onDisconnect,
                       const std::map<std::string, std::any>& options)
            : socketContextFactory(socketContextFactory)
            , onConnect(onConnect)
            , onConnected(onConnected)
            , onDisconnect(onDisconnect)
            , options(options) {
        }

        virtual ~SocketAcceptor() = default;

        void listen(const std::shared_ptr<ServerConfig>& serverConfig, const std::function<void(const SocketAddress&, int)>& onError) {
            this->serverConfig = serverConfig;
            this->onError = onError;

            InitAcceptEventReceiver::publish();
        }

    private:
        void initAcceptEvent() override {
            Socket::open(
                [this](int errnum) -> void {
                    if (errnum > 0) {
                        onError(serverConfig->getLocalAddress(), errnum);
                        destruct();
                    } else {
#if !defined(NDEBUG)
                        reuseAddress([this](int errnum) -> void {
                            if (errnum != 0) {
                                onError(serverConfig->getLocalAddress(), errnum);
                                destruct();
                            } else {
#endif
                                Socket::bind(serverConfig->getLocalAddress(), [this](int errnum) -> void {
                                    if (errnum > 0) {
                                        serverConfig->getLocalAddress();
                                        onError(serverConfig->getLocalAddress(), errnum);
                                        destruct();
                                    } else {
                                        int ret = core::system::listen(Socket::getFd(), serverConfig->getBacklog());

                                        if (ret == 0) {
                                            enable(Socket::getFd());
                                            onError(serverConfig->getLocalAddress(), 0);
                                        } else {
                                            onError(serverConfig->getLocalAddress(), errno);
                                            destruct();
                                        }
                                    }
                                });
#if !defined(NDEBUG)
                            }
                        });
#endif
                    }
                },
                SOCK_NONBLOCK);
        }

        void reuseAddress(const std::function<void(int)>& onError) {
            int sockopt = 1;

            if (core::system::setsockopt(Socket::getFd(), SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) < 0) {
                onError(errno);
            } else {
                onError(0);
            }
        }

        void acceptEvent() override {
            typename SocketAddress::SockAddr remoteAddress{};
            socklen_t remoteAddressLength = sizeof(remoteAddress);

            int fd = -1;

            int count = MAX_ACCEPTS_PER_TICK;

            do {
                fd = core::system::accept4(
                    Socket::getFd(), reinterpret_cast<struct sockaddr*>(&remoteAddress), &remoteAddressLength, SOCK_NONBLOCK);

                if (fd >= 0) {
                    typename SocketAddress::SockAddr localAddress{};
                    socklen_t addressLength = sizeof(localAddress);

                    if (core::system::getsockname(fd, reinterpret_cast<sockaddr*>(&localAddress), &addressLength) == 0) {
                        SocketConnection* socketConnection = new SocketConnection(fd,
                                                                                  socketContextFactory,
                                                                                  SocketAddress(localAddress),
                                                                                  SocketAddress(remoteAddress),
                                                                                  onConnect,
                                                                                  onDisconnect,
                                                                                  serverConfig->getReadTimeout(),
                                                                                  serverConfig->getWriteTimeout(),
                                                                                  serverConfig->getReadBlockSize(),
                                                                                  serverConfig->getWriteBlockSize(),
                                                                                  serverConfig->getTerminateTimeout());

                        onConnected(socketConnection);
                    } else {
                        PLOG(ERROR) << "getsockname";
                        core::system::shutdown(fd, SHUT_RDWR);
                        core::system::close(fd);
                    }
                }
            } while (fd >= 0 && --count > 0);

            if (fd < 0 && errno != EINTR && errno != EAGAIN) {
                PLOG(ERROR) << "accept";
            }
        }

    protected:
        void destruct() {
            delete this;
        }

        std::shared_ptr<ServerConfig> serverConfig = nullptr;

    private:
        void unobservedEvent() override {
            destruct();
        }

        std::shared_ptr<core::socket::SocketContextFactory> socketContextFactory = nullptr;

        std::function<void(SocketConnection*)> onConnect;
        std::function<void(SocketConnection*)> onDestruct;
        std::function<void(SocketConnection*)> onConnected;
        std::function<void(SocketConnection*)> onDisconnect;
        std::function<void(const SocketAddress&, int)> onError = nullptr;

    protected:
        std::map<std::string, std::any> options;
    };

} // namespace core::socket::stream

#endif // CORE_SOCKET_STREAM_SOCKETACCEPTOR_H
