#include <Arduino.h>
#include <WiFi.h>
#include "../modeInterface.h"

/**
 * Show the ESP's ip.
*/
class ShowIp: public ModeInterface {
    private:
        /**
         * Mode id, set by ModeManager. Treat as read-only.
        */
        uint8_t id = 0;
        /**
         * Name for this mode used to display to users.
        */
       std::string friendlyName = "Show IP";
    public:
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