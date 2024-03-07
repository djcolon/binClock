#include <map>
#include <stdexcept>
#include "modeInterface.h"
#include "registers.h"

/**
 * Responsible for managing the various modes used in binClock. 
*/
class ModeManager {
    private:
        /**
         * Track ids assigned to modes.
        */
        uint8_t idCounter = 0;
        /**
         * A map of all modes registered with the manager mapped to their
         * ids.
        */
        std::map<uint8_t, ModeInterface*> modes;
        /**
         * A pointer to the mode currently active.
        */
        ModeInterface* activeMode = nullptr;
    public:
        /**
         * Registers a mode with the mode manager.
         * Will set the first registered mode as active.
        */
        void registerMode(ModeInterface* mode) {
            uint8_t id = idCounter++;
            mode->setId(id);
            modes[id] = mode;
            if(activeMode == nullptr) {
                activeMode = mode;
            }
        }

        /**
         * Runs setup() for every mode registered.
         * Then runs activate on default mode, readying for operation.
        */
        void setup(Registers& registers) {
            std::map<uint8_t, ModeInterface*>::iterator it;
            for(it = modes.begin(); it != modes.end(); it++) {
                it->second->setup(registers);
            }
            activeMode->activate(registers);
        }

        /**
         * Returns the currently active mode.
        */
        ModeInterface* getActiveMode() {
            return activeMode;
        }

        /**
         * Activates the mode with the given id.
        */
        void activateMode(const uint8_t newModeId, Registers& registers) {
            if(newModeId > idCounter) {
                throw std::invalid_argument("received newModeId outside range of existing ids");
            }
            // First clean up the old mode.
            activeMode->deactivate(registers);
            // Set new mode active. We already checked it exists.
            activeMode = modes.at(newModeId);
            // Then init it.
            activeMode->activate(registers);
        }

        /**
         * Switches to the next mode.
        */
        void nextMode(Registers& registers) {
            // Get the next id, or 0 if we've looped.
            uint8_t newId = activeMode->getId();
            // Note the increase of the counter here!
            if(++newId > idCounter) {
                newId = 0;
            }
            activateMode(newId, registers);
        }

        /**
         * Calls loop on the currently active mode.
        */
        void loopActiveMode(Registers& registers) {
            activeMode->loop(registers);
        }
        /**
         * Returns the highest index of any registered mode.
        */
        uint8_t getHighestModeIndex() {
            return idCounter;
        }
};