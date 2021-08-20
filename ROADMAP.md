# Road map of Fabric development


### GUI:
- [ ] waveform display:
  - [x] display write head
  - [x] display play head
  - [ ] move write head with mouse (when stopped only?)
  - [ ] move play head with mouse (at all times, relative only - no jumping)
  - [x] show spray amount visually 
  - [ ] spray offset amount (-1, 0, 1)
- [ ] Volume control:
  - [ ] in volume (dry)
  - [ ] out volume (wet)
  - [x] out mix control
- [ ] Buffer controls:
  - [x] rec active (button)
  - [ ] overdub active (button)
  - [ ] clear bufer (button)  
- [x] Grain playback control:
  - [x] Density
    - Option to adjust Density in Hz or Beats
    - Possibly have two seperate controls, and allow syncing between to allow ramping effects
  - [x] Readhead speed (-5x, 5x)
  - [ ] Readhead sync to writehead (to use Fabric as a set'n'forget effect)
  - [ ] Readhead sync to writehead offset (as above)
  - [x] Grain length
  - [x] Grain Pitch Spray control
  - [x] Grain Pitch
- [ ] Stereo image control:
  - [ ] Pan spray (L & R)
  - [ ] Stereo Width
- [ ] Grain Envelope controls:
  - [x] Sides (taken care of with Tukey window function)
  - [x] Tilt (moving apex left/right)
  - [ ] Curve (move from a convex - straigh - concave shape)
- [ ] Grain direction:
  - [x] forward
  - [x] reverse
  - [x] Implemented as direction forward/reverse factor
- [ ] State:
  - [ ] Save/Load the current content of the audio buffer to a wav file 
  - [ ] Import wav files of different lengths
    - Read first 10 seconds, or allow user to move the audio file if longer?
  - [ ] Save the audio buffer state when record is off 

### DSP:
- [ ] Buffer Logic:
  - [ ] Circular buffer with xfade between writehead and readhead
- [x] Envelope:
  - [x] Grain Envelope with 2 Parameters: Sides, Tilt
- [ ] Volume:
  - [ ] control the output volume based on sqrt of active grains
- [ ] SinC Interpolation for sub-sample playback? (optional)
- [x] Lerp Interpolatioin for sub-sample playback