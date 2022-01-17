#!/bin/bash

# ----------------------------------------------------------------------
# Script to make the Linux deployment.
# ----------------------------------------------------------------------

# Copy the executable.
cp ~/Documents/build-genealogymaker-Desktop-Release/genealogymaker ~/AppDirs/genealogy-maker.AppDir/genealogy-maker/bin/genealogy-maker

# Make the deployment directory.
DEST_DIR="genealogy-maker-v12"
mkdir $DEST_DIR

# Go to the deployment directory.
cd $DEST_DIR

# Make the deployment.
~/Downloads/build-linuxdeployqt-Desktop-Debug/bin/linuxdeployqt ~/AppDirs/genealogy-maker.AppDir/genealogy-maker/share/applications/genealogy-maker.desktop -verbose=2 -appimage -ignore-glob="/home/martin/*"

# Copy the GEDCOM Python files.
cp -R ../python-gedcom-1.0.0/ .

# Go back.
cd ..
