#include "pad.h"

#include <cstdio>

// pad_dma_buf is provided by the user, one buf for each pad
// contains the pad's current state
static char padBuf[256] __attribute__((aligned(64)));

static int waitPadReady(int port, int slot) {
    int state;
    int lastState;
    char stateString[16];

    state = padGetState(port, slot);
    lastState = -1;
    while ((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
            std::printf("Please wait, pad(%d,%d) is in state %s\n", port, slot, stateString);
        }
        lastState = state;
        state = padGetState(port, slot);
    }
    // Were the pad ever 'out of sync'?
    if (lastState != -1) {
        std::printf("Pad OK!\n");
    }
    return 0;
}

int pad_init(int port, int slot) {
    int ret;
    int i;
    int modes;

    padInit(0);

    if ((ret = padPortOpen(port, slot, padBuf)) == 0) {
        std::printf("padOpenPort failed: %d\n", ret);
    }

    waitPadReady(port, slot);

    // How many different modes can this device operate in?
    // i.e. get # entrys in the modetable
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
    std::printf("The device has %d modes\n", modes);

    if (modes > 0) {
        std::printf("( ");
        for (i = 0; i < modes; i++) {
            std::printf("%d ", padInfoMode(port, slot, PAD_MODETABLE, i));
        }
        std::printf(")");
    }

    std::printf("It is currently using mode %d\n", padInfoMode(port, slot, PAD_MODECURID, 0));

    // If modes == 0, this is not a Dual shock controller
    // (it has no actuator engines)
    if (modes == 0) {
        std::printf("This is a digital controller?\n");
        return 1;
    }

    // Verify that the controller has a DUAL SHOCK mode
    i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK) break;
        i++;
    } while (i < modes);
    if (i >= modes) {
        std::printf("This is no Dual Shock controller\n");
        return 1;
    }

    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
        std::printf("This is no Dual Shock controller??\n");
        return 1;
    }

    std::printf("Enabling dual shock functions\n");

    // When using MMODE_LOCK, user cant change mode with Select button
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady(port, slot);
    std::printf("infoPressMode: %d\n", padInfoPressMode(port, slot));

    waitPadReady(port, slot);
    std::printf("enterPressMode: %d\n", padEnterPressMode(port, slot));

    return 1;
}

int pad_get_status(PadStatus *padStatus) {
    int ret = padRead(0, 0, &padStatus->pad);

    if (ret != 0) {
        padStatus->buttons = padStatus->pad.btns ^ 0xFFFF;
        padStatus->buttonsNew = padStatus->buttons & ~padStatus->buttonsPrevious;
        padStatus->buttonsPrevious = padStatus->buttons;
    } else {
        padStatus->buttons = 0;
        padStatus->buttonsNew = 0;
        padStatus->buttonsPrevious = 0;
    }

    return ret;
}