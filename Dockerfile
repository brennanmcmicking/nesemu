FROM centos:7.9.2009

ARG DEVSHELL="scl enable devtoolset-8 -- env CC=/opt/rh/devtoolset-8/root/bin/gcc CXX=/opt/rh/devtoolset-8/root/bin/g++"

RUN yum -y install \
    # General Tools
    git wget make bzip2 openssl openssl-devel python36-setuptools python36-devel perl-Data-Dumper \
    # C & C++ Compiler
    scl-utils centos-release-scl && \
    yum install -y devtoolset-8-gcc devtoolset-8-gcc-c++

RUN git clone https://github.com/mdadams/sde.git

# Skip dir check
RUN sed -i '365,367 s/^/#/' sde/installer

# Monkey patch wget
RUN find sde -type f -exec sed -i 's/wget/wget --no-check-certificate/g' {} \;

RUN sed -i '830s/.*/if [[ $exitearly -eq 1 ]]; then exit 0; fi/' sde/installer

# Cache CMake + GCC
RUN $DEVSHELL exitearly=1 sde/installer -d /opt/sde

RUN yum update -y && yum -y install python-devel freeglut

RUN sed -i '90s/.*/boost_version=1.81.0/' sde/installer

RUN sed -i \
    # Skip ninja build system
    -e '789,797 s/^/#/' \
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

# Setup defualt env
RUN mkdir -p /opt/sde/etc/ && echo "base" > /opt/sde/etc/default_environment

# Finish setup
RUN $DEVSHELL sde/installer -d /opt/sde

CMD ["sleep", "infinity"] 
