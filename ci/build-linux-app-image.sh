#!/bin/bash

# ----------------------------------------------------------------------
# Script to make the Linux deployment.
# ----------------------------------------------------------------------

# HACK: Remove library so that the app image tool uses the correct one
# instead.
sudo rm /usr/local/lib/android/sdk/emulator/lib64/qt/plugins/platforms/libqxcb.so

# Strip binaries.
strip /home/runner/work/genealogy-maker/genealogy-maker/genealogy-maker

# Get the current directory.
CURRENT_DIR=$PWD

# Create the "AppDir" folder.
APP_DIR=appdir/genealogy-maker.AppDir
mkdir -p $APP_DIR

# Copy the icon.
ICON_DIR="$APP_DIR/usr/share/icons/hicolor/256x256/apps"
mkdir -p $ICON_DIR

cp linux-deploy-files/genealogy-maker.png $ICON_DIR/genealogy-maker.png

# Copy the desktop file.
DESKTOP_DIR="$APP_DIR/usr/share/applications"
mkdir -p $DESKTOP_DIR

cp linux-deploy-files/genealogy-maker.desktop $DESKTOP_DIR

# Copy the executable.
BIN_DIR="$APP_DIR/usr/bin"
mkdir -p $BIN_DIR

cp /home/runner/work/genealogy-maker/genealogy-maker/genealogy-maker $BIN_DIR

# Using linuxdeployqt.
wget -c https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage

chmod +x linuxdeployqt-*.AppImage

# Run the program.
./linuxdeployqt-*.AppImage $DESKTOP_DIR/ping_viewer.desktop -verbose=2 -appimage -qmldir=/home/runner/work/genealogy-maker/genealogy-maker/
