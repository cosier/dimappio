#include "midi/output.h"

void dm_output_free(dm_midi_output* output) {
    // for (int i = 0; i < output->out_count; ++i) {
    //     free(output->out_ports[i]);
    // }

    // for (int i = 0; i < output->in_count; ++i) {
    //     free(output->in_ports[i]);
    // }

    snd_seq_close(output->dev);

    free(output->in_ports);
    free(output->out_ports);
    free(output);
}
