#pragma once

#include <RAJA/RAJA.hpp>
#include <caeror/raja_layouts.h>

#include <cassert>
#include <cstddef>
#include <limits>
#include <new>

#include "vis/indexes.h"

namespace vis {

class MaterialBank {
  public:
  using MaterialLayout = caeror::CaerorLayout<caeror::MaterialID>;
  using MaterialView = caeror::CaerorWritableView<double, MaterialLayout>;

  explicit MaterialBank(RAJA::resources::Resource& resource,
                        caeror::CaerorIndexType num_materials)
      : resource_(resource), layout_(num_materials) {
    assert(num_materials != 0);

    sigma_t_ = resource_.allocate<double>(num_materials);
    resource_.memset(sigma_t_, 0, num_materials * sizeof(double));

    sigma_s_ = resource_.allocate<double>(num_materials);
    resource_.memset(sigma_s_, 0, num_materials * sizeof(double));

    mu_c_ = resource_.allocate<double>(num_materials);
    resource_.memset(mu_c_, 0, num_materials * sizeof(double));

    eta_ = resource_.allocate<double>(num_materials);
    resource_.memset(eta_, 0, num_materials * sizeof(double));

    sigma_tr_ = resource_.allocate<double>(num_materials);
    resource_.memset(sigma_tr_, 0, num_materials * sizeof(double));

    sigma_sl_ = resource_.allocate<double>(num_materials);
    resource_.memset(sigma_sl_, 0, num_materials * sizeof(double));

    scatter_ratio_ = resource_.allocate<double>(num_materials);
    resource_.memset(scatter_ratio_, 0, num_materials * sizeof(double));

    resource_.wait();
  }

  ~MaterialBank() {
    resource_.wait();
    for (auto* property : {sigma_t_, sigma_s_, mu_c_, eta_, sigma_tr_,
                           sigma_sl_, scatter_ratio_}) {
      resource_.deallocate(property);
    }
  }

  MaterialBank(const MaterialBank&) = delete;
  MaterialBank& operator=(const MaterialBank&) = delete;
  MaterialBank(MaterialBank&& other) = delete;
  MaterialBank& operator=(MaterialBank&& other) = delete;

  MaterialView GetTotalXSView() {
    return MaterialView(sigma_t_, layout_);
  }

  MaterialView GetScatterXSView() {
    return MaterialView(sigma_s_, layout_);
  }

  MaterialView GetMuCView() {
    return MaterialView(mu_c_, layout_);
  }

  MaterialView GetEtaView() {
    return MaterialView(eta_, layout_);
  }

  MaterialView GetTransportXSView() {
    return MaterialView(sigma_tr_, layout_);
  }
  
  MaterialView GetLargeScatterXSView() {
    return MaterialView(sigma_sl_, layout_);
  }

  MaterialView GetScatterRatioView() {
    return MaterialView(scatter_ratio_, layout_);
  }

  RAJA_HOST_DEVICE double SampleScatteringCosine(double xi, caeror::MaterialID m, const MaterialView& eta, const MaterialView& muc) {
    const double cutoff = muc(m);
    const double screening = eta(m);
    const double scaled_xi = (1 + cutoff) * xi;
    return -1 + 2 * (1 + screening) * (scaled_xi / (1 - cutoff + 2 * screening + scaled_xi));
  }

  void CalculateXS() {
    auto eta = GetEtaView(); 
    auto muc = GetMuCView();
    auto scatter = GetScatterXSView();
    auto total = GetTotalXSView();

    auto transport = GetTransportXSView();
    auto large_scatter = GetLargeScatterXSView();
    auto scatter_ratio = GetScatterRatioView();

    for (auto m : RAJA::TypedRangeSegment<caeror::MaterialID>(0, layout_.size())) {
      const double coeff = 1 - muc(m) + 2 * eta(m);
      transport(m) = 2*scatter(m)*eta(m)*(1+eta(m))*(log(coeff/(2*eta(m))) - (1-muc(m))/(coeff));
      large_scatter(m) = scatter(m)*eta(m)*(1 + muc(m)) / coeff;
      scatter_ratio(m) = large_scatter(m) / total(m);
    }
  }

  private:

  RAJA::resources::Resource& resource_;
  MaterialLayout layout_;
  double* sigma_t_ = nullptr;
  double* sigma_s_ = nullptr;
  double* mu_c_ = nullptr;
  double* eta_ = nullptr;
  double* sigma_tr_ = nullptr;
  double* sigma_sl_ = nullptr;
  double* scatter_ratio_ = nullptr;
};
  
} // namespace vis 