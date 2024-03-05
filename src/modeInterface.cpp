#include <string>

/**
 * Interface for the various modes binClock offers.
*/
class modeInterface {
    public:
        modeInterface(){}
        virtual ~modeInterface(){}

        /**
         * Name for this mode used to display to users.
        */
        std::string friendlyName = "";

        /**
         * Called during setup.
        */
        virtual void setup() = 0;
        /**
         * Called when the mode is activated (switched to).
        */
        virtual void activate() = 0;
        /**
         * Called in the main loop when the mode is active.
        */
        virtual void loop() = 0;
        /**
         * Called when the mode is deactivated (switched away from).
        */
        virtual void deactivate() = 0;

};
