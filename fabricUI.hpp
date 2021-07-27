#pragma once

#include "DistrhoUI.hpp"

START_NAMESPACE_DISTRHO

class fabricUI : public UI,
                 public IdleCallback
{
public:
    fabricUI();
    void idleCallback();

    /* DSP/Plugin Callbacks */
protected:
    /**
        A parameter has changed on the plugin side.
        This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override;
    /**
        A state has changed on the plugin side.
        This is called by the host to inform the UI about state changes.
    */
    void stateChanged(const char*, const char*) override;

    /* Widget Callbacks */
    /**
        The NanoVG drawing function.
    */
    void onNanoDisplay() override;
    /**
        Mouse press event.
        This UI will change color when clicked.
    */
    bool onMouse(const MouseEvent& ev) override;
private:
    /**
        Meter values.
        These are the parameter outputs from the DSP side.
    */
    float fOutLeft, fOutRight;

    /**
        Color and its matching parameter value.
    */
    Color fColor;
    int   fColorValue;
    /**
        Update color if needed.
    */
    void updateColor(const int color);
    /**
        Set our UI class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(fabricUI);
};

END_NAMESPACE_DISTRHO
