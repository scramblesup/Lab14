
FROM ubuntu:jammy


RUN apt update && apt install -y \
    build-essential \
    qt6-base-dev \
    libxcb-randr0-dev \
    wget \
    git \
    cmake \
    && rm -rf /var/lib/apt/lists/*


WORKDIR /app


COPY . /app


RUN wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage -O /usr/bin/linuxdeployqt


RUN chmod +x /usr/bin/linuxdeployqt

