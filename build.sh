#!/bin/sh

mkdir build && \
  cd build && \
  cmake ../ && \
  make smallsh && \
  cd ../