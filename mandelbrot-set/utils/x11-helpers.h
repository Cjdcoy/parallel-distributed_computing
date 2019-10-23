//
//  Created by Rafael Eyng on 6/2/18.
//  Copyright © 2018 Rafael Eyng. All rights reserved.
//

#ifndef x11_h
#define x11_h

#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <iostream>

class XlibWrap {
public:
    XlibWrap(){};

    void init_image(int image_size)
    {
        Visual *visual = DefaultVisual(_display, DefaultScreen(_display));
        int depth = DefaultDepth(_display, DefaultScreen(_display));
        int total;

        total = image_size * 4;
        total *= image_size;

        _x_image = XCreateImage(_display, visual, depth, ZPixmap, 0, (char*)malloc(total), image_size, image_size, 32, 0);
    }

    void init(int image_size)
    {
        char name[128] = "Parallel Mandelbrot";
        char *name_pointer = name;
        _display = XOpenDisplay(NULL);
        _window = XCreateSimpleWindow(_display, DefaultRootWindow(_display), 0, 0, image_size, image_size, 0, BlackPixel(_display, DefaultScreen(_display)),
                                     BlackPixel(_display, DefaultScreen(_display)));
        XSelectInput(_display, _window, StructureNotifyMask);
        XMapWindow(_display, _window);
        XTextProperty text_property;
        Status status = XStringListToTextProperty(&name_pointer, 1, &text_property);
        if (status)
            XSetWMName(_display, _window, &text_property);

        XFlush(_display);
        init_image(image_size);
        _gc = XCreateGC(_display, _window, 0, NULL);
        XSelectInput(_display, _window, ExposureMask | KeyPressMask);
    }

    void destroy()
    {
        XDestroyImage(_x_image);
        XDestroyWindow(_display, _window);
        XCloseDisplay(_display);
    }

    void put_pixel(int x, int y, int r, int g, int b) { XPutPixel(_x_image, x, y, r * 65536 + g * 256 + b); }
    void put_pixel(int x, int y, unsigned int rgb) { XPutPixel(_x_image, x, y, rgb); }

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
