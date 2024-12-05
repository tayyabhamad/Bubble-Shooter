#!/bin/bash

echo "Updating package lists..."
sudo apt-get update

echo "Installing required libraries..."
sudo apt-get install -y \
    build-essential \
    g++ \
    libsdl2-dev \
    libsdl2-mixer-dev \
    libsoil-dev \
    libfreeimage-dev \
    libglu1-mesa-dev \
    libgl1-mesa-dev \
    libglut-dev \
    libopenal-dev \
    libmpg123-dev \
    libsndfile1-dev \
    libx11-dev
echo "All dependencies installed. You can now build your project!"
