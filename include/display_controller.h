#ifndef DISPLAY_CONTROLLER_H_
#define DISPLAY_CONTROLLER_H_

#include "preference_controller.h"

class DisplayController {
    public:
        DisplayController();
        void updateDisplayWithPicture(uint8_t *received);
        void updateDisplayWithPreferenceData(preferenceData preferenceData);
        ~DisplayController();
    private:
        uint8_t* m_framebuffer;
        void intUpdateDisplay(uint32_t delay_ms);
};

#endif