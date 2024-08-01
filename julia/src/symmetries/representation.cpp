#include "representation.hpp"

namespace xdiag::julia {
void define_representation(jlcxx::Module &mod) {

  mod.add_type<Representation>("cxx_Representation")
      .constructor<>()
      .constructor<std::vector<complex> const &>()
      .method("isreal", &Representation::isreal)
      .method("size", &Representation::size);

  mod.method("multiply",
             [](Representation const &r1, Representation const &r2) {
               JULIA_XDIAG_CALL_RETURN(multiply(r1, r2));
             });

  mod.method("to_string", [](Representation const &r) { return to_string(r); });
}

} // namespace xdiag::julia
