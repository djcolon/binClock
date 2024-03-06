#include "../modeInterface.h"

/**
 * Use this as a template to create a new mode.
*/
class ModeTemplate: public ModeInterface {
    private:
        /**
         * Mode id, set by ModeManager. Treat as read-only.
        */
        uint8_t id = 0;
        /**
         * Name for this mode used to display to users.
        */
       std::string friendlyName = "Mode template";
    public:
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
        void loop(Registers& registers) {};
        /**
         * Called when the mode is deactivated (switched away from).
        */
        void deactivate(Registers& registers) {};
};