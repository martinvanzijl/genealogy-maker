#!/bin/bash

# ----------------------------------------------------------------------
# Script to make the Linux deployment.
# ----------------------------------------------------------------------

# Get the current directory.
CURRENT_DIR=$PWD

# Create the "AppDir" folder.
APP_DIR=~/AppDirs/genealogy-maker.AppDir

#APP_DIR="$CURRENT_DIR/AppDirs/genealogy-maker.AppDir"
mkdir -p $APP_DIR

# Copy the icon.
ICON_DIR="$APP_DIR/genealogy-maker/share/icons/hicolor/256x256/apps"
mkdir -p $ICON_DIR

cp linux-deploy-files/genealogy-maker.png $ICON_DIR/genealogy-maker.png
#cp linux-deploy-files/genealogy-maker.png $APP_DIR

# Copy the desktop file.
DESKTOP_DIR="$APP_DIR/genealogy-maker/share/applications"
mkdir -p $DESKTOP_DIR

cp linux-deploy-files/genealogy-maker.desktop $DESKTOP_DIR

# Copy the executable.
BIN_DIR="$APP_DIR/genealogy-maker/bin"
mkdir -p $BIN_DIR

cp ~/Documents/build-genealogymaker-Desktop-Release/genealogymaker "$BIN_DIR/genealogy-maker"

# Make the deployment directory.
DEST_DIR="genealogy-maker-v29"
mkdir -p $DEST_DIR

# Go to the deployment directory.
cd $DEST_DIR

# Make the deployment.
~/Downloads/build-linuxdeployqt-Desktop-Debug/bin/linuxdeployqt "$DESKTOP_DIR/genealogy-maker.desktop" -verbose=2 -appimage -ignore-glob="/home/martin/*"

# Copy the GEDCOM Python files.
cp -R ../python-gedcom-1.0.0/ .

# Copy the help files.
cp -R ../doc/ .

# Copy the example diagrams.
cp -R ../examples/ .

# Go back.
cd ..
