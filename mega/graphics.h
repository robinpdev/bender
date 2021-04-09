#ifndef USRGRAPHICS_H
#define USRGRAPHICS_H

#include <TFT_HX8357.h> // Hardware-specific library
//#include <Adafruit_GFX.h>    // Core graphics
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
#define ORANGE    int(237/8*2048) + int(163/4*32)+ int(35/8) //r:237

#define dir_ltr 1

#define hexize(incol) int(incol.r/8*2048) + int(incol.g/4*32)+ int(incol.b/8)
#define rgbize(hexValue) (color){((hexValue >> 16) & 0xFF) / 255.0, ((hexValue >> 8) & 0xFF) / 255.0, ((hexValue) & 0xFF) / 255.0}

struct color{
    unsigned int r, g, b;
};

void gradient(int x, int y, int w, unsigned int h, unsigned int mscol, int mecol, short dir){
    color scol = rgbize(mscol);
    color ecol = rgbize(mecol);
    switch(dir){
        case dir_ltr:{
            for(int i = 0; i <= w; i++){
                color intercol;
                intercol. r = 0 + 1.0 * (i * 1.0 / w) * (255 - 0);
                intercol. g = 164 + 1.0 * (i * 1.0 / w) * (0 - 164);
                intercol. b = 35 + 1.0 * (i * 1.0 / w) * (0 - 35);
                tft.drawLine(x + i, y, x + i + 1, y + h, hexize(intercol));
                Serial.println(intercol.r);
            }
        }

        default:{
            break;
        } 
    }
}

#endif /* GRANDPARENT_H */