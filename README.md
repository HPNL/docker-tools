# images-docker

The base docker container that need to use in [TCP-IP-Laboratory](https://github.com/UT-Network-Lab/TCP-IP-Laboratory).

## terminal container

[![Docker Pulls](https://img.shields.io/docker/pulls/utnetlab/term.svg)](https://hub.docker.com/r/utnetlab/term/)
[![Docker Stars](https://img.shields.io/docker/stars/utnetlab/term.svg)](https://hub.docker.com/r/utnetlab/term/)

DockerHub repository:

```bash
docker pull utnetlab/term
```

Github repository:

```bash
docker pull ghcr.io/ut-network-lab/docker-tools/term:latest
docker tag ghcr.io/ut-network-lab/docker-tools/term:latest utnetlab/term:latest
```

## web-desktop container

[![Docker Pulls](https://img.shields.io/docker/pulls/utnetlab/web-desktop.svg)](https://hub.docker.com/r/utnetlab/web-desktop/)
[![Docker Stars](https://img.shields.io/docker/stars/utnetlab/web-desktop.svg)](https://hub.docker.com/r/utnetlab/web-desktop/)

```bash
docker pull utnetlab/web-desktop
```

## graphical container

[![Docker Pulls](https://img.shields.io/docker/pulls/utnetlab/gui.svg)](https://hub.docker.com/r/utnetlab/gui/)
[![Docker Stars](https://img.shields.io/docker/stars/utnetlab/gui.svg)](https://hub.docker.com/r/utnetlab/gui/)

Based on `web-desktop` with some network client tools.

DockerHub repository:

```bash
docker pull utnetlab/gui
```

Github repository:

```bash
docker pull ghcr.io/ut-network-lab/docker-tools/gui:latest
docker tag ghcr.io/ut-network-lab/docker-tools/gui:latest utnetlab/gui:latest
```

## mininet container

[![Docker Pulls](https://img.shields.io/docker/pulls/utnetlab/mininet.svg)](https://hub.docker.com/r/utnetlab/mininet/)
[![Docker Stars](https://img.shields.io/docker/stars/utnetlab/mininet.svg)](https://hub.docker.com/r/utnetlab/mininet/)

Based on `web-desktop` with {`mininet`, `wireshark`} and some network tools package.

```bash
docker pull utnetlab/mininet
```
