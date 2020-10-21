/*
 * snode.c - a slim toolkit for network communication
 * Copyright (C) 2020 Volker Christian <me@vchrist.at>
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

#ifndef NET_SOCKET_SOCK_STREAM_TLS_SOCKETLISTENER_H
#define NET_SOCKET_SOCK_STREAM_TLS_SOCKETLISTENER_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#include "Logger.h"
#include "SocketConnection.h"
#include "TLSHandshake.h"
#include "socket/sock_stream/SocketListener.h"
#include "ssl_utils.h"

namespace net::socket::stream {

    template <typename SocketListener>
    class SocketServer;

    namespace tls {

        template <typename SocketT>
        class SocketListener : public stream::SocketListener<stream::tls::SocketConnection<SocketT>> {
        public:
            using SocketConnection = stream::tls::SocketConnection<SocketT>;
            using Socket = typename SocketConnection::Socket;
            using SocketAddress = typename Socket::SocketAddress;

        protected:
            SocketListener(const std::function<void(SocketConnection* socketConnection)>& onConstruct,
                           const std::function<void(SocketConnection* socketConnection)>& onDestruct,
                           const std::function<void(SocketConnection* socketConnection)>& onConnect,
                           const std::function<void(SocketConnection* socketConnection)>& onDisconnect,
                           const std::function<void(SocketConnection* socketConnection, const char* junk, ssize_t junkLen)>& onRead,
                           const std::function<void(SocketConnection* socketConnection, int errnum)>& onReadError,
                           const std::function<void(SocketConnection* socketConnection, int errnum)>& onWriteError,
                           const std::map<std::string, std::any>& options)
                : stream::SocketListener<SocketConnection>(
                      [onConstruct](SocketConnection* socketConnection) -> void {
                          onConstruct(socketConnection);
                      },
                      [onDestruct](SocketConnection* socketConnection) -> void {
                          onDestruct(socketConnection);
                      },
                      [onConnect, &ctx = this->ctx](SocketConnection* socketConnection) -> void {
                          SSL* ssl = socketConnection->startSSL(ctx);

                          if (ssl != nullptr) {
                              int ret = SSL_accept(ssl);
                              int sslErr = SSL_get_error(ssl, ret);

                              if (sslErr == SSL_ERROR_WANT_READ || sslErr == SSL_ERROR_WANT_WRITE) {
                                  socketConnection->ReadEventReceiver::suspend();

                                  TLSHandshake::doHandshake(
                                      ssl,
                                      [&onConnect, socketConnection](void) -> void {
                                          socketConnection->ReadEventReceiver::resume();
                                          onConnect(socketConnection);
                                      },
                                      [socketConnection](void) -> void {
                                          socketConnection->ReadEventReceiver::disable();
                                          PLOG(ERROR) << "TLS handshake timeout";
                                      },
                                      [socketConnection](int sslErr) -> void {
                                          socketConnection->ReadEventReceiver::disable();
                                          PLOG(ERROR) << "TLS handshake failed: " << ERR_error_string(sslErr, nullptr);
                                      });
                              } else if (sslErr == SSL_ERROR_NONE) {
                                  onConnect(socketConnection);
                              } else {
                                  socketConnection->ReadEventReceiver::disable();
                                  PLOG(ERROR) << "TLS accept failed: " << ERR_error_string(ERR_get_error(), nullptr);
                              }
                          } else {
                              socketConnection->ReadEventReceiver::disable();
                              PLOG(ERROR) << "TLS handshake failed: " << ERR_error_string(ERR_get_error(), nullptr);
                          }
                      },
                      [onDisconnect](SocketConnection* socketConnection) -> void { // onDisconnect
                          socketConnection->stopSSL();
                          onDisconnect(socketConnection);
                      },
                      onRead,
                      onReadError,
                      onWriteError,
                      options) {
                ctx = SSL_CTX_new(TLS_server_method());
                sslErr = ssl_init_ctx(ctx, options, true);
            }

            ~SocketListener() override {
                if (ctx != nullptr) {
                    SSL_CTX_free(ctx);
                }
            }

            void listen(const SocketAddress& localAddress, int backlog, const std::function<void(int err)>& onError) override {
                if (sslErr != 0) {
                    onError(-sslErr);
                } else {
                    stream::SocketListener<SocketConnection>::listen(localAddress, backlog, onError);
                }
            }

        private:
            SSL_CTX* ctx = nullptr;
            unsigned long sslErr = 0;

            template <typename SocketListener>
            friend class stream::SocketServer;
        };

    } // namespace tls

} // namespace net::socket::stream

#endif // NET_SOCKET_SOCK_STREAM_TLS_SOCKETLISTENER_H
