#ifndef DISPLAY_CONTROLLER_H_
#define DISPLAY_CONTROLLER_H_

#include "preference_controller.h"

class DisplayController {
    public:
        DisplayController(bool flip180);
        void updateDisplayWithPicture(uint8_t *received);
        ~DisplayController();
    private:
        uint8_t* m_framebuffer;
        void intUpdateDisplay(uint32_t delay_ms);
        bool m_flip180;
};

#endif