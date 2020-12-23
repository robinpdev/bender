#include "graphics.h"

struct vec{
    int x = 0;
    int y = 0;
};

struct guiteminput{
    vec offset;
};

struct guitemreturn{
    vec offset;
};

class guitem{
    public:
    vec drawpos;

    //virtual function for drawing the types of items
    virtual guitemreturn draw_specific(guiteminput in) const {};

    //common draw function that calls the specific draw function for all types of items
    guitemreturn draw(guiteminput in){
        drawpos = in.offset;

        return this->draw_specific(in);
    }

    //for updating items without redrawing all items with the gui manager
    guitemreturn update(){
        guiteminput in;
        in.offset = drawpos;
        return this->draw(in);
    }
};

const int labelymargin = 4;
//just displays text
class label : public guitem{
    public:
    String text = "";
    short color = RED;

    label(String mtext){
        text = mtext;
    }

    guitemreturn draw_specific(guiteminput in) const override {

        tft.setTextSize(5);
        tft.setTextColor(color);
        tft.setCursor(in.offset.x, in.offset.y + labelymargin);
        tft.print(text);

        guitemreturn out;
        out.offset = in.offset;
        out.offset.y = in.offset.y + 5 * 8 + labelymargin; //TODO: replace 5 with textsize variable
        return out;
    }
};

//manager for drawing gui items to the screen
class gui{
    public:
    guitem** items;
    unsigned int ilen;

    gui(guitem** mitems, unsigned int milen){
        items = mitems;
        ilen = milen;
    }

    void draw(){
        guitemreturn out;
        guiteminput in;
        in.offset.x = 10;
        for(int i = 0; i < ilen; i++){
            in.offset = out.offset;
            out = items[i]->draw(in);
        }
    }
};