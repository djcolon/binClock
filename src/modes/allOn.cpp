#include "../modeInterface.h"
#include "time.h"

/**
 * Mode that turns on all LEDs.
*/
class AllOn: public ModeInterface {
    public:
        /**
         * Name for this mode used to display to users.
        */
        std::string getFriendlyName() {
            return "All on";
        };
        /**
         * Returns the delay in ms between runs of loop for module.
        */
        uint16_t getLoopDelay() {
            return 100;
        };
        /**
         * Called during setup.
        */
        void setup(Registers& registers) {};
        /**
         * Called when the mode is activated (switched to).
        */
        void activate(Registers& registers) {
            registers.asInt = UINT32_MAX;
        };
        /**
         * Called in the main loop when the mode is active.
        */
        void loop(Registers& registers) {
            registers.asInt = UINT32_MAX;
        };
        /**
         * Called when the mode is deactivated (switched away from).
        */
        void deactivate(Registers& registers) {};
};