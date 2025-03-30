FROM ubuntu:22.04
LABEL Description="Ultramat build env"

# Disable asking for geographic area
ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /ultramat

RUN touch /etc/apt/sources.list.d/arm-cross-compile-sources.list

RUN echo '\
    deb [arch=arm64] http://ports.ubuntu.com/ jammy main restricted \n\
    deb [arch=arm64] http://ports.ubuntu.com/ jammy-updates main restricted \n\
    deb [arch=arm64] http://ports.ubuntu.com/ jammy universe \n\
    deb [arch=arm64] http://ports.ubuntu.com/ jammy-updates universe \n\
    deb [arch=arm64] http://ports.ubuntu.com/ jammy multiverse \n\
    deb [arch=arm64] http://ports.ubuntu.com/ jammy-updates multiverse \n\
    deb [arch=arm64] http://ports.ubuntu.com/ jammy-backports main restricted universe multiverse' \
    >> /etc/apt/sources.list.d/arm-cross-compile-sources.list

# Prefix every deb entry with build architecture
RUN sed -i 's/^deb /deb [arch=amd64] /' /etc/apt/sources.list

# Add raspberry pi architecture
RUN dpkg --add-architecture arm64

RUN apt-get update
# Install gcc and libgtk for both x86 and arm64
RUN apt-get install --no-install-recommends -y \
    wget meson cmake pkg-config ca-certificates xz-utils dpkg-dev \
    gcc-aarch64-linux-gnu g++-aarch64-linux-gnu gcc g++ \
    libgtk-3-dev:arm64 libgtk-3-dev
