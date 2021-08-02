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
    _st_audioBuffer = _parent->_plugin->st_audioBuffer;
    _writeHeadPos = &_parent->_plugin->bufferPos;
    _sizeOfBuffer = _parent->_plugin->st_audioBufferSize;
    _playHeadPos = &_parent->_plugin->grainPlayer.controls.playHeadPos;
}

void fabricWaveformDisplay::idleCallback()
{
    repaint();
}

void fabricWaveformDisplay::onNanoDisplay()
{
    Color blue = Color(173, 216, 230); //FIXME (alex) make this an enum 
    Color blue_transparent = Color(173, 216, 230, 0.1f);
    //draw black widget background
    static const Color k_black(0, 0, 0);
    beginPath();
    rect(0.0f, 0.0f, getWidth(), getHeight());
    fillColor(k_black);
    fill();
    closePath();
    
    float display_top = 0.f;
    float display_bottom = getHeight();
    float display_center = display_bottom / 2.0;
    float display_left = 0.f;
    float display_right = getWidth();
    float fIndex;
    uint sampleIndex;
    
    
    //draw waveform
    beginPath();
    strokeColor(blue);
    strokeWidth(0.9f);

    for (uint16_t i = 0; i < DISPLAY_WIDTH; i++)
    {
        fIndex = (float)i / DISPLAY_WIDTH;
        sampleIndex = floor(fIndex * _sizeOfBuffer);
        moveTo(i, display_center);
        lineTo(i, display_center + _st_audioBuffer[0][sampleIndex] * display_center);
        moveTo(i, display_center);
        lineTo(i, display_center - _st_audioBuffer[1][sampleIndex] * display_center);
    }
    //fillColor({255,0,0});
    //fill();
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
    */

    // draw grains
    beginPath();
    strokeColor(blue_transparent); //change to an enum
    strokeWidth(2.0f);
    for (int i = 0; i < 128; i++){
        if (_parent->_plugin->grainPlayer.grainArray[i].playing == true){
            float grainPos = (float)_parent->_plugin->grainPlayer.grainArray[i].startTimeBuffer / (float)(44100*10) * display_right; //FIXME (alex) use sample rate calc please!
            //std::cout << "drawing grain at: " << (float)plugin->grainPlayer.grain_array[i].start_position << std::endl;
            moveTo(grainPos, display_top); //- 25);
            lineTo(grainPos, display_bottom); //+ 25);
        }
    }
    stroke();
    closePath();
    

    // rec head line
    beginPath();
    strokeColor(255,0,0); //FIXME (alex) change to an enum- use same red as rec button
    strokeWidth(2.0f);
    float recheadPos = display_left + (float)(*_writeHeadPos) / _sizeOfBuffer * display_right;
    moveTo(recheadPos, display_top);
    lineTo(recheadPos, display_bottom);
    stroke();
    closePath();

    
    // play head line
    beginPath();
    strokeColor(blue);
    strokeWidth(2.0f);
    float playheadPos = (float)(*_playHeadPos) / _sizeOfBuffer * display_right;
    moveTo(playheadPos, display_top);
    lineTo(playheadPos, display_bottom);
    stroke();
    closePath();
    
}



END_NAMESPACE_DISTRHO
