
#include <alsa/asoundlib.h>
#include "utils.h"

void MMA_rawmidi_devices_on_card(snd_ctl_t* ctl, int card);
void MMA_rawmidi_subdevice_info(snd_ctl_t* ctl, int card, int device);

static int is_input(snd_ctl_t* ctl, int card, int device, int sub);
static int is_output(snd_ctl_t* ctl, int card, int device, int sub);
