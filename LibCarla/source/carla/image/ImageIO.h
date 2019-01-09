// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

//#include "carla/image/ImageIOConfig.h"

namespace carla {
namespace image {

  class ImageIO {
  public:

    template <typename ImageT>
    static void ReadImage(const std::string &, ImageT &) {
    }

    template <typename ViewT>
    static std::string WriteView(std::string out_filename, const ViewT &) {
      return out_filename;
    }
  };

} // namespace image
} // namespace carla
