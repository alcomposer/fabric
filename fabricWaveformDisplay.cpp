/*
 * Fabric Granular Audio Effect
 * Copyright (C) 2021 Alexander Waite Mitchell <alex.w.mitchell@gmail.com>
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
    setSize(size); 
    _parent = static_cast<fabricUI *>(widget);
    _st_audioBuffer = _parent->_plugin->st_audioBuffer;
    _writeHeadPos = &_parent->_plugin->bufferPos;
    _sizeOfBuffer = _parent->_plugin->st_audioBufferSize;
    _playHeadPos = &_parent->_plugin->grainPlayer.controls.playHeadPos;
    m_spray = &_parent->_plugin->grainPlayer.controls.spray;
}

void fabricWaveformDisplay::onNanoDisplay()
{
    Color blue = Color(173, 216, 230); //FIXME (alex) make this an enum 
    Color blue_transparent = Color(173, 216, 230, 0.3f);
    Color blue_transparent_2 = Color(173, 216, 230, 0.12f);
    Color green_transparent = Color(173, 230, 173, 0.3f);
    //draw black widget background
    static const Color k_black(0, 0, 0);
    beginPath();
    rect(0.0f, 0.0f, getWidth(), getHeight());
    fillColor(k_black);
    fill();
    
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
    strokeWidth(1.f);
    moveTo(display_left, display_center);

    for (uint16_t i = 0; i < DISPLAY_WIDTH; i++)
    {
        fIndex = (float)i / DISPLAY_WIDTH;
        sampleIndex = floor(fIndex * _sizeOfBuffer);
        lineTo(i, display_center + (_st_audioBuffer[0][sampleIndex] + _st_audioBuffer[1][sampleIndex]) / 2.0 * display_center);
    }
    stroke();
    
    // center line
    beginPath();
    moveTo(display_left, display_center);
    lineTo(display_right, display_center);
    stroke();
    
    // draw spray region
    bool left_draw_overflow = false;
    bool right_draw_overflow = false;
    float left_spray_x_overflow = 0.0;
    float right_spray_x_overflow = 0.0;
    float barSize = 5.f;
    
    float playHeadPos = (float)(*_playHeadPos) / _sizeOfBuffer * display_right;
    float sprayOffset = (float)(*m_spray / 10000) * .5f * display_right;
    
    float left_spray_x = playHeadPos - sprayOffset;
    float right_spray_x = playHeadPos + sprayOffset;
    if (left_spray_x < 0.0 + display_left){ 
        left_spray_x_overflow = left_spray_x + getWidth();
        left_spray_x = display_left + 0.0;
        left_draw_overflow = true;
    };
    if (right_spray_x > getWidth() + display_left){ 
        right_spray_x_overflow = right_spray_x - getWidth();
        right_spray_x = display_left + getWidth();
        right_draw_overflow = true;
    };

    // draw overflow region of spray
    if (left_draw_overflow){
        beginPath();
        fillColor(blue);
        moveTo(left_spray_x_overflow, 0);
        lineTo(left_spray_x_overflow, barSize);
        lineTo(getWidth(), barSize);
        lineTo(getWidth(), 0);
        fill();   
                
        beginPath();
        fillColor(blue_transparent_2);
        moveTo(left_spray_x_overflow, 0);
        lineTo(left_spray_x_overflow, display_bottom);
        lineTo(getWidth(), display_bottom);
        lineTo(getWidth(), 0);
        fill();   
    }  
    if (right_draw_overflow){
        beginPath();
        fillColor(blue);
        moveTo(0, display_top);
        lineTo(0, barSize);
        lineTo(right_spray_x_overflow, barSize);
        lineTo(right_spray_x_overflow, 0);
        fill(); 

        beginPath();
        fillColor(blue_transparent_2);
        moveTo(0, display_top);
        lineTo(0, display_bottom);
        lineTo(right_spray_x_overflow, display_bottom);
        lineTo(right_spray_x_overflow, 0);
        fill(); 
    }  
    // draw standard spray region
    beginPath();
    fillColor(blue);
    moveTo(left_spray_x, 0);
    lineTo(left_spray_x, barSize);
    lineTo(right_spray_x, barSize);
    lineTo(right_spray_x, 0);
    fill();

    beginPath();
    fillColor(blue_transparent_2);
    moveTo(left_spray_x, 0);
    lineTo(left_spray_x, display_bottom);
    lineTo(right_spray_x, display_bottom);
    lineTo(right_spray_x, 0);
    fill();

   // draw grains
    strokeWidth(2.0f);
    for (int i = 0; i < 128; i++){

        Grain &grain = _parent->_plugin->grainPlayer.grainArray[i];

        if (grain.m_playing == true){
            float grainPos = (float)grain.m_startTimeBuffer / (float)(_parent->getSampleRate()*10) * display_right;//FIXME (alex) don't calcuate the buffer time, use a define
            beginPath();
            if (grain.m_direction == Grain::Direction::forward){
                strokeColor(blue_transparent); 
            }else{
                strokeColor(green_transparent);
            }
            moveTo(grainPos, display_top); 
            lineTo(grainPos, display_bottom);
            stroke();
        }
    }

    // rec head line
    beginPath();
    strokeColor(255,0,0); //FIXME (alex) change to an enum- use same red as rec button
    strokeWidth(2.0f);
    float recheadPos = display_left + (float)(*_writeHeadPos) / _sizeOfBuffer * display_right;
    moveTo(recheadPos, display_top);
    lineTo(recheadPos, display_bottom);
    stroke();
    
    // play head line
    beginPath();
    strokeColor(blue);
    strokeWidth(2.0f);
    float playheadPos = (float)(*_playHeadPos) / _sizeOfBuffer * display_right;
    moveTo(playheadPos, display_top);
    lineTo(playheadPos, display_bottom);
    stroke();
}



END_NAMESPACE_DISTRHO
