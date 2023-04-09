FROM ubuntu:20.04
LABEL Description="Ultramat build env"

# Disable asking for geographic area
ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /ultramat

# Get GTK dependencies
RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    gcc cmake git make pkg-config \
    libgtk-3-0 libgtk-3-dev
