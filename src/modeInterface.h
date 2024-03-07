#pragma once
#include <string>
#include "registers.h"

/**
 * Interface for the various modes binClock offers.
*/
class ModeInterface {
    protected:
        /**
         * Mode id.
        */
        uint8_t id = 0;
    public:
        /**
         * Getter for mode id.
        */
        uint8_t getId() {
            return id;
        };
        /**
         * Setter for mode id.
        */
        void setId(uint8_t newId) {
            id = newId;
        };
        /**
         * Name for this mode used to display to users.
        */
        virtual std::string getFriendlyName() = 0;
        /**
         * Returns the delay between runs of loop for module.
        */
        virtual uint16_t getLoopDelay() = 0;
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
