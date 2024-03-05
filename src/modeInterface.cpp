#include <string>
#include "registers.cpp"

/**
 * Interface for the various modes binClock offers.
*/
class ModeInterface {
    public:
        ModeInterface(){}
        virtual ~ModeInterface(){}

        /**
         * Name for this mode used to display to users.
        */
        std::string friendlyName = "";

        /**
         * Called during setup.
        */
        virtual void setup(Registers* registers = nullptr) = 0;
        /**
         * Called when the mode is activated (switched to).
        */
        virtual void activate(Registers* registers = nullptr) = 0;
        /**
         * Called in the main loop when the mode is active.
        */
        virtual void loop(Registers* registers = nullptr) = 0;
        /**
         * Called when the mode is deactivated (switched away from).
        */
        virtual void deactivate(Registers* registers = nullptr) = 0;

};
