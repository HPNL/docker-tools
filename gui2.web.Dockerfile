FROM utnetlab/web-desktop

COPY ssl /etc/nginx/ssl
ENV SSL_PORT=443

RUN apt-get update \
    && dpkg-query -f '${binary:Package}\n' -W | sort > base_packages \
    && DEBIAN_FRONTEND=noninteractive apt-get -y --no-install-recommends install \
        libc6-dev make curl ca-certificates build-essential autoconf automake autotools-dev meson ninja-build \
    && curl -OL https://github.com/troglobit/mtools/releases/download/v2.3/mtools-2.3.tar.gz \
    && tar xfz mtools-2.3.tar.gz \
    && cd mtools-2.3 \
    && make \
    && make install \
    && cd .. \
    && rm -r mtools-2.3* \
    && curl -OL https://github.com/iputils/iputils/archive/s20200821.tar.gz \
    && tar xzf s20200821.tar.gz \
    && cd iputils-s20200821/ \
    && meson builddir -Dprefix=/ -DUSE_GETTEXT=false -DBUILD_RARPD=false -DBUILD_TFTPD=false -DBUILD_TRACEROUTE6=false -DBUILD_NINFOD=false -DBUILD_HTML_MANS=false -DNO_SETCAP_OR_SUID=true -DBUILD_MANS=false -DBUILD_CLOCKDIFF=false -DBUILD_TRACEPATH=false -DBUILD_ARPING=false -DBUILD_PING=false -DBUILD_RDISC=true -DUSE_CAP=false \
    && make \
    && make install \
    && cd .. \
    && rm -r *s20200821* \
    && DEBIAN_FRONTEND=noninteractive apt-get -y --no-install-recommends install \
        sudo htop bash-completion screen less man-db  curl wget socat knot-host mtr-tiny nano vim \
        net-tools iperf3 traceroute tcpdump isc-dhcp-client icmpush iputils-ping \
        telnet ftp tftp rdate snmp ntp ntpdate netcat arping iproute2 openssh-client iptables \
        openssl ifupdown \
        vlc \
    && rm -rf /var/lib/apt/lists/* && rm -f /var/cache/apt/archives/*.deb
    #&& dpkg-query -f '${binary:Package}\n' -W | sort > packages \
    #&& DEBIAN_FRONTEND=noninteractive apt-get -y purge \
    #    `comm -13 base_packages packages` 
    # && rm -f base_packages packages \

# add netlab user
RUN useradd -m netlab -s /bin/bash && \
    adduser netlab sudo && echo "netlab:netlab" | chpasswd netlab && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers && \
    mkdir /home/netlab/code

# config system
COPY bashrc /root/.bashrc
COPY bashrc /home/netlab/.bashrc

COPY mibs/* /usr/share/snmp/mibs/
# copy program file
COPY *.c /home/netlab/code/
COPY sock /home/netlab/code/sock

RUN cd /home/netlab/code/ && \
    gcc netspydd.c -o netspydd && \
    gcc netspyd.c -o netspyd && \
    gcc netspy.c -o netspy && \
    gcc TCPclient.c -o TCPclient && \
    gcc TCPserver.c -o TCPserver && \
    gcc UDPclient.c -o UDPclient && \
    gcc UDPserver.c -o UDPserver && \
    gcc rdisc.c -o rdisc && \
    cd sock && autoreconf -if && ./configure && make && \
    cp ./src/sock /usr/local/bin/socket && cd .. && rm -rf sock

# Don't ignore all ICMP ECHO and TIMESTAMP requests sent to it via broadcast/multicast
RUN echo "net.ipv4.icmp_echo_ignore_broadcasts = 0" >> /etc/sysctl.conf

# Disable root check for vlc
RUN sed -i 's/geteuid/getppid/' /usr/bin/vlc

COPY group.mp4 /home/netlab/

ENV PATH=/home/netlab/code:$PATH
