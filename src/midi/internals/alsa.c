#include "midi/internals/alsa.h"
#ifdef __linux__

#include <alsa/asoundlib.h>
#include <ncurses.h>
#include <stdlib.h>

void iterate_midi_devices_on_card();
void iterate_subdevice_info();
int is_input();
int is_output();

Devices *MMAlsa_GetDevices() {
  Devices *devices = malloc(sizeof(Devices));
  devices->count = 0;

  int card = -1;
  int status = 0;

  if ((status = snd_card_next(&card)) < 0) {
    printf("Failed to retrieve midi data");
  }

  if (card < 0) {
    printf("No available sound cards found %s\n", snd_strerror(status));
    return devices;
  }

  snd_ctl_t *ctl;

  while (card >= 0) {
    iterate_midi_devices_on_card(ctl, card);

    if ((status = snd_card_next(&card)) < 0) {
      printf("No other cards available: %s", snd_strerror(status));
      break;
    }
  }

  return devices;
}

Device *MMAlsa_CreateVirtualDevice(char *name) {
  /* printw("Creating virtual device: alsa\n"); */
  Device *dev;
  return dev;
}

void iterate_midi_devices_on_card(snd_ctl_t *ctl, int card) {
  int device = -1;
  char name[32];
  int status;

  sprintf(name, "hw:%d"  , card);

  if ((status = snd_ctl_open(&ctl, name, 0)) < 0) {
    printf("Could not open ctl for sound card: %s\n", name);

  } else {
    /* printf("snd_ctl_opened: %s\n", name); */
    do {
      status = snd_ctl_rawmidi_next_device(ctl, &device);

      if (status < 0) {
        printf("failed to determined device number\n");
        break;
      }

      if (device >= 0) {
        iterate_subdevice_info(ctl, card, device);

      } else {
        /* printf("unable to open rawmidi for device: %s %d\n", */
        /*        name, device); */
      }
    } while (device >= 0);
  }
}

void iterate_subdevice_info(snd_ctl_t *ctl, int card, int device) {
  printf("reading subdevice info\n");
  snd_rawmidi_info_t *info;

  const char *name;
  const char *sub_name;

  int subs, subs_in, subs_out;
  int sub, in, out;
  int status;

  // prepare info for subs INPUT query
  snd_rawmidi_info_alloca(&info);
  snd_rawmidi_info_set_device(info, device);
  snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);

  snd_ctl_rawmidi_info(ctl, info);
  subs_in = snd_rawmidi_info_get_subdevices_count(info);

  // prepare info for subs OUTPUT query
  snd_rawmidi_info_alloca(&info);
  snd_rawmidi_info_set_device(info, device);
  snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);

  snd_ctl_rawmidi_info(ctl, info);
  subs_out = snd_rawmidi_info_get_subdevices_count(info);

  printf("hw:%d -> subs_in: %d\n", card, subs_in);
  printf("hw:%d -> subs_out: %d\n", card, subs_out);

  // determine the upper boundary for info searching
  subs = subs_in > subs_out ? subs_in : subs_out;

  sub = 0;
  in = out = 0;

  if ((status = is_output(ctl, card, device, sub)) < 0) {
    // is_output failed with a status code
    printf("cannot get rawmidi sub output information %d:%d %s",
           card, device, snd_strerror(status));
    return;
  } else if (status) {
    // is_output determined the sub is an output
    out = 1;
  }

  // if is_output did not determine outputness, try is_input
  if (status) {
    if ((status = is_input(ctl, card, device, sub)) < 0) {
      // is_output failed with a status code
      printf("cannot get rawmidi sub input information %d:%d %s",
             card, device, snd_strerror(status));
      return;
    } else if (status) {
      // is_output determined the sub is an output
      in = 1;
    }
  }

  printf("hw:%d -> in: %d / out: %d\n", card, in, out);

  // still no luck, we need to return..
  if (status == 0) {
    printf("Could not determine any subputs\n\n");
    return;
  }

  name = snd_rawmidi_info_get_name(info);
  sub_name = snd_rawmidi_info_get_subdevice_name(info);
  printf("name: %s\n      %s\n\n", name, sub_name);
}

// Returns true if given card/device/sub can output MIDI
int is_output(snd_ctl_t *ctl, int card, int device, int sub) {
  snd_rawmidi_info_t *info;
  int status;

  snd_rawmidi_info_alloca(&info);
  snd_rawmidi_info_set_device(info, device);
  snd_rawmidi_info_set_subdevice(info, sub);
  snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);

  if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0 && status != -ENXIO) {
    return status; // failed, return status code for parsing
  } else if (status == 0) {
    // we got a match, return success
    return 1;
  }

  // nope
  return 0;
}

// Returns true if specify card/device/sub can input MIDI
int is_input(snd_ctl_t *ctl, int card, int device, int sub) {
  snd_rawmidi_info_t *info;
  int status;

  snd_rawmidi_info_alloca(&info);
  snd_rawmidi_info_set_device(info, device);
  snd_rawmidi_info_set_subdevice(info, sub);
  snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);

  if ((status = snd_ctl_rawmidi_info(ctl, info)) < 0 && status != -ENXIO) {
    return status; // failed, return status code for parsing
  } else if (status == 0) {
    // we got a match, return success
    return 1;
  }

  // nope
  return 0;

}

#endif
