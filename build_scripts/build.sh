#!/usr/bin/env bash

wget -qO - http://archive.neon.kde.org/public.key | sudo apt-key add -
echo "deb http://archive.neon.kde.org/user/ xenial main" | sudo tee /etc/apt/sources.list.d/neon.list
wget -qO - http://repo.nxos.org/public.key | sudo apt-key add -
echo "deb http://repo.nxos.org nxos main" | sudo tee /etc/apt/sources.list.d/nxos.list

sudo apt-get update -qq

sudo apt-get -y install --allow-unauthenticated inkscape git cmake patchelf extra-cmake-modules qtdeclarative5-dev-tools qml-module-qtgraphicaleffects plasma-framework-dev qtbase5-dev qtdeclarative5-dev libqt5xmlpatterns5-dev nomad-plasma-look-and-feel luv-icon-theme nomad-desktop-settings

cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
build_scripts/appimage.create.sh
find appdir -executable -type f -exec ldd {} \; | grep " => /usr" | cut -d " " -f 2-3 | sort | uniq

if [ -z "$TRAVIS_TAG" ]; then
  export TRAVIS_TAG="continuous";
  git tag -f $TRAVIS_TAG;
fi

mkdir out
mv -v Software_Center*.AppImage* out/
curl --upload-file out/Software_Center*.AppImage https://transfer.sh/nx_software_center