#ifndef MY_FMOD_H
#define MY_FMOD_H

// comment line out to ignore sounds
#define USE_SOUND

#ifdef USE_SOUND
#define NSOUNDS 3
extern FMOD_CHANNEL *channel;
extern int fmod_cleanup(void);
extern int fmod_init(void);
extern int fmod_createsound(char *fname, int i);
extern int fmod_playsound(int i);
extern int fmod_setmode(int i, int mode);
extern int fmod_getlength(int i, unsigned int *lenms);
extern int fmod_systemupdate(void);
extern int fmod_getchannelsplaying(int *channelsplaying);
#endif

#endif //MY_FMOD_H

