#include "../modeInterface.h"
#include <Arduino.h>

class AllOn: public ModeInterface {
    private:
        /**
         * Mode id.
        */
        uint8_t id = 0;
        /**
         * Name for this mode used to display to users.
        */
       std::string friendlyName = "Ping Pong";
       /**
        * Track which way we're moving.
       */
        bool pingPongLeft = true;
    public:
        /**
         * Called during setup.
        */
        void setup(Registers& registers) {

        };
        /**
         * Called when the mode is activated (switched to).
        */
        void activate(Registers& registers) {
            // Make sure we start with only a single LED being on.
            registers.asInt = 1;
        };
        /**
         * Called in the main loop when the mode is active.
        */
        void loop(Registers& registers) {
            // Do we need to change dir?
            if(registers.asInt & 1) {
                pingPongLeft = true;
            // 2147483648 = 2^31 i.e. highest bit on register set.
            }
            // Then shift.
            if(pingPongLeft) {
                registers.asInt = registers.asInt << 1;
            } else {
                registers.asInt = registers.asInt >> 1;
            }
            // Reverse other way?
            if(registers.asInt & 2147483648) {
                pingPongLeft = false;
            }
            delay(100);
        };
        /**
         * Called when the mode is deactivated (switched away from).
        */
        void deactivate(Registers& registers) {};
};