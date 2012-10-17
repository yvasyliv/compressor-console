#include "compresor.h"


void compressor::compress(string fin, string fout) {
  ifstream in(fin.c_str(), ios::binary);
  ofstream out(fout.c_str(), ios::binary);

  map<unsigned char, size_t> cnt;
  assert(in.is_open());
  int insize = 0;
  char signedch;
  while ((in.get(signedch) || true) && in.gcount() != 0) {
    unsigned char ch = (unsigned char)signedch;
    ++insize;
    ++cnt[ch];
  }
  cerr << insize << endl;
  in.close();
  in.open(fin.c_str(), ios::binary);
  tree T;
  T.build(cnt);



  deque<bool> add_information(5, false);
  cerr << sizeof(size_t) << endl;
  size_t outsize = T.full_size(cnt);

  deque<bool> td = T.to_deque();
  outsize += td.size();
  cerr << outsize << endl;
  outsize = (8 - outsize % 8) % 8;
  for (int i = 2; i >= 0; --i)
      add_information.push_back((outsize >> i) & 1);
  assert(add_information.size() == 8);
  Q.insert(Q.end(), add_information.begin(), add_information.end());
  Q.insert(Q.end(), td.begin(), td.end());

  while ((in.get(signedch) || true) && in.gcount() != 0) {
    unsigned char ch = (unsigned char)signedch;
    deque<bool> tmp = T.codeof(ch);
    Q.insert(Q.end(), tmp.begin(), tmp.end());
    output(out, false);
  }
  output(out, true);

  in.close();
  out.close();
}

void compressor::output(ofstream &out, bool end) {
  if (end || Q.size() > 50000) {
      while (Q.size() >= 8 || (end && Q.size() > 0)) {
        unsigned char ch = 0;
        for (int i = 7; i >= 0 && (!end || Q.size() > 0); --i) {
          bool t = Q.front();
          Q.pop_front();
          ch |= t << i;
        }
        out.put(ch);
      }
  }
}

void vertex::set_symbol(bool b) {
  symbol = b;
}

size_t vertex::get_cnt() {
  return cnt;
}

deque<bool> tree::codeof(unsigned char ch) {
  if (m_code_of.count(ch))
    return m_code_of[ch];
  deque<bool> ans;
  assert(MP.count(ch));
  for (pvertex p = MP[ch]; p->perent != NULL; p = p->perent) {
    ans.push_front(p->symbol);
  }

  return m_code_of[ch] = ans;
}

void tree::build(map<unsigned char, size_t> cnt) {
  priority_queue<pvertex, vector<pvertex> , vertex> PQ;
  for (map<unsigned char, size_t>::iterator it = cnt.begin(); it != cnt.end(); ++it) {
    pvertex tv = new vertex(NULL, NULL, NULL, it->second);
    tv->vertexchar = it->first;
    PQ.push(tv);
    MP[it->first] = tv;
  }

  while (PQ.size() >= 2) {
    pvertex pl, pr;
    pl = PQ.top();
    PQ.pop();
    pr = PQ.top();
    PQ.pop();

    pl->set_symbol(0);
    pr->set_symbol(1);

    pvertex p = new vertex(NULL, pl, pr, pl->get_cnt() + pr->get_cnt());
    pl->perent = pr->perent = p;
    PQ.push(p);
  }
}

pvertex tree::build(bitreader &br) {
  vector<unsigned char> we_have;
  for (int ch = 0; ch < 256; ++ch) {
    if (br.get_for_sure()) {
      we_have.push_back(ch);
    }
  }
  pvertex root = new vertex(NULL, NULL, NULL, 0);
  for (size_t i = 0; i < we_have.size(); ++i) {
    size_t sz = 0;
    for (int j = 0; j < 8; ++j)
      sz = (sz << 1) + br.get_for_sure();
    pvertex pv = root;
    cerr << int(we_have[i]) << " ";
    for (; sz > 0; --sz) {
      bool c = br.get_for_sure();
      cerr << c;
      pvertex &pvnext = c ? pv->right : pv->left;
      if (pvnext == NULL) {
        pvnext = new vertex(pv, NULL, NULL, 0);
      }
      pv = pvnext;
    }
    pv->vertexchar = we_have[i];
    cerr << endl;
  }
  return root;
}

void tree::push_deque(deque<bool> &d, unsigned char ch) {
  for (int i = 7; i >= 0; --i)
    d.push_back((ch >> i) & 1);
}

deque<bool> tree::to_deque() {
  deque<bool> res(256, false);
  for (map<unsigned char, pvertex>::iterator it = MP.begin(); it != MP.end(); ++it) {
    //push_deque(res, it->first);
    res[it->first] = true;
    deque<bool> tmp = codeof(it->first);
    push_deque(res, (unsigned char)(tmp.size()));
    res.insert(res.end(), tmp.begin(), tmp.end());
    cerr << (int)it->first << " ";
    for (int i = 0; i < tmp.size(); ++i)
      cerr << tmp[i];
    cerr << endl;
  }
  return res;
}

bool bitreader::read_more() {
  assert(in.is_open());
  if (Q.empty()) {
    if (in.peek() == EOF)
      return false;
    unsigned char ch = in.get();
    int end = 0;
    if (in.peek() == EOF)
      end = cnt_of_spam_symbols;
    for (int i = 7; i >= end; --i)
      Q.push_back((ch >> i) & 1);
  }
  return true;
}

size_t tree::full_size(map<unsigned char , size_t> const &cnt) {
 size_t ans = 0;
 for (map<unsigned char, size_t>::const_iterator it = cnt.begin(); it != cnt.end(); ++it) {
     deque<bool> trash = codeof(it->first);
     ans += trash.size() * it->second;
 }
 return ans;
}

void compressor::upcompress(string fin, string fout) {
  ifstream in(fin.c_str(), ios::binary);
  ofstream out(fout.c_str(), ios::binary);
  unsigned char tmpbyte = in.get();
  size_t cntspam = tmpbyte & 7;
  bitreader br(in, cntspam);
  tree T;
  pvertex root = T.build(br);
  bool c;
  pvertex v = root;
  while (br.get(c)) {
    v = v->next(c);
    if (v->is_leaf()) {
      out.put(v->vertexchar);
      v = root;
    }
  }

  out.close();
}

bool vertex::is_leaf() {
  assert((left == NULL && right == NULL) || (left != NULL && right != NULL));
  return (left == NULL) && (right == NULL);
}

vertex * vertex::next(bool c) {
  return c ? right : left;
}

bool bitreader::get_for_sure() {
  if (Q.empty())
    assert(read_more());
  bool res = Q.front();
  Q.pop_front();
  return res;
}

bool bitreader::get(bool &res) {
  if (Q.empty() && !read_more())
    return false;
  res = Q.front();
  Q.pop_front();
  return true;
}
