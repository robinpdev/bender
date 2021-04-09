#include "graphics.h"
char tabindex = 1;
#include "gui_predefines.h"

//helper defines
#define nitems(name) guitem *name[] =
#define newcol(name, inarr) collumn name(inarr, sizeof(inarr) / 2)
#define newgui(name, inarr, index) gui name(inarr, sizeof(inarr) / 2, index)
#define whenselected(guio, element) if (guio.selitem == &element)

//internal helper defines
#define renewsel true
#define nextsel false

bool inputmodified = false;

//common draw function that calls the specific draw function for all types of items
guitemreturn guitem::draw(guiteminput in)
{
    in.offset.x += transform.x;
    in.offset.y += transform.y;
    drawpos = in.offset;
    //this->clear(bgcolor);
    return this->draw_specific(in);
}

//for updating items without redrawing all items with the gui manager
guitemreturn guitem::update()
{
    guiteminput in;
    in.offset = drawpos;
    this->clear(bgcolor);

    if (this->parent != nullptr)
    {
        collumn *tparent = static_cast<collumn *>(this->parent);
        return tparent->update();
    }

    return this->draw(in);
}

const short labelymargin = 4;
//just displays text
label::label(String mtext)
{
    text = mtext;
}

guitemreturn label::draw_specific(guiteminput in)
{

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

void label::clear(unsigned short incol) const
{
    tft.fillRect(drawpos.x, drawpos.y + labelymargin, prevlen * textsize * 6, textsize * 8, incol);
}

void label::tupdate(String ntext)
{
    text = ntext;
    if (tabindex == tab)
    {
        update();
    }
}

const short boxmargin = 4;
const short boxpadding = 4;

input::input()
{
    isinput = true;
}

void input::addchar(char in)
{
    inputmodified = true;
    if (in == '*')
    {
        if(text.length() == 0){
            text += '0';
        }
        text += ',';
    }
    else
    {
        short commaindex = -1;
        for(short i = 0; i < text.length(); i++){
            if(text.charAt(i) == ','){
                commaindex = i + 1;
                break;
            }
        }
        //limit 
        if(text.length() - commaindex < 2 || commaindex == -1){
            text += in;
        }
    }
    width = max(minwidth, text.length() * 6 * textsize + 2 * boxpadding);
    this->update();
    this->select(inputselcolor);
}

guitemreturn input::draw_specific(guiteminput in)
{
    in.offset.y += boxmargin;
    tft.setTextSize(textsize);
    tft.setTextColor(color);
    tft.setCursor(in.offset.x + boxpadding, in.offset.y + boxpadding);
    tft.print(text);

    this->select(inputboxcolor);

    prevlen = text.length();

    guitemreturn out;
    out.offset = in.offset;
    out.offset.x += width;
    out.offset.y += textsize * 8 + 2 * boxpadding;
    return out;
}

void input::clear(unsigned short color) const
{
    tft.fillRect(drawpos.x, drawpos.y + boxmargin - 1, width, textsize * 8 + 2 * boxpadding + 1, color);
}

void input::select(short boxcolor)
{
    if (minwidth > width)
    {
        width = minwidth;
    }

    tft.drawRect(drawpos.x, drawpos.y + boxmargin, width, textsize * 8 + 2 * boxpadding, boxcolor);
}

void input::deselect(unsigned short bgcolor)
{
    clear(bgcolor);
    text = prevtext;
    width = max(minwidth, text.length() * 6 * textsize + 2 * boxpadding);
    update();
    tft.drawRect(drawpos.x, drawpos.y + boxmargin, width, textsize * 8 + 2 * boxpadding, inputboxcolor);
}

void input::tupdate(String ntext)
{
    text = ntext;
    width = max(minwidth, text.length() * 6 * textsize + 2 * boxpadding);
    if (tabindex == tab)
    {
        update();
    }
}

//collumns for drawing guitems horizontally
const short colmarginy = 4; //spacing between collumns

collumn::collumn(guitem **mitems, unsigned int milen)
{
    items = mitems;
    ilen = milen;
    iscol = true;

    for (int i = 0; i < ilen; i++)
    {
        items[i]->parent = this;
    }
}

guitemreturn collumn::draw_specific(guiteminput in)
{
    guitemreturn out;
    int maxoffy = 0;
    guiteminput newin;
    newin.offset = in.offset;

    for (int i = 0; i < ilen; i++)
    {
        items[i]->tab = tab; //TODO: search a better solution for this

        out = items[i]->draw(newin);
        newin.offset.x = out.offset.x + colmarginy;
        maxoffy = max(maxoffy, out.offset.y);
    }
    out.offset = in.offset;
    out.offset.y = maxoffy;

    return out;
}

input *collumn::newselect(bool mode)
{
    int startindex = 0;
    if (selindex != -1)
    {
        startindex = selindex + 1;
    }

    if (mode == renewsel)
    {
        startindex = 0;
    }

    if (startindex >= ilen)
    {
        return nullptr;
    }

    for (int i = startindex; i < ilen; i++)
    {
        if (items[i]->isinput)
        {
            if (selitem != nullptr)
            {
                selitem->deselect(bgcolor);
            }

            selitem = static_cast<input *>(items[i]);
            selitem->select(inputselcolor);

            selindex = i;
            return selitem;
        }
    }

    selindex = -1;
    selitem = nullptr;
    return nullptr;
}

void collumn::clear(unsigned short color) const
{
    for (int i = 0; i < ilen; i++)
    {
        items[i]->clear(bgcolor);
    }
}

//manager for drawing gui items to the screen

gui::gui(guitem **mitems, unsigned int milen, char mtab)
{
    items = mitems;
    ilen = milen;
    for (int i = 0; i < ilen; i++)
    {
        items[i]->tab = mtab;
    }
}

void gui::draw()
{
    guitemreturn out;
    guiteminput in;
    in.offset.x = 10;
    for (int i = 0; i < ilen; i++)
    {
        in.offset.y = out.offset.y;
        out = items[i]->draw(in);
    }
}

input *gui::newselect()
{
    if (selitem != nullptr)
    {
        selitem->deselect(inputboxcolor);
    }

    int startindex = 0;
    if (selindex != -1)
    {
        startindex = selindex + 1;

        if (items[selindex]->iscol)
        {
            collumn *coltest = static_cast<collumn *>(items[selindex]);
            input *colsel = coltest->newselect(nextsel);
            if (colsel != nullptr)
            {
                selitem = colsel;
                return selitem;
            }
        }
    }

    if (startindex >= ilen)
    {
        selindex = -1;
        selitem = nullptr;
        return nullptr;
    }

    for (int i = startindex; i < ilen; i++)
    {
        if (items[i]->isinput)
        {
            selitem = static_cast<input *>(items[i]);
            selitem->select(inputselcolor);

            selindex = i;
            return selitem;
        }
        else if (items[i]->iscol)
        {
            if (selitem != nullptr)
            {
                selitem->deselect(inputboxcolor);
            }

            collumn *coltest = static_cast<collumn *>(items[i]);
            input *colsel = coltest->newselect(renewsel);
            if (colsel != nullptr)
            {
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