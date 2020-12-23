class encoder {
  private:
    int count;

  public:
    void add();
    int getcount();
};

void encoder::add(){
  this->count ++;
}

int encoder::getcount(){
  return this->count;
}
