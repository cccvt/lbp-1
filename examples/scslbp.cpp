// -*- mode: c++; -*-

#include <iostream>
using namespace std;

#include <lbp/frame_range.hpp>
#include <lbp/scslbp.hpp>
#include <lbp/utils.hpp>

#include <boost/hana/integral_constant.hpp>
#include <boost/hana/tuple.hpp>
namespace hana = boost::hana;
using namespace hana::literals;

#include <options.hpp>
#include <run.hpp>

static void
f (cv::VideoCapture& cap, const options_t& opts) {
    const bool display = opts.have ("display");

    auto op = lbp::scslbp< float, 2, 12 > (/* .05 */);

    for (auto& frame : lbp::getframes_from (cap)) {
        lbp::frame_delay temp { 0 };

        const auto dst = op (lbp::bgr2gray (frame));

        if (display) {
            imshow (
                "Spatial Extended Center-Symmetric LBP",
                lbp::equalize (dst));
        }

        if (temp.wait_for_key (27))
            break;
    }
}

int main (int argc, char** argv) {
    program_options_from (argc, argv);
    return run_with (f, global_options ()), 0;
}
