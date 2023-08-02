FROM centos:7.9.2009

ARG DEVSHELL="scl enable devtoolset-11 -- env CC=/opt/rh/devtoolset-11/root/bin/gcc CXX=/opt/rh/devtoolset-11/root/bin/g++"

RUN yum -y install \
    # General Tools
    git wget make bzip2 openssl openssl-devel python-devel python36-setuptools python36-devel perl-Data-Dumper \
    # C & C++ Compiler
    scl-utils centos-release-scl && \
    yum install -y devtoolset-11-gcc devtoolset-11-gcc-c++ gdb && \
    # Reduce image size
    yum -y clean all && rm -rf /var/cache

RUN git clone https://github.com/mdadams/sde.git

# Skip dir check
RUN sed -i '365,367 s/^/#/' sde/installer

# Monkey patch wget
RUN find sde -type f -exec sed -i 's/wget /wget --no-check-certificate /g' {} \;

RUN sed -i '830s/.*/if [[ $gccexit -eq 1 ]]; then exit 0; fi/' sde/installer

# Cache CMake + GCC
RUN $DEVSHELL gccexit=1 sde/installer -d /opt/sde

RUN sed -i \
    # Skip ghi gh://stephencelis/ghi
    -e '803,811 s/^/#/' \
    # Skip GCC Alternative
    -e '835,854 s/^/#/' \
    # Skip Clang
    -e '860,874 s/^/#/' \
    # Skip Clang Alternative
    -e '883,905 s/^/#/' \
    # Skip GDB (ISL)
    -e '999,1013 s/^/#/' \
    # Skip LCOV (CGAL)
    -e '1019,1027 s/^/#/' \
    # Skip Texlive
    -e '1033,1058 s/^/#/' \
    # Skip Jasper
    -e '1066,1084 s/^/#/' \
    # Skip SPL (CGAL)
    -e '1090,1117 s/^/#/' \
    # Skip Aristotle
    -e '1165,1176 s/^/#/' \
    # Skip Vim
    -e '1182,1191 s/^/#/' \
    # Skip Vim LSP
    -e '1197,1208 s/^/#/' \
    # Skip YCM
    -e '1214,1239 s/^/#/' \
    # Skip GCCGO
    -e '1245,1256 s/^/#/' \
    # Skip HUB
    -e '1262,1270 s/^/#/' \
    # Skip GH CLI
    -e '1276,1284 s/^/#/' \
    sde/installer

RUN sed -i '90s/.*/boost_version=1.81.0/' sde/installer

# Setup defualt env
RUN mkdir -p /opt/sde/etc/ && echo "base" > /opt/sde/etc/default_environment

# Finish setup
RUN $DEVSHELL sde/installer -d /opt/sde

RUN rm /opt/sde/bin/gdb
RUN echo "/opt/sde/bin/sde_shell; clear" >> ~/.bashrc

RUN yum -y remove git git* && \
    yum -y install https://packages.endpointdev.com/rhel/7/os/x86_64/endpoint-repo.x86_64.rpm && \
    yum -y install git && \
    # Reduce image size
    yum -y clean all && rm -rf /var/cache

RUN yum install -y libGL-devel libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel && \
    git clone https://github.com/glfw/glfw.git && cd glfw && git checkout 3.2.1 && \
    mkdir build && cd build && \
    $DEVSHELL /opt/sde/packages/cmake-3.26.3/bin/cmake .. && $DEVSHELL make && $DEVSHELL make install

RUN mkdir /dev/input

CMD ["sleep", "infinity"] 
