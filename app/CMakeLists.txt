# Copyright 2025 The Manifold Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

if(NOT MANIFOLD_TEST)
  return()
endif()

add_executable(manifold_levelset src/levelset.cpp)
target_link_libraries(manifold_levelset PRIVATE manifold)
target_compile_options(manifold_levelset PRIVATE ${MANIFOLD_FLAGS})

add_executable(manifold_primitives src/primitives.cpp)
target_link_libraries(manifold_primitives PRIVATE manifold)
target_compile_options(manifold_primitives PRIVATE ${MANIFOLD_FLAGS})

add_executable(manifold_boolean src/boolean.cpp)
target_link_libraries(manifold_boolean PRIVATE manifold)
target_compile_options(manifold_boolean PRIVATE ${MANIFOLD_FLAGS})

add_executable(manifold_transform src/transform.cpp)
target_link_libraries(manifold_transform PRIVATE manifold)
target_compile_options(manifold_transform PRIVATE ${MANIFOLD_FLAGS})

add_executable(manifold_extrude src/extrude.cpp)
target_link_libraries(manifold_extrude PRIVATE manifold)
target_compile_options(manifold_extrude PRIVATE ${MANIFOLD_FLAGS})

add_executable(manifold_smooth src/smooth.cpp)
target_link_libraries(manifold_smooth PRIVATE manifold)
target_compile_options(manifold_smooth PRIVATE ${MANIFOLD_FLAGS})

add_executable(manifold_hull src/hull.cpp)
target_link_libraries(manifold_hull PRIVATE manifold)
target_compile_options(manifold_hull PRIVATE ${MANIFOLD_FLAGS})

add_executable(manifold_slice src/slice.cpp)
target_link_libraries(manifold_slice PRIVATE manifold)
target_compile_options(manifold_slice PRIVATE ${MANIFOLD_FLAGS})

add_executable(manifold_warp src/warp.cpp)
target_link_libraries(manifold_warp PRIVATE manifold)
target_compile_options(manifold_warp PRIVATE ${MANIFOLD_FLAGS})

add_executable(manifold_info src/info.cpp)
target_link_libraries(manifold_info PRIVATE manifold)
target_compile_options(manifold_info PRIVATE ${MANIFOLD_FLAGS})

if(MANIFOLD_CROSS_SECTION)
  add_executable(manifold_cross src/cross.cpp)
  target_link_libraries(manifold_cross PRIVATE manifold)
  target_compile_options(manifold_cross PRIVATE ${MANIFOLD_FLAGS})
endif()
