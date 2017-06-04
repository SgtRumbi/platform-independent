Project platform-independent
============================

TLDR;
Currently this project looks as it wants to get a small, nifty and (very) advanced Pong-clone. ;)
-> todo: Rename project to something appropriate like super-pong ^^

This project should be a tech-demonstration of stateless rendering and targeting multiple platforms using ~1 code base.

Real To Dos:
------------
 - Make rendering independent from Screen-Resultion -> Have fixed coordinate system(s)
 - Implement core game mechanics for Pong
    - Ball-reflection on walls
    - Paddle-/Wall-Behind-Paddles-Collision
    - Game-Over
 - Implement game modes
    - In-Game
    - Menu
    - Pause
 - Support render to texture
    - Support post processing on appropriate targets (probably Desktop (OpenGL 3.0 and above)
      and Android (maybe OpenGL ES 2.0 and OpenGL ES 3.0 and above)
    - Have fallback if r2t is not supported
 - Be more 'stateless' -> Get rid of Initialized-flags and migrate buffers to STREAM_DRAW/DYNAMIC_DRAW
 - Preload shaders
 - Figure out how to process assetes (like fonts, images or audio)
 - Get an audio engine up and running
    - Maybe ALSA on linux
    - Check out OpenSL ES on Android
    - Check out Csound API
    - Check out DirectSound on win32
    - Load audio files (.wav?)
    - Stream audio files

~ js
