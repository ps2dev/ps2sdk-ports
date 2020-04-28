# PS2SDK-PORTS

## What is it ?
This git repository contains various number of ports working with the current `ps2sdk`. It is not intended to be a releasable package.

Most of the ports contains specific changes to make them work for the `ps2sdk`, however there are some ports included as a `git submodules` that are being compiled using original source code, just adding the specficic `PS2` flags.

Additionaly, the `SDL` ports requires to have installed previously [GSKit](https://github.com/ps2dev/gsKit), more concretely they are:

```
sdl sdlgfx sdlimage sdlmixer sdlttf
```

## Compilation & Installation

Easy for everyone, just type 
```
make
```