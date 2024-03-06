#pragma once
#include <string>
#include "registers.h"

/**
 * Interface for the various modes binClock offers.
*/
class ModeInterface {
    public:
        /**
         * Name for this mode used to display to users.
        */
        virtual std::string getFriendlyName() = 0;
        /**
         * Getter for mode id.
        */
        virtual uint8_t getId() = 0;
        /**
         * Setter for mode id.
        */
        virtual void setId(uint8_t newId) = 0;
        /**
         * Called during setup.
        */
        virtual void setup(Registers& registers) = 0;
        /**
         * Called when the mode is activated (switched to).
        */
        virtual void activate(Registers& registers) = 0;
        /**
         * Called in the main loop when the mode is active.
        */
        virtual void loop(Registers& registers) = 0;
        /**
         * Called when the mode is deactivated (switched away from).
        */
        virtual void deactivate(Registers& registers) = 0;
};
