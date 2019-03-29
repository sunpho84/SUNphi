#!/bin/bash -e

pushd /tmp

git clone https://github.com/sunpho84/yaml-cpp.git --depth=1

pushd yaml-cpp

mkdir build

pushd build

cmake \
    .. \
    -DYAML_CPP_BUILD_TESTS=no \
    -DYAML_CPP_BUILD_TOOLS=no \
    -DCMAKE_INSTALL_PREFIX:PATH=$HOME

make

make install

popd

popd

rm -fr yaml-cpp

popd
