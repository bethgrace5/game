## Game Title
#### Software Engineering (cs335) Final Game Project, CSUB
#### Collaborators
* Bethany Armitage
* Chad Danner
* Ted Pascua
* Brian Singenstrew

#### Game Description
Paralax side scroller developed using mainly C/C++ with OpenGL game graphics.
Adventure through the harware of a computer cleansing it of malicious viruses.

#### to run on desktop in CSUB Science III
```export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/mesa/libGL.so.1```

### Using Tool Editor
* Create a new blank slate:
    ```./tool```

*  Inside Tool Editor
    *  ```c``` - create mode -> click to place objects
    *  ```v``` - select mode -> select created objects
    *  ```r``` - increase row size of selected object
    *  ```s``` - save level as created
    *  ```l``` - revert back to last saved state
    *  
*  Notes:
    *  There must be at least 7 objects, or segmentation fault is thrown
*  TODO:
    *  put usage instructions on level editor
