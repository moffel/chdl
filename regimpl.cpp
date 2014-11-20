#include <map>

#include "node.h"
#include "gates.h"
#include "reg.h"
#include "regimpl.h"
#include "nodeimpl.h"

#include "hierarchy.h"

using namespace chdl;
using namespace std;

regimpl::regimpl(node d): q(0), next_q(0), d(d) {}

regimpl::~regimpl() {}

bool regimpl::eval(evaluator_t &e) { return q; }

void regimpl::print(ostream &out) {
  out << "  reg " << d << ' ' << id << endl;
}

void regimpl::print_vl(ostream &out) {
  const bool reset_signal(false), level_trig_reset(false);

  if (!reset_signal) {
    out << "  initial" << endl
        << "    begin" << endl
        << "      __x" << id << " <= 0;" << endl
        << "    end" << endl;
  }

  out << "  always @ (posedge phi)" << endl;
  if (reset_signal && level_trig_reset) {
    out << "  if (!reset)" << endl;
  }
  out << "    begin" << endl
      << "      __x" << id << " <= " << "__x" << d << ';' << endl
      << "    end" << endl;

  if (reset_signal) {
    out << "  always @ (posedge reset)" << endl
        << "    begin" << endl
        << "      __x" << id << " <= " << "0;" << endl
        << "    end" << endl;
  }
}

// Do nothing-- the value is already there. 
void regimpl::gen_eval(evaluator_t &e, execbuf &b, nodebuf_t &from) {}
void regimpl::gen_store_result(execbuf &b, nodebuf_t &from, nodebuf_t &to) {}

void regimpl::gen_tick(evaluator_t &e, execbuf &b,
                       nodebuf_t &from, nodebuf_t &to)
{
  b.push(char(0x48)); // mov &from[d], %rbx
  b.push(char(0xbb));
  b.push((void*)&from[d]);

  b.push(char(0x8b)); // mov *%rbx, %eax
  b.push(char(0x03));

  b.push(char(0x48)); // mov &to[id], %rbx
  b.push(char(0xbb));
  b.push((void*)&to[id]);

  b.push(char(0x89)); // mov %eax, *%rbx
  b.push(char(0x03));  
}

void regimpl::gen_tock(evaluator_t &e, execbuf &b,
                       nodebuf_t &from, nodebuf_t &to)
{
}

node chdl::Reg(node d, bool val) {
  HIERARCHY_ENTER();  
  if (val) {
    node r((new regimpl(Inv(d)))->id);
    HIERARCHY_EXIT();
    return Inv(r);
  } else {
    node r((new regimpl(d))->id);
    HIERARCHY_EXIT();
    return r;
  }
}

node chdl::Wreg(node w, node d, bool initial) {
  HIERARCHY_ENTER();
  node q;
  q = Reg(Mux(w, q, d), initial);
  HIERARCHY_EXIT();
  return q;
}

void chdl::get_reg_nodes(set<nodeid_t> &s) {
  // We used to use a map<node, node> to keep track of all registers. This
  // requires less maintenance:
  for (size_t i = 0; i < nodes.size(); ++i) {
    regimpl *r(dynamic_cast<regimpl*>(nodes[i]));
    if (r) { s.insert(r->id); s.insert(r->d); }
  }
}

void chdl::get_reg_d_nodes(set<nodeid_t> &s) {
  for (size_t i = 0; i < nodes.size(); ++i) {
    regimpl *r(dynamic_cast<regimpl*>(nodes[i]));
    if (r) s.insert(r->d);
  }
}

void chdl::get_reg_q_nodes(set<nodeid_t> &s) {
  for (size_t i = 0; i < nodes.size(); ++i) {
    regimpl *r(dynamic_cast<regimpl*>(nodes[i]));
    if (r) s.insert(r->id);
  }
}
