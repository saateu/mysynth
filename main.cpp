#include <iostream>
#include <alsa/asoundlib.h>

int main (int argc, char *argv[]) {
  int rate = 44100;
  unsigned exact_rate;
  int periods = 2;
  int num_frames;
  unsigned char *data;
  int l1, l2;
  short s1, s2;
  int frames;
  char *pcm_name;
  snd_pcm_t *pcm_handle; 
  snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_uframes_t periodsize = 8192;

  std::cout << "Start my synth\n";

  pcm_name = argv[1];

  snd_pcm_hw_params_alloca (&hwparams);

  if (snd_pcm_open (&pcm_handle, pcm_name, stream, 0) < 0) {
    std::cout << "open error\n";
    return -1;
  }

  if (snd_pcm_hw_params_any (pcm_handle, hwparams) < 0) {
    std::cout << "configure error\n";
    return -1;
  }

  if (snd_pcm_hw_params_set_access (pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
    std::cout << "error setting access\n";
    return -1;
  }

  if (snd_pcm_hw_params_set_format (pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
    std::cout << "error setting format\n";
    return -1;
  }

  exact_rate = rate;
  if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
    std::cout << "error setting rate\n";
    return -1;
  }

  if (rate != exact_rate) {
    std::cout << "rate != exact_rate";
  }

  /* Set number of channels */
  if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
    std::cout << "error setting channels\n";
    return -1;
  }

  if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
    std::cout << "error setting periods\n";
    return -1;
  }

  if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, (periodsize * periods)>>2) < 0) {
    std::cout << "error setting buffersize\n";
    return(-1);
  }

  if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
    std::cout << "error setting hw params\n";
    return(-1);
  }

  data = (unsigned char *)malloc(periodsize);
  frames = periodsize >> 2;
  for(l1 = 0; l1 < 100; l1++) {
    for(l2 = 0; l2 < num_frames; l2++) {
      s1 = (l2 % 128) * 100 - 5000;  
      s2 = (l2 % 256) * 100 - 5000;  
      data[4*l2] = (unsigned char)s1;
      data[4*l2+1] = s1 >> 8;
      data[4*l2+2] = (unsigned char)s2;
      data[4*l2+3] = s2 >> 8;
    }
    while ((snd_pcm_writei(pcm_handle, data, frames)) < 0) {
      snd_pcm_prepare(pcm_handle);
      fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
    }
  }

  snd_pcm_drop(pcm_handle);
  snd_pcm_drain(pcm_handle);
}
