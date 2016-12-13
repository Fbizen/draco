// Copyright 2016 The Draco Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#ifndef DRACO_COMPRESSION_POINT_CLOUD_ALGORITHMS_FLOAT_POINTS_KD_TREE_ENCODER_H_
#define DRACO_COMPRESSION_POINT_CLOUD_ALGORITHMS_FLOAT_POINTS_KD_TREE_ENCODER_H_

#include <memory>
#include <vector>

#include "compression/point_cloud/algorithms/point_cloud_types.h"
#include "compression/point_cloud/algorithms/quantize_points_3.h"
#include "core/encoder_buffer.h"

namespace draco {

// This class encodes a given point cloud based on the point cloud compression
// algorithm in:
// Olivier Devillers and Pierre-Marie Gandoin
// "Geometric compression for interactive transmission"
//
// In principle the algorithm keeps on splitting the point cloud in the middle
// while alternating the axes. For 3D this results in an Octree like structure.
// In each step we encode the number of points in the first half.
// The algorithm uses quantization and does not preserve the order of points.
//
// However, the algorithm here differs from the original as follows:
// The algorithm keeps on splitting the point cloud in the middle of the axis
// that keeps the point cloud as clustered as possible, which gives a better
// compression rate.
// The number of points is encode by the deviation from the half of the points
// in the smaller half of the two. This results in a better compression rate as
// there are more leading zeros, which is then compressed better by the
// arithmetic encoding.

// TODO(hemmer): Make name consistent with other point cloud encoders.
class FloatPointsKdTreeEncoder {
 public:
  FloatPointsKdTreeEncoder();
  explicit FloatPointsKdTreeEncoder(uint32_t quantization_bits,
                                    uint32_t compression_level);

  template <class InputIteratorT>
  bool EncodePointCloud(InputIteratorT points_begin, InputIteratorT points_end);
  EncoderBuffer *buffer() { return &buffer_; }

  uint32_t version() const { return version_; }
  uint32_t quantization_bits() const { return qinfo_.quantization_bits; }
  uint32_t &quantization_bits() { return qinfo_.quantization_bits; }
  uint32_t compression_level() const { return compression_level_; }
  uint32_t &compression_level() { return compression_level_; }
  float range() const { return qinfo_.range; }
  uint32_t num_points() const { return num_points_; }
  std::string identification_string() const {
    return "FloatPointsKdTreeEncoder";
  }

 private:
  void Clear() { buffer_.Clear(); }
  bool EncodePointCloudInternal(std::vector<Point3ui> *qpoints);

  static const uint32_t version_;
  QuantizationInfo qinfo_;
  uint32_t num_points_;
  EncoderBuffer buffer_;
  uint32_t compression_level_;
};

template <class InputIteratorT>
bool FloatPointsKdTreeEncoder::EncodePointCloud(InputIteratorT points_begin,
                                                InputIteratorT points_end) {
  Clear();

  // Collect necessary data for encoding.
  num_points_ = std::distance(points_begin, points_end);

  // TODO(hemmer): Extend quantization tools to make this more automatic.
  // Compute range of points for quantization
  std::vector<Point3ui> qpoints;
  qpoints.reserve(num_points_);
  QuantizePoints3(points_begin, points_end, &qinfo_,
                  std::back_inserter(qpoints));

  // Encode header.
  buffer()->Encode(version_);
  buffer()->Encode(qinfo_.quantization_bits);
  buffer()->Encode(qinfo_.range);
  buffer()->Encode(num_points_);
  buffer()->Encode(compression_level_);
  if (num_points_ == 0)
    return true;
  return EncodePointCloudInternal(&qpoints);
}

}  // namespace draco

#undef BIT_ENCODER_TYPE

#endif  // DRACO_COMPRESSION_POINT_CLOUD_ALGORITHMS_FLOAT_POINTS_KD_TREE_ENCODER_H_
