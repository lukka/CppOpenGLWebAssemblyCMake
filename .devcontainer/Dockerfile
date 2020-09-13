FROM emscripten/emsdk:2.0.3

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends ninja-build libx11-dev xorg-dev libglu1-mesa-dev  libopenal-dev gdb

RUN echo "alias lr='ls -lart' \n alias cd..='cd ..'" >> "$HOME/.bashrc"