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
#ifndef DRACO_IO_MESH_PLY_DECODER_H_
#define DRACO_IO_MESH_PLY_DECODER_H_

#include <string>

#include "core/decoder_buffer.h"
#include "io/ply_reader.h"
#include "mesh/mesh.h"

namespace draco {

// Decodes a Wavefront OBJ file into draco::Mesh (or draco::PointCloud if the
// connectivity data is not needed).
// TODO(ostava): The current implementation assumes that the input vertices are
// defined with x, y, z properties. The decoder also reads uint8 red, green,
// blue, alpha color information, but all other attributes are ignored for now.
class PlyDecoder {
 public:
  PlyDecoder();

  // Decodes an obj file stored in the input file.
  // Returns nullptr if the decoding failed.
  bool DecodeFromFile(const std::string &file_name, Mesh *out_mesh);
  bool DecodeFromFile(const std::string &file_name,
                      PointCloud *out_point_cloud);

  bool DecodeFromBuffer(DecoderBuffer *buffer, Mesh *out_mesh);
  bool DecodeFromBuffer(DecoderBuffer *buffer, PointCloud *out_point_cloud);

 protected:
  bool DecodeInternal();
  DecoderBuffer *buffer() { return &buffer_; }

 private:
  bool DecodeFaceData(const PlyElement *face_element);
  bool DecodeVertexData(const PlyElement *vertex_element);

  DecoderBuffer buffer_;

  // Data structure that stores the decoded data. |out_point_cloud_| must be
  // always set but |out_mesh_| is optional.
  Mesh *out_mesh_;
  PointCloud *out_point_cloud_;
};

}  // namespace draco

#endif  // DRACO_IO_MESH_PLY_DECODER_H_
