/*
Copyright 2018 - 2019 Gordon Brown

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __ENZEN_INDEX_H__
#define __ENZEN_INDEX_H__

namespace enzen {

/*
 * @brief Structure which represents a three dimensional iteration space.
 */
class shape {
 public:
  /*
   * @brief Constructs a shape that represents the itertion space {w, h, d}.
   * @param w Width of iteration space.
   * @param h Height of iteration space.
   * @param d Depth of iteration space.
   */
  shape(size_t w, size_t h, size_t d) : iterationSpace_{w, h, d} {}

  /*
   * @brief Returns the value of a dimension of the iteration space.
   * @param dim Dimension of the iteration space to be returned.
   * @return The value of the iteration space at the specified dimension.
   */
  size_t operator[](int dim) const noexcept { return iterationSpace_[dim]; }

 private:
  size_t iterationSpace_[3];
};

/*
 * @brief Structure which represents an index within a three dimensional
 * iteration space.
 */
class index {
 public:
  /*
   * @brief Constructs an index that represents the point {x, y, z} within an
   * iteration space.
   * @param iterationSpace Iteration space.
   * @param x X dimension of index.
   * @param y Y dimension of index.
   * @param z Z dimension of index.
   */
  index(shape iterationSpace, size_t x, size_t y, size_t z)
      : iterationSpace_{iterationSpace}, index_{x, y, z} {}

  /*
   * @brief Constructs an index that represents the point {x, y, z} within the
   * iteration space {w, h, d}.
   * @param w Width of iteration space.
   * @param h Height of iteration space.
   * @param d Depth of iteration space.
   * @param x X dimension of index.
   * @param y Y dimension of index.
   * @param z Z dimension of index.
   */
  index(size_t w, size_t h, size_t d, size_t x, size_t y, size_t z)
      : iterationSpace_{w, h, d}, index_{x, y, z} {}

  /*
   * @brief Returns the value of a dimension of the index.
   * @param dim Dimension of the index to be returned.
   * @return The value of the index at the specified dimension.
   */
  size_t operator[](int dim) const noexcept { return index_[dim]; }

  /*
   * @brief Returns a shape object representing the iteration space the index is
   * within.
   * @return THe shape object that represents the iteration space.
   */
  shape get_shape() const noexcept { return iterationSpace_; }

 private:
  size_t index_[3];
  shape iterationSpace_;
};

}  // namespace enzen

#endif  // __ENZEN_INDEX_H__