#pragma once

namespace xdiag::basis::electron {

template <typename bit_t, typename coeff_t, bool symmetric, class basis_t,
          class apply_f, class fill_f>
void generic_term_diagonal(Coupling cpl, basis_t &&basis, apply_f &&apply,
                           fill_f &&fill) try {
  if constexpr (symmetric) {
    int64_t idx = 0;

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int64_t idx_ups = 0; idx_ups < basis.n_rep_ups(); ++idx_ups) {
      bit_t ups = basis.rep_ups(idx_ups);
      auto dnss = basis.dns_for_ups_rep(ups);
      for (bit_t dns : dnss) {
        coeff_t c = apply(ups, dns);
        fill(idx, idx, c);
        ++idx;
      }
    }

  } else { // if not symmetric
#ifdef _OPENMP
#pragma omp parallel
    {
      auto ups_and_idces = basis.states_indices_ups_thread();
#else
    auto ups_and_idces = basis.states_indices_ups();
#endif

      for (auto [ups, idx_up] : ups_and_idces) {
        int64_t idx = idx_up * basis.size_dns();
        for (bit_t dns : basis.states_dns()) {
          coeff_t c = apply(ups, dns);
          fill(idx, idx, c);
          ++idx;
        }
      }

#ifdef _OPENMP
    }
#endif
  }
} catch (Error const &e) {
  XDIAG_RETHROW(e);
}

} // namespace xdiag::basis::electron
