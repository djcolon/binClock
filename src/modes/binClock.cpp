#include <Arduino.h>
#include <stdexcept>
#include "time.h"
#include "../modeInterface.h"

/**
 * The main mode used to show the time in binary.
*/
class BinClock: public ModeInterface {
    private:
        /**
         * The timezone for the clock.
        */
       std::string timezone = "GMT0BST,M3.5.0/1,M10.5.0;";

        // Sets TZ from POSIX style TZ.
        // See: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
        void setTimezone(std::string timezone)
        {
            setenv("TZ", timezone.c_str(), 1);
            tzset();
        }
    public:
        /**
         * Name for this mode used to display to users.
        */
        std::string getFriendlyName() {
            return "Binary clock";
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
        void setup(Registers& registers) {
            struct tm timeinfo;
            // First connect to NTP server, with 0 TZ offset
            configTime(0, 0, "pool.ntp.org");
            if (!getLocalTime(&timeinfo)) {
                throw std::runtime_error("failed to get local time");
            }
            // Now we can set the real timezone
            setTimezone(timezone);
        };

        /**
         * Called when the mode is activated (switched to).
        */
        void activate(Registers& registers) {};

        /**
         * Called in the main loop when the mode is active.
        */
        void loop(Registers& registers) {
            // Get the latest time.
            struct tm timeinfo;
            if (!getLocalTime(&timeinfo)) {
                throw std::runtime_error("failed to get local time");
            }
            registers.asStruct.register1 = timeinfo.tm_hour;
            registers.asStruct.register2 = timeinfo.tm_min;
            registers.asStruct.register3 = timeinfo.tm_sec;
        };

        /**
         * Called when the mode is deactivated (switched away from).
        */
        void deactivate(Registers& registers) {};
};