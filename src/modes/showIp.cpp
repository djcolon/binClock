#include <Arduino.h>
#include <WiFi.h>
#include "../modeInterface.h"

/**
 * Show the ESP's ip.
*/
class ShowIp: public ModeInterface {
    public:
        /**
         * Name for this mode used to display to users.
        */
        std::string getFriendlyName() {
            return "Show IP";
        };
        /**
         * Returns the delay in ms between runs of loop for module.
        */
        uint16_t getLoopDelay() {
            return 1000;
        };
        /**
         * Called during setup.
        */
        void setup(Registers& registers) {};
        /**
         * Called when the mode is activated (switched to).
        */
        void activate(Registers& registers) {
            registers.asInt = WiFi.localIP();
            delay(100);
        };
        /**
         * Called in the main loop when the mode is active.
        */
        void loop(Registers& registers) {};
        /**
         * Called when the mode is deactivated (switched away from).
        */
        void deactivate(Registers& registers) {};
};