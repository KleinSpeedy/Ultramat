FROM ubuntu:20.04
LABEL Description="Ultramat build env"

WORKDIR /ultramat

# Get GTK dependencies
RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essentials \
    gcc cmake git pkg-config \
    libgtk-3-0

# Copy project files for building afterwards
COPY . /ultramat
