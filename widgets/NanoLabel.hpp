#ifndef WOLF_NANO_LABEL_HPP_INCLUDED
#define WOLF_NANO_LABEL_HPP_INCLUDED

#include "Widget.hpp"
#include "NanoVG.hpp"
#include "Margin.hpp"
#include "WolfWidget.hpp"
#include <string>

START_NAMESPACE_DISTRHO

class NanoLabel : public WolfWidget
{
  public:
    explicit NanoLabel(Widget  *widget, Size<uint> size) noexcept;

    void setFontSize(float fontSize);
    void setColor(Color color);
    void setMargin(Margin margin);
    void setAlign(int align); //NanoVG align
    void setText(const char *text);
    void setText(float valueText);     // we can't copy the address, we have to copy the actual text itself
    void setFontId(NanoVG::FontId fontId);

  protected:
    void onNanoDisplay() override;

  private:
    const char *fText;
    float valueText;
    std::string stringText;
    Margin fMargin;
    int fAlign;
    Color fColor;
    float fFontSize;
    NanoVG::FontId fFontId;

    DISTRHO_LEAK_DETECTOR(NanoLabel)
};

END_NAMESPACE_DISTRHO

#endif