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

#include "core/EventDispatcher.h"

// #include "core/DescriptorEventDispatcher.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// #include <climits>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace core {

    /*
        void EventDispatcher::observeEnabledEvents() {
            for (DescriptorEventDispatcher& eventDispatcher : eventDispatcher) {
                eventDispatcher.observeEnabledEvents();
            }
        }

        void EventDispatcher::unobserveDisabledEvents(const utils::Timeval& currentTime) {
            for (DescriptorEventDispatcher& eventDispatcher : eventDispatcher) {
                eventDispatcher.unobserveDisabledEvents(currentTime);
            }
        }

        utils::Timeval EventDispatcher::getNextTimeout(const utils::Timeval& currentTime) {
            utils::Timeval nextTimeout = {LONG_MAX, 0};

            for (const DescriptorEventDispatcher& eventDispatcher : eventDispatcher) {
                nextTimeout = std::min(eventDispatcher.getNextTimeout(currentTime), nextTimeout);
            }

            return nextTimeout;
        }

        void EventDispatcher::stop() {
            core::TickStatus tickStatus;

            do {
                for (DescriptorEventDispatcher& eventDispatcher : eventDispatcher) {
                    eventDispatcher.stop();
                }

                tickStatus = dispatch(2, true);
            } while (tickStatus == TickStatus::SUCCESS);

            do {
                timerEventDispatcher.stop();
                tickStatus = dispatch(0, false);
            } while (tickStatus == TickStatus::SUCCESS);
        }
    */

} // namespace core
