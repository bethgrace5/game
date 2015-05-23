
#ifndef FMOD_H
#define FMOD_H

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

#endif //FMOD_H

