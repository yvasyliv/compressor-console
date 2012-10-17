#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <deque>
#include <map>
#include <queue>
//#define NDEBUG
#include <cassert>

using namespace std;

class bitreader {
  ifstream &in;
  size_t cnt_of_spam_symbols;
  deque<bool> Q;
  bool read_more();
public:
  bool get_for_sure();
  bool get(bool &);
  bitreader(ifstream &in, size_t cnt):in(in),cnt_of_spam_symbols(cnt)  {
  }
  ~bitreader() {
    in.close();
  }
};

struct vertex {
  vertex * perent, *left, *right;
  size_t cnt;
  bool symbol;
  unsigned char vertexchar;
  vertex * next(bool c);
  bool is_leaf();
  void set_symbol(bool b);
  size_t get_cnt();
  bool operator() (const vertex * a, const vertex * b) {
    return a->cnt > b->cnt;
  }
  vertex() {}
  vertex(vertex * per, vertex * l, vertex * r, size_t cnt) : perent(per), left(l), right(r), cnt(cnt) {}
};

typedef vertex * pvertex;

class tree {
private :
  map<unsigned char, pvertex> MP;
  map<unsigned char, deque<bool> > m_code_of;
  void push_deque(deque<bool> &d, unsigned char ch);
public :
  pvertex build(bitreader &br);
  size_t full_size(map<unsigned char, size_t> const &cnt);
  deque<bool> to_deque();
  deque<bool> codeof(unsigned char ch);
  void build(map<unsigned char, size_t> cnt);
};

class compressor {
private:
  deque<bool> Q;
  void output(ofstream &out, bool);
public :
  void upcompress(string fin, string fout);
  void compress(string fin, string fout);
};


