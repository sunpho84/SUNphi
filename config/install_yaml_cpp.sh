#!/bin/bash -e

pushd /tmp

git clone git@github.com:sunpho84/yaml-cpp.git

pushd yaml-cpp

mkdir build

pushd build

cmake ..

make

make install

popd

popd

rm -fr yaml-cpp

popd
