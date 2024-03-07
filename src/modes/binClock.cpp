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
         * Mode id, set by ModeManager. Treat as read-only.
        */
        uint8_t id = 0;
        /**
         * Name for this mode used to display to users.
        */
       std::string friendlyName = "Mode template";
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

            delay(100);
        };

        /**
         * Called when the mode is deactivated (switched away from).
        */
        void deactivate(Registers& registers) {};
};