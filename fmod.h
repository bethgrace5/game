#ifndef FMOD_H
#define FMOD_H

#ifdef USE_SOUND
#define NSOUNDS 100
extern FMOD_CHANNEL *channel;
extern int fmod_cleanup(void);
extern int fmod_init(void);
extern int fmod_createsound(char *fname, int i);
extern int fmod_playsound(int i);
extern int fmod_setmode(int i, int mode);
extern int fmod_getlength(int i, unsigned int *lenms);
extern int fmod_systemupdate(void);
extern int fmod_getchannelsplaying(int *channelsplaying);
extern int fmod_stopAll(void);
int fmod_releasesound(int s);
#endif

#endif

