/*
Copyright (c) 2015 Sean Bohan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include "MuGlobals.h"

namespace mural {

    class EventDispatcher;

    /**
    * Event base class. Subclass this class to pass complex data through events.
    */
    class Event : public MuObject {
        public:
            /**
             * Default constructor.
             */
            Event();

            /**
             * Initializes the event with an eventCode
             * @param eventCode Event code to initalize with.
             */
            Event(int eventCode);
            virtual ~Event();

            /**
             * Returns the event code for this event.
             * @return Event code for the event.
             */
            int getEventCode() const;

            /**
             * Returns the event dispatcher which originated the event.
             * @return Event dispatcher which originated the event.
             */
            EventDispatcher *getDispatcher() const;

            void setEventCode(int eventCode);
            void setDispatcher(EventDispatcher *dispatcher);
            const std::string& getEventType() const;

            // In order to prevent "namespace" collisions between events of different types, all event integers must be unique.
            // This is managed by arbitrarily assigning each class a "base" constant, and adding it to all its event type constants.
            // Bases for all Mural classes are documented below, third party event types should be EVENTBASE_NONMURAL or over.
            // Note that collisions are usually safe as long as collisions do not occur between a class and its subclass.

            static const int EVENTBASE_EVENT = 0x100;
            static const int EVENTBASE_NONMURAL = 0x10000;

            bool deleteOnDispatch;

        protected:
            std::string eventType;
            EventDispatcher *dispatcher;
            int eventCode;
    };

}
