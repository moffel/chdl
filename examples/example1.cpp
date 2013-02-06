#include <iostream>
#include <fstream>

#include <netlist.h>

#include <gateops.h>
#include <bvec-basic.h>

#include <adder.h>

#include <opt.h>
#include <tap.h>
#include <sim.h>

using namespace std;
using namespace chdl;

int main(int argc, char **argv) {
  // The design
  rvec<4> r(Reg<4>());
  bvec<8> t;

  r.connect(Adder(r, Lit<4>(1)));

  t[range<0,3>()] = bvec<4>(r[range<0,3>()]);
  t[range<4,7>()] = r[range<0,3>()];

  cerr << "t[0] = " << t[0] << endl;

  TAP(r); TAP(t);

  // The simulation (generate .vcd file)
  optimize();
  run(cout, 8);

  ofstream netlist_file("example1.nand");
  print_netlist(netlist_file);
  netlist_file.close();
}
