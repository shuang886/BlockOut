# BlockOut

<img width="800" alt="Screenshot 2023-11-23 at 11 23 14 AM" src="https://github.com/shuang886/BlockOut/assets/140762048/01b7ad8c-859b-45ee-b9ba-825d9faaf22f">

This is a copy of BlockOut® II so I can attempt to port it to Apple operating systems.

BlockOut® II is a free adaptation of the original BlockOut® DOS game edited by California Dreams in 1989. BlockOut® II has the same features than the original game with few graphic improvements. The game has been designed to reproduce the original game kinematics as accurately as possible. The score calculation is also nearly similar to the original.

BlockOut® II also features an [online score database](http://blockout.net/blockout2/score.php) where you can upload your high-scores with their replays and get various statistics. The score registering system also includes an efficient fake score detection.

## Compiling

First of all, make sure you're in the `macos` branch.
```
brew install SDL SDL_mixer
cd ImageLib/src
make
cd ../../BlockOut
make _release=1
export BL2_HOME=[folder where `images` and `sounds` folders are]
./blockout
```

You can further:
```
strip blockout
```
but that saves just 44KB.

Apple M1 Pro
macOS Sonoma 14.1.1 (23B81)
