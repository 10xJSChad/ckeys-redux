/*
 * ckeys_x11.hpp
 *
 * X11 specific CKeys functions
 */
#pragma once


#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>


#define SLEEP_MS(ms) usleep(1000 * ms)
#define XDOTOOL_PATH "/usr/bin/xdotool"


size_t Mouse_GetWindow(void);
double CKeys_TimeElapsed(void);
void _mouse_init(void);


/* X11 display */
Display* _display_p = NULL;


void Mouse_SetPos(int x, int y) {
    Window root = RootWindow(_display_p, _mouse_get()->screen);
    XWarpPointer(_display_p, None, root, 0, 0, 0, 0, x, y);
    XFlush(_display_p);
}


void _mouse_button(int button, bool is_press) {
    XTestFakeButtonEvent(_display_p, button, is_press, CurrentTime);
    XFlush(_display_p);
}


void _mouse_click(int button) {
    _mouse_button(button, true);
    _mouse_button(button, false);
}


void Mouse_LeftClick(void) {
    _mouse_click(Button1);
}


void Mouse_RightClick(void) {
    _mouse_click(Button3);
}


void Pixel_ColorAtPos(int x, int y, char* buf) {
    XColor color;
    XImage* image;

    image = XGetImage(_display_p, XRootWindow(_display_p, XDefaultScreen(_display_p)), x, y, 1, 1, AllPlanes, XYPixmap);

    if (image == NULL)
        _fprintf_exit("XGetImage failed");

    color.pixel = XGetPixel(image, 0, 0);
    XQueryColor(_display_p, DefaultColormap(_display_p, XDefaultScreen(_display_p)), &color);
    XFree(image);

    snprintf(buf, 8, "#%02x%02x%02x", color.red >> 8, color.green >> 8, color.blue >> 8);
}


void _key_button(int keycode, bool is_press) {
    XKeyEvent event;

    event.display = _display_p;
    event.window = _mouse_get()->window;
    event.root = _mouse_get()->window;
    event.subwindow = None;
    event.time = CurrentTime;
    event.x = 1;
    event.y = 1;
    event.x_root = 1;
    event.y_root = 1;
    event.same_screen = True;

    event.type = is_press ? KeyPress : KeyRelease;
    event.keycode = XKeysymToKeycode(_display_p, keycode);
    event.state = 0;

    XSendEvent(_display_p, event.window, True, KeyPressMask, (XEvent*) &event);
    XFlush(_display_p);
}


void Key_Down(int keycode) {
    _key_button(keycode, true);
}


void Key_Up(int keycode) {
    _key_button(keycode, false);
}


void Key_WriteString(const char* str) {
    char* cmd = (char*) _xcalloc(1, 64 + strlen(str));
    snprintf(cmd, 64 + strlen(str), XDOTOOL_PATH " type '%s'", str);
    system(cmd);
    free(cmd);
}


bool Key_IsPressed(int keycode) {
    char keys_return[32];
    XQueryKeymap(_display_p, keys_return);
    KeyCode kc2 = XKeysymToKeycode(_display_p, keycode);
    return (keys_return[kc2>>3] & (1<<(kc2&7)));
}


void _mouse_update(void) {
    Window root, window;
    int root_x, root_y, win_x, win_y;
    int screen_num = 0;
    int screencount = ScreenCount(_display_p);
    unsigned int dummy_mask = 0;

    for (int i = 0; i < screencount; i++) {
        Screen* screen = ScreenOfDisplay(_display_p, i);

        if (XQueryPointer(_display_p, RootWindowOfScreen(screen),
                &root, &window, &root_x, &root_y, &win_x, &win_y, &dummy_mask)) {
            screen_num = i;
            break;
        }
    }

    _mouse_p->x = root_x;
    _mouse_p->y = root_y;
    _mouse_p->screen = screen_num;
    _mouse_p->window = window; /* inaccurate */
}


void _mouse_update_window(void) {
    if (_mouse_p == NULL)
        _fprintf_exit("Mouse struct not initialized");

    FILE* fp = _popen_err_checked(XDOTOOL_PATH " getmouselocation", "r");
    fscanf(fp, "x: %d y: %d screen: %d window: %lu", &_mouse_p->x, &_mouse_p->y, &_mouse_p->screen, &_mouse_p->window);
    _pclose_err_checked(fp);
}


void _mouse_init(void) {
    _mouse_p = (struct _Mouse*) _xcalloc(1, sizeof *_mouse_p);
}


size_t Mouse_GetWindow(void) {
    _mouse_update_window();
    return _mouse_p->window;
}


/* Initializes the CKeys library, must be called before any other function */
void CKeys_Init(void) {
    _display_p = XOpenDisplay(NULL);
    _mouse_init();
    CKeys_TimeElapsed();

    if (_display_p == NULL)
        _fprintf_exit("XOpenDisplay failed");
}

