# Midi Mapper

### Tool for remapping Virtual Midi Devices
midi-mapper allows you to re-route and remap individual notes from your Midi Devices.

**Implemented in pure C**, midi-mapper accesses the low level audio API(s) for OS.
Designed for performance, it does one thing and hopes to do it well: midi mapping.

## Use Case:

For example, you can remap your drum pads to play in a particular octave - instead of the default C0-octave _(which is not easily modifiable, even in a Digital Audio Workstation)_

This can allow for a small 25-midi-key device to access it's drum pads to support a wider range of performances.
- Some 25-key midi devices come with up to 16 pressure sensitive drum pads! Take advantage of those!


## Features:
- Supports ALSA on Linux
- Supports CoreAudio for Mac OSX 

## Usage:

```
  Usage: midi-mapper
  -m, --monitor = <id:port> Monitor a MIDI Client
  -t, --target  = <id:port> Specify a target Midi Client (use with --send-note)
  -x, --source = <id:port> Specify a Midi Client Source for mapping from
  -s, --send = <note> Send midi note to a specifc client
  -r, --remap = <note:note> Remap a list of note<>note

  -l, --list     List all available midi clients
  -h, --help     Show application usage
  -v, --version  Print version

```
