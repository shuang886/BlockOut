# BlockOut

This is a copy of BlockOut® II so I can attempt to port it to Apple operating systems.

BlockOut® II is a free adaptation of the original BlockOut® DOS game edited by California Dreams in 1989. BlockOut® II has the same features than the original game with few graphic improvements. The game has been designed to reproduce the original game kinematics as accurately as possible. The score calculation is also nearly similar to the original.

BlockOut® II also features an [online score database](http://blockout.net/blockout2/score.php) where you can upload your high-scores with their replays and get various statistics. The score registering system also includes an efficient fake score detection.

## Compiling

First of all, make sure you're in the `macos` branch.

```
brew install SDL SDL_mixer
cd ImageLib
make
cd ../BlockOut
make
export BL2_HOME=[folder where `images` and `sounds` folders are]
./blockout
```
