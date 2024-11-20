FROM ubuntu:22.04
LABEL Description="Ultramat build env"

# Disable asking for geographic area
ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /ultramat

RUN apt-get update && apt-get upgrade -y
RUN apt-get install --no-install-recommends -y \
    build-essential \
    gcc git make pkg-config meson \
    libgtk-3-0 libgtk-3-dev
