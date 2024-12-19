# ckeys-redux (X11 only)

Library to do ahk-like stuff in C++, this *was* a C project, but then I wanted to use vectors and maps, so now it's C++, except not really because I'm still writing "C with vectors and maps".
Relies on xdotool being in PATH, which is also stupid but I won't fix it until it bothers me enough, also I'm pretty sure I used a *lot* of reserved names, though it's extremely unlikely this will ever be a problem.

If I still haven't convinced you to not use this, then here's how you build it.

```
g++ main.cpp -lm -lX11 -lXtst -Wall -Wextra -o main
```

Maybe one day I'll turn this into a proper "thing" instead of a hastily rigged together autohotkey-esque C++ library, but for now it's gonna be as it is. Been very useful to have though, did a lot of cool things with this.
