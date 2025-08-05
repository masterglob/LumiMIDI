# LED Effects Palette - Base and Secondary Programs

## Base Effects

| Name | Type | Technical Description | Applicable Controls |
|------|------|----------------------|-------------------|
| **Breathing** | Base | Global sinusoidal brightness variation across all LEDs. Uses main color with intensity modulation. No position dependency. | Main color, Speed, Amplitude (breathing depth) |
| **Beat** | Base | 50Hz RMS tracking with hysteresis filtering. Global flash on beat detection. Intensity hold according to decay/hold settings. | Main color, Threshold, Decay, Hold, Sensitivity |
| **Multi-Beat** | Base | 4 LED plane zones assigned to 4 frequency bands (50Hz, 300Hz, 1kHz, 3kHz). Each zone reacts to its band with specific color. | 4 band colors, Global threshold, Sensitivity, Zone mapping |
| **Color Wave** | Base | Color wave crossing the plane left to right with colorimetric gradient. X position determines hue in spectrum defined around main color. | Main color, Speed, Gradient width, Color range |
| **Color Fade** | Base | Smooth continuous transition between main color and adjacent colors on chromatic circle. Global effect without spatial dependency. | Main color, Speed, Fade range, Saturation |
| **Radial Pulse** | Base | Circular pulse from plane center outward. Euclidean distance calculation from center. Main color with radial attenuation. | Main color, Speed, Center X/Y, Pulse width |
| **Corner Sweep** | Base | Rotational sweep from the 4 plane corners. Angle calculation relative to corners, sequential activation. | Main color, Speed, Corner intensity, Sweep width |
| **Breathing Colors** | Base | Like Breathing but with slow rotation on chromatic circle. Combines intensity and hue modulation. | Starting color, Speed, Color rotation speed, Amplitude |
| **Color Pulse** | Base | Periodic color shifts between main color and complementary color. Sharp transitions with smooth fades. Global effect. | Main color, Speed, Transition sharpness, Hold time |
| **Warm/Cool Cycle** | Base | Oscillation between warm and cool versions of main color. Temperature shift on color wheel while maintaining hue family. | Main color, Speed, Temperature range, Saturation |
| **Saturation Wave** | Base | Main color with varying saturation levels. Creates rich-to-pale color breathing while keeping same hue. | Main color, Speed, Saturation range, Minimum saturation |
| **Rainbow Slow** | Base | Very slow progression through entire color spectrum. Each LED shows same color, spectrum advances over time. | Speed, Saturation, Brightness, Start hue offset |
| **Audio Reactive** | Base | Global intensity driven by global RMS (sum of 4 bands). Real-time reactivity with smoothing. | Main color, Sensitivity, Attack, Release, Boost |
| **Solid Fill** | Base | Simple static fill of all LEDs with main color. Uniform intensity across entire plane. No position dependency. | Main color, Intensity |
| **Gradient Fill** | Base | Linear gradient from one side to opposite. Color interpolation between main color and secondary color based on X or Y position. | Main color, Secondary color, Direction (H/V), Gradient steepness |
| **Checkerboard** | Base | Alternating pattern of main color and off/secondary color. Grid size configurable. Position dependency: (X+Y) modulo pattern. | Main color, Secondary color, Grid size, Offset |
| **Diagonal Sweep** | Base | Diagonal bands moving across the plane. Color based on (X+Y+time) modulo calculation. Creates moving stripe pattern. | Main color, Speed, Band width, Direction |
| **Alternate Blink** | Base | Alternating activation of LEDs in checkerboard pattern. Phase offset creates blinking between two groups. | Main color, Secondary color, Speed, Phase offset |
| **Ping Pong** | Base | Single LED or small group moving back and forth across X or Y axis. Position calculated with triangle wave function. | Main color, Speed, Trail length, Direction (H/V) |
| **Random Fill** | Base | Random LEDs turn on/off with main color. Each LED has independent timing. Creates organic, firefly-like effect. | Main color, Density, Speed variation, Hold time |

## Secondary Effects

| Name | Type | Technical Description | Applicable Controls |
|------|------|----------------------|-------------------|
| **Strobe** | Secondary | Simultaneous flash on White component. Fixed frequency according to Speed. Complete W override on all active LEDs. | Speed, Intensity |
| **Wave White** | Secondary | White wave from right to left then back. Auto-stop after 1 round trip. W override according to X position. | Speed, Amplitude |
| **Sparkle** | Secondary | Random blinking of individual LEDs on White component. Continuous probabilistic selection. | Density, Speed, Intensity |
| **Lightning** | Secondary | Ultra-fast flash (1-3 frames) on White with irregular pattern. Lightning simulation with random timing. Auto-stop. | Intensity, Pattern randomness |
| **Edge Glow** | Secondary | Highlight of LEDs on plane periphery. Automatic detection of border LEDs. Addition on all RGBW components. | Intensity, Width, Color tint |
| **Center Spot** | Secondary | Centered circular spot with radial gradient. Distance calculation from center. RGBW addition with attenuation. | Size, Intensity, Color tint |
| **Beat Flash** | Secondary | Flash on beat detection (50Hz RMS). Temporary RGBW override. More reactive than base Beat. | Threshold, Flash duration, Intensity |
| **Random Pop** | Secondary | Random activation of individual LEDs with fast fade-in/out. Random spatial pattern. Complete temporary override. | Frequency, Hold time, Intensity |
| **Strobe Zone** | Secondary | Strobe on defined rectangular zone. White override in specified zone. | Zone X/Y/W/H, Speed, Intensity |
| **Color Accent** | Secondary | Addition of accent color on randomly selected LEDs. Additive mixing with base effect. | Accent color, Density, Intensity |
| **Mirror Flash** | Secondary | Symmetrical flash on opposite plane edges. RGBW override on mirror zones. Auto-stop. | Duration, Intensity, Symmetry axis |
| **Frequency Burst** | Secondary | Burst of random LEDs triggered by peak in selected frequency band. | Frequency band, Threshold, Burst size, Duration |

## Technical Notes

### Control System
- **Main color**: HSV or RGB, base for derived color calculations
- **Speed**: Speed multiplier (0.1x to 10x)
- **Intensity**: Global intensity multiplier (0% to 200%)
- **Density**: Activation probability for random effects
- **Threshold**: Trigger threshold for audio-reactive effects

### 2D Position Management
- Normalized coordinates (0,0) to (1,1)
- Euclidean distance calculations for radial effects
- Automatic detection of borders and center
- Support for user-defined zones

### Audio Reactivity
- Global RMS = weighted average of 4 bands
- Configurable temporal smoothing (attack/release)
- Adaptive thresholds with hysteresis
- Logarithmic mapping for natural perception