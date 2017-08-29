# Midi Mapper

### Tool for creating Smart Virtual Midi Devices
midi-mapper allows you to re-route and remap individual notes from your Midi Devices.

**Implemented in pure C**, midi-mapper accesses the low level audio API(s) for OS.
Designed for performance, it does one thing and hopes to do it well: midi mapping.

## Features:
- Create flexible virtual midi ports
- Create mapping definitions, with support for nesting and many-to-many routes.
- Monitors hardware and remaps on the fly to other ports
- Low level integration with raw midi/sequencer API(s) on Linux / OSX

## Use Case:

For example, you can remap your drum pads to play in a particular octave - instead of the default C0-octave _(which is not easily modifiable, even in a Digital Audio Workstation)_

This can allow for a small 25-midi-key device to access it's drum pads to support a wider range of performances.
- Some 25-key midi devices come with up to 16 pressure sensitive drum pads! Take advantage of those!


## Usage:

```
Usage: midi-mapper [options] [Mapping]

[Mapping] consists of a comma delimited list of notes.
Within that list, you may nest sub lists to specify
a group of notes to be executed per src trigger

Example:
midi-mapper -x 32:0  C3:C3|C4|C5,F#3:F#4

This will create a virtual interface attached to the device port 32:0.
Then it will map two keys (C3 and F#3) to trigger multiple other keys

Options:
  -s, --send=<note>       Send midi note to a specifc client
  -t, --target=<id:port>  Specify a target Midi Client
  -x, --source=<id:port>  Specify a Midi Client source

  -m, --monitor           Monitor a MIDI Client
  -l, --list     List all available midi clients
  -z, --debug    Turn on debug mode

  -h, --help     Show application usage
  -v, --version  Print version


```



## Installation:

Midi Mapper depends only on audio API(s) provided by your OS.
ALSA for Linux, and CoreAudio on OSX.


CMake is used to build the application.

Linux (Ubuntu):
```sudo apt-get install cmake```


OSX:
```brew install cmake```

Get the code and build it with the automated **bin/install** script.

```
git clone https://github.com/cosier/midi-mapper
cd midi-mapper
bin/install
```


## Authors:
In no particular order,

- Bailey Cosier <github.com/cosier>

Contributions / Pull requests / Issues are all welcome!
