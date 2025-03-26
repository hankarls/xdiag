#include "../../catch.hpp"

#include <iostream>

#include "../electron/testcases_electron.hpp"
#include "../spinhalf/testcases_spinhalf.hpp"
#include "../tj/testcases_tj.hpp"
#include <xdiag/algebra/apply.hpp>
#include <xdiag/algebra/isapprox.hpp>
#include <xdiag/algebra/matrix.hpp>
#include <xdiag/algorithms/sparse_diag.hpp>
#include <xdiag/states/create_state.hpp>

using namespace xdiag;

static void test_regular_block_e0(OpSum ops, int nsites, int nup, int ndn) {
  auto block = Electron(nsites, nup, ndn);
  auto blockd = ElectronDistributed(nsites, nup, ndn);
  double e0 = eigval0(ops, block);
  double e0d = eigval0(ops, blockd);
  REQUIRE(isapprox(e0, e0d));
}

TEST_CASE("electron_distributed_apply", "[electron_distributed]") try {
  using namespace xdiag::testcases::electron;

  Log("electron_distributed_apply test");
  int N = 4;

  // Play-around test
  for (int nup = 0; nup <= N; ++nup) {
    for (int ndn = 0; ndn <= N; ++ndn) {
      auto block = ElectronDistributed(N, nup, ndn);
      OpSum ops;
      for (int i = 0; i < N; ++i) {
        ops += "Jz" * Op("SzSz", {i, (i + 1) % N});
        ops += "Jx" * Op("Exchange", {i, (i + 1) % N});
        ops += "TDN" * Op("Hopdn", {i, (i + 1) % N});
        ops += "TUP" * Op("Hopup", {i, (i + 1) % N});
      }
      ops["Jz"] = 1.32;
      ops["Jx"] = complex(.432, .576);
      ops["TDN"] = complex(-0.1432, .3576);
      ops["TUP"] = complex(-0.4321, .5763); // 2.104;

      ops += 8.34 * Op("HubbardU");

      double e0 = eigval0(ops, block);
      auto block2 = Electron(N, nup, ndn);
      double e02 = eigval0(ops, block2);
      // Log("{} {} {:.12f} {:.12f}", nup, ndn, e0, e02);
      REQUIRE(isapprox(e0, e02));
    }
  }

  ///////////////////////////////////////////////////
  // Test Fermion all to all, free fermions (real)
  for (int nsites = 3; nsites < 7; ++nsites) {

    Log("electron_distributed_apply: Hubbard random all-to-all test (real), N: "
        "{}",
        nsites);
    OpSum ops = freefermion_alltoall(nsites);
    ops["U"] = 5.0;

    for (int nup = 0; nup <= nsites; ++nup) {
      for (int ndn = 0; ndn <= nsites; ++ndn) {
        test_regular_block_e0(ops, nsites, nup, ndn);
      }
    }
  }

  // /////////////////
  // Test Fermion all to all, free fermions (cplx, up/dn different)
  for (int nsites = 3; nsites < 7; ++nsites) {
    Log("electron_apply: Hubbard random all-to-all test (cplx), N: {}", nsites);
    OpSum ops = freefermion_alltoall_complex_updn(nsites);
    ops["U"] = 5.0;

    for (int nup = 0; nup <= nsites; ++nup) {
      for (int ndn = 0; ndn <= nsites; ++ndn) {
        test_regular_block_e0(ops, nsites, nup, ndn);
      }
    }
  }

  ////////////////////////////
  // Henry's MATLAB code test (tests Heisenberg terms)
  Log("electron_apply: U-hopping-HB apply of Henry's Matlab code");
  {
    auto [ops, eigs_correct] = randomAlltoAll4NoU();

    int nsites = 4;
    for (int nup = 0; nup <= nsites; ++nup) {
      for (int ndn = 0; ndn <= nsites; ++ndn) {
        test_regular_block_e0(ops, nsites, nup, ndn);
      }
    }
  }

  ////////////////////////////////
  // all-to-all complex
  for (int nsites = 3; nsites <= 5; ++nsites) {
    Log.out("electron_apply: random all-to-all complex exchange test N={}",
            nsites);
    auto ops = xdiag::testcases::tj::tj_alltoall_complex(nsites);
    for (int nup = 0; nup <= nsites; ++nup) {
      for (int ndn = 0; ndn <= nsites; ++ndn) {
        test_regular_block_e0(ops, nsites, nup, ndn);
      }
    }
  }

} catch (Error e) {
  error_trace(e);
}
