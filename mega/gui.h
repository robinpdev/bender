#include "graphics.h"

#define renewsel true
#define nextsel false

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

class guitem {
    public:
    vec drawpos;
    bool isinput = false;
    bool iscol = false;

    //virtual function for drawing the types of items
    virtual guitemreturn draw_specific(guiteminput in) {};
    virtual void clear(unsigned short color) const {};

    //common draw function that calls the specific draw function for all types of items
    guitemreturn draw(guiteminput in){
        drawpos = in.offset;
        this->clear(bgcolor);
        return this->draw_specific(in);
    }

    //for updating items without redrawing all items with the gui manager
    guitemreturn update(){
        guiteminput in;
        in.offset = drawpos;
        this->clear(bgcolor);
        return this->draw(in);
    }
};

const short labelymargin = 4;
//just displays text
class label : public guitem {
    public:
    String text = "";
    short color = RED;
    short textsize = 3;
    short prevlen = 0;

    label(String mtext){
        text = mtext;
    }

    guitemreturn draw_specific(guiteminput in) override {

        tft.setTextSize(textsize);
        tft.setTextColor(color);
        tft.setCursor(in.offset.x, in.offset.y + labelymargin);
        tft.print(text);

        prevlen = text.length();
        guitemreturn out;
        out.offset = in.offset;
        out.offset.x += prevlen * textsize * 6;
        out.offset.y += textsize * 8 + labelymargin;
        return out;
    }

    void clear(unsigned short color) const override {
        tft.fillRect(drawpos.x, drawpos.y + labelymargin, prevlen * textsize * 6,textsize * 8, color);
    }
};

short boxmargin = 4; //TODO: variable selection box margin
class input : public guitem {
    public:
    short minwidth = 50;
    short width = minwidth;

    String text = "";
    short color = RED;
    short textsize = 3;
    short prevlen = 0;
    input(){
        isinput = true;
    }

    guitemreturn draw_specific(guiteminput in) override{
        guitemreturn out;
        out.offset = in.offset;

        out.offset.x += width;
        return out;
    }

    void select(){
        if(minwidth > width){
            width = minwidth;
        }

        tft.drawRect(drawpos.x, drawpos.y, width, textsize * 8, color);
    }

    void deselect(short bgcolor){
        tft.drawRect(drawpos.x, drawpos.y, width, textsize * 8, bgcolor);
    }


};

//collumns for drawing guitems horizontally
const short colmarginy = 4; //spacing between collumns
class collumn : public guitem {
    public:
    guitem** items;
    unsigned int ilen;

    int selindex = -1; //which index of the items is selected
    input* selitem = nullptr;

    collumn(guitem** mitems, unsigned int milen){
        items = mitems;
        ilen = milen;
        iscol = true;
    }

    guitemreturn draw_specific(guiteminput in) override {
        guitemreturn out;
        int maxoffy = 0;
        guiteminput newin;
        newin.offset = in.offset;
        Serial.print("newin: ");
        Serial.println(newin.offset.x);
        for(int i = 0; i < ilen; i++){
            out = items[i]->draw(newin);
            newin.offset.x = out.offset.x + colmarginy;
            maxoffy = max(maxoffy, out.offset.y);
        }
        out.offset = in.offset;
        out.offset.y = maxoffy;
        
        return out;
    }

    input* newselect(bool mode){
        int startindex = 0;
        if(selindex != -1){
            startindex = selindex + 1;
        }

        if(mode == renewsel){
            startindex = 0;
        }

        if(startindex >= ilen){
            return nullptr;
        }

        for(int i = startindex; i < ilen; i++){
            if(items[i]->isinput){
                if(selitem != nullptr){
                    selitem->deselect(bgcolor);
                }

                selitem = static_cast<input*>(items[i]);
                selitem->select();
                
                selindex = i;
                return selitem;
            }
        }

        selindex = -1;
        selitem = nullptr;
        return nullptr;
    }
};

//manager for drawing gui items to the screen
class gui{
    public:
    guitem** items;
    unsigned int ilen;

    int selindex = -1; //which index of the items is selected
    input* selitem = nullptr;

    gui(guitem** mitems, unsigned int milen){
        items = mitems;
        ilen = milen;
    }

    void draw(){
        guitemreturn out;
        guiteminput in;
        in.offset.x = 10;
        for(int i = 0; i < ilen; i++){
            in.offset.y = out.offset.y;
            out = items[i]->draw(in);
        }
    }

    input* newselect(){
        int startindex = 0;
        if(selindex != -1){
            startindex = selindex + 1;

            if(items[selindex]->iscol){
                collumn* coltest = static_cast<collumn*>(items[selindex]);
                input* colsel = coltest->newselect(nextsel);
                if(colsel != nullptr){
                    return colsel;
                }
            }
        }

        if(startindex >= ilen){
            startindex = 0;
        }

        Serial.print("startindex: ");
        Serial.println(startindex);

        for(int i = startindex; i < ilen; i++){
            if(items[i]->isinput){
                if(selitem != nullptr){
                    selitem->deselect(bgcolor);
                }

                selitem = static_cast<input*>(items[i]);
                selitem->select();
                
                selindex = i;
                return selitem;
            }else if(items[i]->iscol){
                if(selitem != nullptr){
                    selitem->deselect(bgcolor);
                }

                collumn* coltest = static_cast<collumn*>(items[i]);
                input* colsel = coltest->newselect(renewsel);
                if(colsel != nullptr){
                    selitem = colsel;
                    selindex = i;
                    return colsel;
                }
            }
        }

        selindex = -1;
        selitem = nullptr;
        return nullptr;
    }
};