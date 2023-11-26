# BlockOut

<img width="800" alt="Screenshot 2023-11-23 at 11 23 14 AM" src="https://github.com/shuang886/BlockOut/assets/140762048/01b7ad8c-859b-45ee-b9ba-825d9faaf22f">

This is a copy of BlockOut® II so I can attempt to port it to Apple operating systems.

> BlockOut® II is a free adaptation of the original BlockOut® DOS game edited by California Dreams in 1989. BlockOut® II has the same features than the original game with few graphic improvements. The game has been designed to reproduce the original game kinematics as accurately as possible. The score calculation is also nearly similar to the original.
> 
> BlockOut® II also features an [online score database](http://blockout.net/blockout2/score.php) where you can upload your high-scores with their replays and get various statistics. The score registering system also includes an efficient fake score detection.

## Compiling

### SDL2 Universal Cocoa App

If unsure, this is probably the one you want.

1. Use the [macos-sdl2](https://github.com/shuang886/BlockOut/tree/macos-sdl2) branch.
2. Download [SDL2.framework](https://github.com/libsdl-org/SDL/releases) and place it in a folder called `Frameworks` at the same level as the root directory of BlockOut.
3. Download [SDL2_mixer.framework](https://github.com/libsdl-org/SDL_mixer/releases) and place it in a folder called `Frameworks` at the same level as the root directory of BlockOut.
4. Open the `BlockOut.xcodeproj` in Xcode.
5. Build and run the "BlockOut" target.

This produces a universal app suitable for both Apple Silicon and Intel Macs.

### SDL2 version command-line tool ported to macOS

SDL1.2 is now over ten years old and was deprecated years ago, so I've upgraded the code to SDL2. Use the [macos-sdl2](https://github.com/shuang886/BlockOut/tree/macos-sdl2) branch.

```
brew install SDL2 SDL2_mixer
cd ImageLib/src
make
cd ../../BlockOut
make _release=1
export BL2_HOME=[folder where `images` and `sounds` folders are]
./blockout
```

### SDL version command-line tool ported to macOS

Use the [macos](https://github.com/shuang886/BlockOut/tree/macos) branch.

```
brew install SDL SDL_mixer
cd ImageLib/src
make
cd ../../BlockOut
make _release=1
export BL2_HOME=[folder where `images` and `sounds` folders are]
./blockout
```

### Windows and Linux versions

The `main` branch preserves the original Release 2.5 of the game targeting Windows and Linux. I don't know if they work and I won't be much help with them.

## Roadmap

This is a list of possibilities, not promises:

- [X] Universal (Apple Silicon and Intel) app
- [ ] iOS app
- [ ] Metal app (to replace deprecated OpenGL)
