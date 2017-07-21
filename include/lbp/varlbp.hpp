#ifndef LBP_VARLBP_HPP
#define LBP_VARLBP_HPP

#include <lbp/defs.hpp>
#include <lbp/detail/neighborhoods.hpp>
#include <lbp/detail/sampling.hpp>

#include <opencv2/core.hpp>

#include <boost/hana/fold.hpp>
#include <boost/hana/size.hpp>
#include <boost/hana/tuple.hpp>

//
// @inproceedings{Ojala:2001:GLB:646260.685274,
//  author = {Ojala, Timo and Pietik\"{a}inen, Matti and M\"{a}enp\"{a}\"{a}, Topi},
//  title = {A Generalized Local Binary Pattern Operator for Multiresolution Gray Scale and Rotation Invariant Texture Classification},
//  booktitle = {Proceedings of the Second International Conference on Advances in Pattern Recognition},
//  series = {ICAPR '01},
//  year = {2001},
//  isbn = {3-540-41767-2},
//  pages = {397--406},
//  numpages = {10},
//  url = {http://dl.acm.org/citation.cfm?id=646260.685274},
//  acmid = {685274},
//  publisher = {Springer-Verlag},
//  address = {London, UK, UK},
// }
//

namespace lbp {
namespace varlbp_detail {

template< typename T >
auto varlbp = [](auto neighborhood, auto sampler) {
    namespace hana = boost::hana;
    using namespace hana::literals;

    const auto N = hana::size (neighborhood).value - 1;

    return [=](const cv::Mat& src, size_t i, size_t j) {
        double d = 0., m = 0.;

        const auto s = hana::fold_left (
            neighborhood, 0, [&, k = 0](auto accum, auto c) mutable {
                const auto x = sampler (src, i + c [0_c], j + c [1_c]);

                d = x - m;
                m += d / ++k;

                return accum + d * (x - m);
            });

        return s / N;
    };
};

} // namespace varlbp_detail

template< typename T, size_t R, size_t P >
auto varlbp = [](const cv::Mat& src) {
    cv::Mat dst (src.size (), src.type ());

    auto op = varlbp_detail::varlbp< T > (
        detail::circular_neighborhood< R, P >,
        detail::bilinear_sampler< T >);

#pragma omp parallel for
    for (size_t i = R; i < src.rows - R - 1; ++i) {
        for (size_t j = R; j < src.cols - R - 1; ++j) {
            dst.at< T > (i, j) = op (src, i, j);
        }
    }

    return dst;
};


} // namespace lbp

#endif // LBP_VARLBP_HPP
