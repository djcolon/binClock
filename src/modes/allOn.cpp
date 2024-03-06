#include "../modeInterface.h"
#include <Arduino.h>

class AllOn: public ModeInterface {
    private:
        /**
         * Mode id.
        */
        uint8_t id = 0;
    public:
        /**
         * Name for this mode used to display to users.
        */
        std::string getFriendlyName() {
            return "All on";
        }
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
         * Called during setup.
        */
        void setup(Registers& registers) {};
        /**
         * Called when the mode is activated (switched to).
        */
        void activate(Registers& registers) {};
        /**
         * Called in the main loop when the mode is active.
        */
        void loop(Registers& registers) {
            registers.asInt = UINT32_MAX;
            delay(100);
        };
        /**
         * Called when the mode is deactivated (switched away from).
        */
        void deactivate(Registers& registers) {};
};