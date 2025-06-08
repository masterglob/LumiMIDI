# LumiMIDI

**LumiMIDI** is an open-source project designed to dynamically generate MIDI signals for controlling lighting systems (such as RGB/RGBW LEDs), using musical and audio analysis inside any DAW (Digital Audio Workstation). Inspired by tools like DMXIS but focused on generative and programmable visual effects, LumiMIDI acts as a plugin (VST/AU/AAX) that integrates seamlessly into your production or live performance environment.

---

## 🎯 Project Goals

- Enable real-time MIDI output for lighting control based on audio input.
- Integrate directly into DAWs as a VST/AU/AAX plugin.
- Support spatial and physical configuration of LEDs (RGB, RGBW, etc.).
- Provide a library of customizable, tempo-synced visual effects.
- Allow randomness and controlled variation to create organic light shows.
- Offer ways to trigger or override effects via MIDI or plugin parameters.

---

## ✨ Key Features

- **DAW Plugin Integration**  
  Runs as a plugin inside any compatible DAW, allowing real-time synchronization with audio tracks.

- **Dynamic MIDI Output**  
  Generates MIDI CC or Note data based on the plugin's effect engine, suitable for tools like DMXIS or other DMX-MIDI bridges.

- **LED Mapping Configuration**  
  Define the spatial layout and type of each LED or lighting fixture to drive effects accurately.

- **Effect Engine**  
  Includes built-in visual effects (pulses, waves, strobe, color shifts, etc.) that react to:
  - Beat detection
  - Frequency bands
  - Harmony
  - Tempo and time signature

- **Controlled Randomness**  
  Allows variation in effects while maintaining musical coherence. Think "generative but intentional."

- **Sequencing & Triggers**  
  Effects can be triggered manually, by automation, or MIDI notes/CC to match specific musical moments or transitions.

---

## 🛠 Use Cases

- Live music performances with synchronized lights
- Audio-reactive visual shows in clubs or festivals
- Automatic light shows for home DJs or streaming setups
- Composers and producers adding light control to their DAW workflow

---

## 🚧 Roadmap (Planned)

- [ ] Core MIDI generation engine
- [ ] Audio analysis and beat detection
- [ ] Plugin UI with LED layout editor
- [ ] Preset system for effects
- [ ] Integration with common lighting plugins (e.g. DMXIS, Lightjams)
- [ ] Scripting support for custom effects (future)

---

## 💬 Contributions

Contributions are welcome! Whether you’re a developer, designer, or lighting artist, feel free to open issues, suggest features, or submit pull requests.

---

## 📄 License

This project is open-source and available under the MIT License. See the [LICENSE](./LICENSE) file for more details.

