/*
 * snode.c - a slim toolkit for network communication
 * Copyright (C) 2020, 2021, 2022 Volker Christian <me@vchrist.at>
 *               2021, 2022 Daniel Flockert
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

#ifndef DATABASE_MARIADB_COMMANDS_MARIADBFETCHROWCOMMAND
#define DATABASE_MARIADB_COMMANDS_MARIADBFETCHROWCOMMAND

#include "database/mariadb/MariaDBCommand.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <functional>
#include <memory>
#include <mysql.h>
#include <string>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace database::mariadb::commands {

    class MariaDBFetchRowCommand : public MariaDBCommand {
    public:
        MariaDBFetchRowCommand(MariaDBConnection* mariaDBConnection, MYSQL_RES* result, const std::function<void()>& onRowResult);
        ~MariaDBFetchRowCommand();

        int start(MYSQL* mysql) override;
        int cont(MYSQL* mysql, int status) override;

        void commandCompleted(MYSQL* mysql) override;
        void commandError(const std::string& errorString, unsigned int errorNumber) override;

        bool error() override;

    private:
        MYSQL_RES* result;
        MYSQL_ROW row;
        std::function<void()> onRowResult;
    };

} // namespace database::mariadb::commands

#endif // DATABASE_MARIADB_COMMANDS_MARIADBFETCHROWCOMMAND
