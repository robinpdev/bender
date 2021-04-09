struct vec
{
    int x = 0;
    int y = 0;
};

struct guiteminput
{
    vec offset;
};

struct guitemreturn
{
    vec offset;
};

class guitem
{
public:
    vec drawpos;
    vec transform;
    bool isinput = false;
    bool iscol = false;
    char tab = 0;
    guitem *parent = nullptr; //this is actually a pointer to a collumn

    //virtual function for drawing the types of items
    virtual guitemreturn draw_specific(guiteminput in){};
    virtual void clear(unsigned short) const {};

    guitemreturn draw(guiteminput in);
    guitemreturn update();
};

//just displays text
class label : public guitem
{
public:
    String text = "";
    short color = BLACK;
    short textsize = 3;
    short prevlen = 0;

    label(String mtext);

    guitemreturn draw_specific(guiteminput in) override;
    void clear(unsigned short incol) const override;
    void tupdate(String ntext);
};

//TODO: make input have label built in to save ram
class input : public guitem
{
public:
    short minwidth = 50;
    short width = minwidth;

    String text = "";
    String prevtext = "";
    short color = RED;
    short textsize = 3;
    short prevlen = 0;

    input();
    void addchar(char in);
    void select(short boxolor);
    void deselect(unsigned short bgcolor);

    guitemreturn draw_specific(guiteminput in) override;
    void clear(unsigned short color) const override;
    void tupdate(String ntext);
};

class collumn : public guitem
{
public:
    guitem **items;
    unsigned int ilen;

    int selindex = -1; //which index of the items is selected
    input *selitem = nullptr;

    collumn(guitem **mitems, unsigned int milen);
    input *newselect(bool mode);

    guitemreturn draw_specific(guiteminput in) override;
    void clear(unsigned short color) const override;
};

class gui
{
public:
    guitem **items;
    unsigned int ilen;

    int selindex = -1; //which index of the items is selected
    input *selitem = nullptr;

    gui(guitem **mitems, unsigned int milen, char mtab);
    void draw();
    input *newselect();
};