//cs335 Spring 2015 final project 
//Bethany Armitage: contributions to main

// 1 for quick load, 0 for slow load with menu images
#define QUICK_LOAD_TIME 1
timeval frameStart, frameEnd;
int frameIndex=0;

//Images and Textures
Ppmimage *initImages[32];
GLuint initTextures[32];

Ppmimage *computerScreenImages[26];
GLuint computerScreenTextures[26];

void renderInitMenu();
void renderComputerScreenMenu();

void init_opengl (void) {
    //OpenGL initialization
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    //Set the screen background color
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    initFastFont();

    //Load images into ppm structure.
    //Hero image
    heroImage = ppm6GetImage("./images/hero.ppm");
    //Create texture elements
    glGenTextures(1, &heroTexture);
    int w = heroImage->width;
    int h = heroImage->height;
    glBindTexture(GL_TEXTURE_2D, heroTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //build a new set of data...
    unsigned char *silhouetteData = buildAlphaData(heroImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
	    GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
    delete [] silhouetteData;

    gettimeofday(&fireStart, NULL);

    if (!QUICK_LOAD_TIME) {

        // load initialization screens
        unsigned char *menuData;
        glGenTextures(32, initTextures);
        string fileName;

        for (int q=0; q<32; q++) {

            fileName = "./images/init/init";
            fileName += itos(q);
            fileName += ".ppm";
            cout << "loading file: " <<fileName <<endl;
            initImages[q] = ppm6GetImage(fileName.c_str());

            glBindTexture(GL_TEXTURE_2D, initTextures[q]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            menuData = buildAlphaData(initImages[q]);
            w = initImages[q]->width;
            h = initImages[q]->height;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, menuData);
        }
        delete [] menuData;

        // load blinking computer screens
        unsigned char *computerData;
        glGenTextures(26, computerScreenTextures);
        fileName = "";

        for (int q=0; q<26; q++) {

            fileName = "./images/cs/cs";
            fileName += itos(q);
            fileName += ".ppm";
            cout << "loading file: " <<fileName <<endl;
            computerScreenImages[q] = ppm6GetImage(fileName.c_str());

            glBindTexture(GL_TEXTURE_2D, computerScreenTextures[q]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            computerData = buildAlphaData(computerScreenImages[q]);
            w = computerScreenImages[q]->width;
            h = computerScreenImages[q]->height;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, computerData);
        }
        delete [] computerData;
    }


void renderInitMenu () {
    gettimeofday(&frameStart, NULL);
    int frameTime = 100;

    // loop through frames
    if (diff_ms(frameStart, frameEnd) > frameTime) {
        frameIndex++;
        gettimeofday(&frameEnd, NULL);
    }

    if (frameIndex == 32) {
        cout << "frame index: "<< frameIndex <<endl;
        frameIndex = 0;
        level = 0;
        return;
    }

    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glTranslatef(WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT, 0);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, initTextures[frameIndex]);
    //glAlphaFunc(GL_GREATER, 0.0f);
    //glAlphaFunc(GL_LESS, 1.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1) ; glVertex2i(-WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);
    glTexCoord2f(0, 0) ; glVertex2i(-WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 0) ; glVertex2i( WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 1) ; glVertex2i( WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);

    glEnd(); glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);


}
void renderComputerScreenMenu () {
    gettimeofday(&frameStart, NULL);
    int frameTime = 300;

    // loop through frames
    if (diff_ms(frameStart, frameEnd) > frameTime) {
        frameIndex++;
        gettimeofday(&frameEnd, NULL);
        //cout << "frame index: "<< frameIndex <<endl;
    }

    if (frameIndex == 26) {
        frameIndex = 0;
        level = 1;
        return;
    }

    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glTranslatef(WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT, 0);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, computerScreenTextures[frameIndex]);
    //glAlphaFunc(GL_GREATER, 0.0f);
    //glAlphaFunc(GL_LESS, 1.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1) ; glVertex2i(-WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);
    glTexCoord2f(0, 0) ; glVertex2i(-WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 0) ; glVertex2i( WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 1) ; glVertex2i( WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);

    glEnd(); glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);


}
