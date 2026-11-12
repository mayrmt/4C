#!/usr/bin/env bash

# This file is part of 4C multiphysics licensed under the
# GNU Lesser General Public License v3.0 or later.
#
# See the LICENSE.md file in the top-level for license information.
#
# SPDX-License-Identifier: LGPL-3.0-or-later

# Install necessary packages and linkages for compilation on macos.
# Pass command line option --for-github-workflow to enable setup
# that is specific to the GitHub Actions runner.

# Exit the script at the first failure
set -e

if [[ $# -gt 1 || ( $# -eq 1 && "$1" != "--for-github-workflow" ) ]]; then
  echo "Usage: $0 [--for-github-workflow]" >&2
  exit 2
fi

for_github_workflow=false
if [[ $# -eq 1 ]]; then
  for_github_workflow=true
fi

echo "System information:"
g++ -v
uname
sysctl -n hw.physicalcpu

brew update
brew install openmpi hdf5 llvm boost cln metis netcdf lld scalapack fftw vtk gmsh qhull
brew install suite-sparse

if [[ "$for_github_workflow" == true ]]; then
  sudo ln -sf "$(brew --prefix gcc@14)/bin/gfortran-14" /opt/homebrew/bin/gfortran
  brew install jpeg-turbo python@3.14
  brew uninstall --cask --force firefox google-chrome microsoft-edge

  echo "DYLD_LIBRARY_PATH=/opt/homebrew/lib:${DYLD_LIBRARY_PATH}" >> "$GITHUB_ENV"
  echo "HOME=$HOME" >> "$GITHUB_ENV"

  # On macOS 26, Gmsh dependencies can be installed to the wrong location.
  mkdir -p "$HOME/opt"
  sudo chown -R "$(whoami)" "$HOME/opt"
  sudo mkdir -p /opt/homebrew/Cellar/fltk/1.4.4/lib
  sudo mkdir -p /opt/homebrew/Cellar/libpng/1.6.54_1/lib
  sudo ln -sf /opt/homebrew/opt/fltk/lib/libfltk_images.1.4.dylib /opt/homebrew/Cellar/fltk/1.4.4/lib/libfltk_images.dylib
  sudo ln -sf /opt/homebrew/opt/libpng/lib/libpng16.16.dylib /opt/homebrew/Cellar/libpng/1.6.54_1/lib/libpng16.dylib
  sudo ln -sf /opt/homebrew/opt/libpng/lib/libpng.dylib /opt/homebrew/Cellar/libpng/1.6.54_1/lib/libpng.dylib
  sudo ln -sf /opt/homebrew/opt/jpeg-turbo/lib/libjpeg.8.dylib /opt/homebrew/Cellar/libpng/1.6.54_1/lib/libjpeg.dylib
  sudo ln -sf /opt/homebrew/opt/fltk/lib/libfltk.1.4.dylib /opt/homebrew/Cellar/fltk/1.4.4/lib/libfltk.dylib
  sudo ln -sf /opt/homebrew/opt/fltk/lib/libfltk_gl.1.4.dylib /opt/homebrew/Cellar/fltk/1.4.4/lib/libfltk_gl.dylib
fi
