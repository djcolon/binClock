#include <gtest/gtest.h>
#include "modeManager.cpp"
#include "registers.h"
#include "modes/allOn.cpp"
#include "modes/pingPong.cpp"
#include "modeInterface.h"

/**
 * Tests registration of a mode.
*/
TEST(TestModeManager, TestRegisterSingle) {
    ModeManager modeManager;
    AllOn* allOn = new AllOn();
    modeManager.registerMode(allOn);
    EXPECT_EQ(modeManager.getActiveMode(), allOn);
    EXPECT_EQ(modeManager.getHighestModeIndex(), 1);
    EXPECT_EQ(modeManager.getActiveMode()->getId(), 0);
    EXPECT_EQ(modeManager.getActiveMode()->getFriendlyName(), "All on");
}

/**
 * Tests registration of multiple modes.
*/
TEST(TestModeManager, TestRegisterMultiple) {
    ModeManager modeManager;
    AllOn* allOn = new AllOn();
    modeManager.registerMode(allOn);
    modeManager.registerMode(new PingPong());
    EXPECT_EQ(modeManager.getActiveMode(), allOn);
    EXPECT_EQ(modeManager.getHighestModeIndex(), 2);
    EXPECT_EQ(modeManager.getActiveMode()->getId(), 0);
    EXPECT_EQ(modeManager.getActiveMode()->getFriendlyName(), "All on");
}

/**
 * Tests registration of a mode in a different scope using new.
*/
void directInitHelper(ModeManager* modeManager) {
    modeManager->registerMode(new AllOn());
}

TEST(TestModeManager, TestDirectInit) {
    ModeManager modeManager;
    directInitHelper(&modeManager);
    EXPECT_EQ(modeManager.getActiveMode()->getFriendlyName(), "All on");
}

/**
 * Tests nextMode.
*/
TEST(TestModeManager, TestNextMode) {
    ModeManager modeManager;
    Registers registers;
    registers.asInt = 0;

    modeManager.registerMode(new AllOn());
    modeManager.registerMode(new PingPong());
    EXPECT_EQ(modeManager.getActiveMode()->getFriendlyName(), "All on");
    modeManager.nextMode(registers);
    EXPECT_EQ(modeManager.getActiveMode()->getFriendlyName(), "Ping pong");
    EXPECT_EQ(registers.asInt, 1) << "Ping pong activate not run.";
}

/**
 * Tests calling loop on active mode.
*/
TEST(TestModeManager, TestLoop) {
    Registers registers;
    registers.asInt = 0;
    ModeManager modeManager;

    modeManager.registerMode(new AllOn());
    modeManager.setup(registers);

    modeManager.loopActiveMode(registers, 10);

    EXPECT_EQ(registers.asInt, UINT32_MAX);
}

/**
 * Tests loop non-blocking delay works.
*/
TEST(TestModeManager, TestLoopDelay) {
    Registers registers;
    registers.asInt = 0;
    ModeManager modeManager;

    modeManager.registerMode(new PingPong());
    modeManager.setup(registers);

    modeManager.loopActiveMode(registers, 0);
    EXPECT_EQ(registers.asInt, (uint64_t)0b00000001);
    modeManager.loopActiveMode(registers, 100);
    EXPECT_EQ(registers.asInt, (uint64_t)0b00000001);
    modeManager.loopActiveMode(registers, 201);
    EXPECT_EQ(registers.asInt, (uint64_t)0b00000010);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
    ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}