#include <iostream>
#include <math.h>
#include <alsa/asoundlib.h>

#define SAMPLING_RATE 44100
#define PCM_FORMAT SND_PCM_FORMAT_FLOAT
#define HELZ 440
#define PERIODS 4
#define PCM_NAME "default"
#define PERIOD_SIZE 262144
#define NUM_LOOP 100
#define MODE_FLOAT true
#define DEBUG false

int init_alsa (snd_pcm_t *pcm_handle, snd_pcm_uframes_t periodsize,
    int periods,int rate) {
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_uframes_t bufsize = periodsize * periods;

  snd_pcm_hw_params_alloca (&hwparams);

  if (snd_pcm_hw_params_any (pcm_handle, hwparams) < 0) {
    std::cout << "configure error\n";
    return -1;
  }

  if (snd_pcm_hw_params_set_access (pcm_handle, hwparams,
      SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
    std::cout << "error setting access\n";
    return -1;
  }

  if (snd_pcm_hw_params_set_format (pcm_handle, hwparams, PCM_FORMAT) < 0) {
    std::cout << "error setting format\n";
    return -1;
  }

  if (snd_pcm_hw_params_set_rate(pcm_handle, hwparams,
      rate, 0) < 0) {
    std::cout << "error setting rate\n";
    return -1;
  }

  if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
    std::cout << "error setting channels\n";
    return -1;
  }

  if (snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams,
      &bufsize) < 0) {
    std::cout << "error setting buffersize\n";
    return(-1);
  }

  if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods,
      0) < 0) {
    std::cout << "error setting periods\n";
    return -1;
  }

  if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
    std::cout << "error setting hw params\n";
    return(-1);
  }
}

void populate_data (void *data, snd_pcm_uframes_t periodsize, int hz, int rate) {
  int i;
#if MODE_FLOAT
  float *d = (float *)data;
  for (i = 0; i <= periodsize * 2 + 3; i+=2) {
    d[i] = sin (2.0 * M_PI * i * hz / rate);
    d[i+1] = d[i];
  }
#else
  unsigned char *d = (unsigned char *)data;
  for(int l2 = 0; l2 < periodsize; l2++) {
    int s1, s2;
    s1 = (l2 % 128) * 100 - 5000;
    s2 = (l2 % 256) * 100 - 5000;
    data[4*l2] = (unsigned char)s1;
    data[4*l2+1] = s1 >> 8;
    data[4*l2+2] = (unsigned char)s2;
    data[4*l2+3] = s2 >> 8;
  }
#endif
#if DEBUG
  for (i -= 10; i <= periodsize * 2 + 3; i++ )
    printf ("tail data[%d] = %f\n", i, data[i]);
#endif
}

int main (int argc, char *argv[]) {
  char pcm_name[64] = PCM_NAME;
  int rate = SAMPLING_RATE;
  int hz = HELZ;
  int periods = PERIODS;
  snd_pcm_t *pcm_handle;
  snd_pcm_uframes_t periodsize = PERIOD_SIZE;
  snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
#if MODE_FLOAT
  float *data = (float *)malloc(periodsize * sizeof (float) * 2);
#else
#endif
  std::cout << "\nStart my synth\n\n";
  printf ("==== setting ====\n");
  printf ("device name = %s\n", pcm_name);
  printf ("sampling rate = %d\n", rate);
  printf ("hz = %d\n", hz);
  printf ("periods = %d\n", periods);
  printf ("periodsize = %lu\n", periodsize);
  printf ("=================\n");

  if (snd_pcm_open (&pcm_handle, pcm_name, stream, 0) < 0) {
    std::cout << "open error\n";
    return -1;
  }

  if (init_alsa (pcm_handle, periodsize, periods, rate) < 0) {
    std::cout << "init_alsa error\n";
    return -1;
  }

  populate_data (data, periodsize, hz, rate);

  for(int i = 0; i < NUM_LOOP; i++) {
    while ((snd_pcm_writei(pcm_handle, data, periodsize)) < 0) {
      snd_pcm_prepare(pcm_handle);
      fprintf(stderr,
          "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
    }
  }

  free (data);
  snd_pcm_drop(pcm_handle);
  snd_pcm_drain(pcm_handle);
}
