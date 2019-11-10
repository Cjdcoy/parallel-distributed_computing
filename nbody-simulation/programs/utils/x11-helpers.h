#ifndef x11_h_
#define x11_h_

#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <iostream>

#define WIN_SIZE 200

class XlibWrap {
public:
    XlibWrap(){};

    void init_image()
    {
        Visual *visual = DefaultVisual(_display, DefaultScreen(_display));
        int depth = DefaultDepth(_display, DefaultScreen(_display));
        int total;

        total = WIN_SIZE * 4;
        total *= WIN_SIZE;

        _x_image = XCreateImage(_display, visual, depth, ZPixmap, 0, (char*)malloc(total), WIN_SIZE, WIN_SIZE, 32, 0);
    }

    void init()
    {
        char name[128] = "Parallel nbody";
        char *name_pointer = name;
        _display = XOpenDisplay(NULL);
        _window = XCreateSimpleWindow(_display, DefaultRootWindow(_display), 0, 0, WIN_SIZE, WIN_SIZE, 0, BlackPixel(_display, DefaultScreen(_display)),
                                     BlackPixel(_display, DefaultScreen(_display)));
        XSelectInput(_display, _window, StructureNotifyMask);
        XMapWindow(_display, _window);
        XTextProperty text_property;
        Status status = XStringListToTextProperty(&name_pointer, 1, &text_property);
        if (status)
            XSetWMName(_display, _window, &text_property);

        XFlush(_display);
        init_image();
        _gc = XCreateGC(_display, _window, 0, NULL);
        XSelectInput(_display, _window, ExposureMask | KeyPressMask);
    }

    void clear()
    {
        memset(_x_image->data, 1, WIN_SIZE * WIN_SIZE * 4);
    }

    void destroy()
    {
        XDestroyImage(_x_image);
        XDestroyWindow(_display, _window);
        XCloseDisplay(_display);
    }

    void put_pixel(int x, int y, int r, int g, int b) { XPutPixel(_x_image, x, y, r * 65536 + g * 256 + b); }
    void put_pixel(int x, int y, unsigned int rgb) { XPutPixel(_x_image, x, y, rgb); }
    void draw_square(int x1, int y1, int x2, int y2)
    {
        int r = 133;
        int g = 0;
        int b = 133;
        for (int x = x1; x < x2 ; x++) {
            XPutPixel(_x_image, x, y1, r * 65536 + g * 256 + b);
        }
        for (int y = y1; y < y2 ; y++) {
            XPutPixel(_x_image, x1, y, r * 65536 + g * 256 + b);
        }
        for (int x = x1; x < x2 ; x++) {
            XPutPixel(_x_image, x, y2, r * 65536 + g * 256 + b);
        }
        for (int y = y1; y < y2 ; y++) {
            XPutPixel(_x_image, x2, y, r * 65536 + g * 256 + b);
        }
    }
    void put_image(int src_x, int src_y, int dest_x, int dest_y, int width, int height)
    {
        XPutImage(_display, _window, _gc, _x_image, src_x, src_y, dest_x, dest_y, width, height);
    }

    void flush() { XFlush(_display); }

private:
    Display *_display;
    Window  _window;
    GC      _gc;
    XImage  *_x_image;
};


#endif /* h */
