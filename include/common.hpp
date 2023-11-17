#pragma once

//stuff used both by renderer and game logic found here

namespace opengles_workspace{

    struct Pos{
        float x,y; //normalized position of each body chunk's (+ food's) upper left corner (between -1.0f and 1.0f)
        bool operator==(Pos&);
        bool operator!=(Pos&);
    };

    const float step = 0.2f; //space of snake movements and pieces rendering

    
}