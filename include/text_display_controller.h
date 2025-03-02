#ifndef TEXT_DISPLAY_CONTROLLER_H_
#define TEXT_DISPLAY_CONTROLLER_H_

class TextDisplayController {
    public:
        TextDisplayController();
        void updateDisplayNoWifi();
        void updateDisplayNotConfigured();
        void updateDisplayNotRegistred(String endpointName, String macAddresse);
        ~TextDisplayController();
    private:
        void writeHeader();
};

#endif