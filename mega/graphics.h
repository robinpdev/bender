#ifndef USRGRAPHICS_H
#define USRGRAPHICS_H

#include <TFT_HX8357.h> // Hardware-specific library
#include <Adafruit_GFX.h>    // Core graphics
TFT_HX8357 tft = TFT_HX8357();

//definiëring van kleuren.

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    int(210/8*2048) + int(216/4*32)+ int(224/8)
#define ORANGE    int(237/8*2048) + int(163/4*32)+ int(35/8)

#endif /* GRANDPARENT_H */