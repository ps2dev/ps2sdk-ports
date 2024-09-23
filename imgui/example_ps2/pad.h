#include <libpad.h>
#include <tamtypes.h>

struct PadStatus {
    padButtonStatus pad;

    // Current status of the buttons
    u16 buttons;

    // Status of the buttons in the previous frame
    u16 buttonsPrevious;

    // Only buttons which are new since the previous frame
    u16 buttonsNew;
};

int pad_init(int port = 0, int slot = 0);
int pad_get_status(PadStatus *padStatus);