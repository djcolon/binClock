#include "../modeInterface.cpp"
#include <Arduino.h>

class AllOn: public ModeInterface {
    public:
        /**
         * Name for this mode used to display to users.
        */
        std::string friendlyName = "All on";

        /**
         * Called during setup.
        */
        virtual void setup(Registers* registers = nullptr) {};
        /**
         * Called when the mode is activated (switched to).
        */
        virtual void activate(Registers* registers = nullptr) {};
        /**
         * Called in the main loop when the mode is active.
        */
        virtual void loop(Registers* registers = nullptr) {
            registers->asInt = UINT32_MAX;
            delay(100);
        };
        /**
         * Called when the mode is deactivated (switched away from).
        */
        virtual void deactivate(Registers* registers = nullptr) {};
};