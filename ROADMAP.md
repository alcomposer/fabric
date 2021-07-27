# Road map of Fabric development


### GUI:
- [ ] waveform display:
  - [x] display write head
  - [x] display play head
  - [ ] move write head with mouse (when stopped only?)
  - [ ] move play head with mouse (at all times)
  - [ ] show grains as boxes (to show both position, and length)
  - [x] show spray amount as box
  - [ ] options for spray:
    - left of playhead
    - right of playhead
    - center of playhead
- [ ] Volume control:
  - [ ] in volume (dry)
  - [ ] out volume (wet)
  - [ ] out mix control
- [ ] Buffer controls:
  - [x] rec active (button)
  - [ ] overdub active (button)
  - [ ] clear bufer (button)  
- [x] Grain playback control:
  - [x] Density
    - Option to adjust Density in Hz or Beats
    - Possibly have two seperate controls, and allow syncing between to allow ramping effects
  - [x] Readhead speed
  - [ ] Readhead sync to writehead (to use Fabric as a set'n'forget effect)
  - [ ] Readhead sync to writehead offset (as above)
  - [ ] Grain length
- [ ] Grain Envelope controls:
  - [ ] Sides (taken care of with Tukey window function)
  - [ ] Tilt (moving apex left/right)
  - [ ] Curve (move from a convex - straigh - concave shape)
- [ ] Grain direction:
  - [x] forward
  - [ ] reverse
- [ ] State:
  - [ ] Save/Load the current content of the audio buffer to a wav file 
  - [ ] Import wav files of different lengths
    - Read first 10 seconds, or allow user to move the audio file if longer?
  - [ ] Save the audio buffer state when record is off 

### DSP:
- [ ] Buffer Logic:
  - [ ] Circular buffer with xfade between writehead and readhead
- [ ] Envelope:
  - [ ] Grain Envelope with 3 Parameters: Sides, Tilt, Curve
- [ ] Volume:
  - [ ] control the output volume based on sqrt of active grains
- [ ] SinC Interpolation for sub-sample playback? (optional)