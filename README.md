## Game Title
**Software Engineering (cs335) Final Game Project, CSUB**

**to run on desktop in CSUB Science III**
```export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/mesa/libGL.so.1```

### Using Tool Editor
* Create a new blank slate:
    `$ ./tool`

*  Inside Tool Editor
    *  `c` - create mode -> click to place objects
    *  `v` - select mode -> select created objects
    *  `r` - increase row size of selected object
    *  `t` - increase column size of selected object
    *  `y` - decrease row size of selected object
    *  `u` - decrease column size of selected object
    *  `s` - save level as created
    *  `l` - revert back to last saved state
    *  `j` - change tile right/left
    *  `h` - change tile down/up
    *  `x` - access to click tile mode. Press the middle mouse button 
    *  `n` - delete holding or last hold object.
    *  `b` - delete last object put
    *  `f` - change modes: Platform Editor, Enemy Editor
*  Notes:
    *  There must be at least 7 objects, or segmentation fault is thrown
    *  Theres A bug on the click y axis while in click tile mode. Y position may
    *  not be right. It will usally click the tile lower than you click
*  TODO:
    *  put usage instructions on level editor

### Adding Sound Effects
* add wav file to `sounds/` directory
* add a corresponding `fmod_createsound()` line in `sounds.cpp` to import the sound, incrementing to the next sound number.
* add line to enumeration in `sounds.cpp` so we can call that sound by a name, and not it's number
* call sound where you need the sound made by `fmod_playsound(soundName);`
* be sure to wrap each sound call in `#ifdef USE_SOUND` and `#endif`, so those of us with macs can develop without sounds.



### Collaborators
* Bethany Armitage
* Chad Danner
* Ted Pascua
* Brian Singenstrew

### Game Description
Paralax side scroller developed using mainly C/C++ with OpenGL game graphics.
Adventure through the harware of a computer cleansing it of malicious viruses.
