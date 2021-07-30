/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2019 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "fabricWaveformDisplay.hpp"
#include "DistrhoPluginInfo.h"
#include "fabricUI.hpp"
#include <iostream>

START_NAMESPACE_DISTRHO

fabricWaveformDisplay::fabricWaveformDisplay(Widget *widget, Size<uint> size) noexcept
    : NanoSubWidget(widget)
{
    Window &pw = getWindow(); //this is needed to refresh the waveform display
    pw.addIdleCallback(this, 10);

    setSize(size); 
    _parent = static_cast<fabricUI *>(widget);
    _waveformPtr = &_parent->_plugin->waveForm;
}

void fabricWaveformDisplay::idleCallback()
{
    repaint();
}

void fabricWaveformDisplay::onNanoDisplay()
{
    //draw black widget background
    static const Color k_black(0, 0, 0);
    beginPath();
    rect(0.0f, 0.0f, getWidth(), getHeight());
    fillColor(k_black);
    fill();
    closePath();
    
    float display_center = getHeight() / 2.0;
    float display_left = 0.0f;
    float display_right = getWidth();


    double samples_per_pixel = 1.0; // FIXME still connected to scroll zoom somehow
    float fIndex;
    uint iIndex;

    fIndex = float(0) + float(samples_per_pixel);
    iIndex = fIndex;
    
        //std::cout << "drawing waveform" << std::endl;

        beginPath();
        strokeColor({255,0,0});
        strokeWidth(1.0f);
        moveTo(0, getHeight()/2.0);


    
        for (uint16_t i = 0; i < DISPLAY_WIDTH; i++)
        {
            fIndex = float(0) + (float(i) * samples_per_pixel);
            iIndex = fIndex;
            auto minmax = std::minmax_element(_waveformPtr->begin() + iIndex, _waveformPtr->begin() + iIndex + int(samples_per_pixel));
            uint16_t min = *minmax.first + display_center;
            uint16_t max = *minmax.second + display_center;
            lineTo(i + display_left, min);
            lineTo(i + display_left, max);
        }
        stroke();
        closePath();
        // center line
        beginPath();
        moveTo(display_left, display_center);
        lineTo(display_right, display_center);
        stroke();
        closePath();

        /*
        // draw spray region
        //beginPath();
        //strokeColor(0,0,255,30); //change to an enum
        //strokeWidth(2.0f);
        fillColor(0,0,255,30);
        bool left_draw_overflow = false;
        bool right_draw_overflow = false;

        float left_spray_x_overflow = 0.0;
        float right_spray_x_overflow = 0.0;
        
        float playHeadPos = display_left + (float)plugin->playheadPos / plugin->st_audioBuffer.size() * display_width;
        float sprayOffset = plugin->fSpray * 0.5 * display_width;
        
        float left_spray_x = playHeadPos - sprayOffset;
        float right_spray_x = playHeadPos + sprayOffset;

        if (left_spray_x < 0.0 + display_left){ 
            left_spray_x_overflow = left_spray_x + display_width;
            left_spray_x = display_left + 0.0;
            left_draw_overflow = true;
        };
        if (right_spray_x > display_width + display_left){ 
            right_spray_x_overflow = right_spray_x - display_width;
            right_spray_x = display_left + display_width;
            right_draw_overflow = true;
        };
        // draw overflow region
        if (left_draw_overflow){
            beginPath();
            moveTo(left_spray_x_overflow, display_top);
            lineTo(left_spray_x_overflow, display_bottom);
            lineTo(display_left + display_width, display_bottom);
            lineTo(display_left + display_width, display_top);
            fill();
            closePath();     
        }  
        if (right_draw_overflow){
            beginPath();
            moveTo(display_left, display_top);
            lineTo(display_left, display_bottom);
            lineTo(right_spray_x_overflow, display_bottom);
            lineTo(right_spray_x_overflow, display_top);
            fill();
            closePath();     
        }  
        // draw standard region
        beginPath();
        moveTo(left_spray_x, display_top);
        lineTo(left_spray_x, display_bottom);
        lineTo(right_spray_x, display_bottom);
        lineTo(right_spray_x, display_top);
        fill();
        closePath();


        // draw grains
        beginPath();
        strokeColor(0,0,255,150); //change to an enum
        strokeWidth(2.0f);
        for (int i = 0; i < 128; i++){
            //std::cout << "drawing grain number: " << i << std::endl;
            if (plugin->grainPlayer.grain_array[i].erase_me == false){
                float grainPos = display_left + plugin->grainPlayer.grain_array[i].start_position * display_width;
                //std::cout << "drawing grain at: " << (float)plugin->grainPlayer.grain_array[i].start_position << std::endl;
                moveTo(grainPos, display_center - 25);
                lineTo(grainPos, display_center + 25);
            }
        }
        stroke();
        closePath();


        // rec head line
        beginPath();
        strokeColor(255,0,0,200); //change to an enum
        strokeWidth(2.0f);
        float recheadPos = display_left + (float)plugin->bufferPos / plugin->st_audioBuffer.size() * display_width;
        moveTo(recheadPos, display_top);
        lineTo(recheadPos, display_bottom);
        stroke();
        closePath();

        // play head line
        beginPath();
        strokeColor(0,0,255,200); //change to an enum
        strokeWidth(2.0f);
        moveTo(playHeadPos, display_top);
        lineTo(playHeadPos, display_bottom);
        stroke();
        closePath();

        */
}



END_NAMESPACE_DISTRHO
