##  Copyright 2006-2024 Carnegie Mellon University
##  See license information in LICENSE.txt.

##  libfixbuf.spec: Generated from libfixbuf.spec.in by make.

##  @DISTRIBUTION_STATEMENT_BEGIN@
##  libfixbuf 2.5
##
##  Copyright 2024 Carnegie Mellon University.
##
##  NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
##  INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
##  UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
##  IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
##  FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
##  OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT
##  MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
##  TRADEMARK, OR COPYRIGHT INFRINGEMENT.
##
##  Licensed under a GNU-Lesser GPL 3.0-style license, please see
##  LICENSE.txt or contact permission@sei.cmu.edu for full terms.
##
##  [DISTRIBUTION STATEMENT A] This material has been approved for public
##  release and unlimited distribution.  Please see Copyright notice for
##  non-US Government use and distribution.
##
##  This Software includes and/or makes use of Third-Party Software each
##  subject to its own license.
##
##  DM24-1020
##  @DISTRIBUTION_STATEMENT_END@

#   The following --with X and --without X options are supported with
#   the default shown in brackets:
#
#   openssl [without]: enables use of TLS for sending/receiving
#   sctp [without]: enables use of SCTP (stream control transmission
#                   protocol) RFC9260, IP PROTO 132
#   spread [without]: enables use of the Spread Toolkit messaging
#                     service (www.spread.org)

%bcond openssl  0
%bcond sctp     0
%bcond spread   0

%define name libfixbuf
%define version 2.5.0
%define release 1%{?with_openssl:_openssl}%{?with_sctp:_sctp}%{?with_spread:_spread}%{?dist}

Summary:        Fixbuf IPFIX implementation library
Name:           %{name}
Version:        %{version}
Release:        %{release}
Group:          Applications/System
License:        LGPLv3
Source:         https://tools.netsa.cert.org/releases/%{name}-%{version}.tar.gz
URL:            https://tools.netsa.cert.org/fixbuf2/
Provides:       %{name} = %{version}
BuildRequires:  gcc
Requires:       glib2 >= 2.34.0
BuildRequires:  glib2-devel >= 2.34.0
BuildRequires:  pkgconfig >= 0.16
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
%if %{with openssl}
Requires:       openssl-libs >= 1.0.2
BuildRequires:  openssl-devel >= 1.0.2
%endif
%if %{with sctp}
Requires:       lksctp-tools
BuildRequires:  lksctp-tools-devel
%endif
%if %{with spread}
Requires:       libspread
BuildRequires:  libspread-devel
%endif
BuildRoot:      %{_tmppath}/%{name}-%{version}

%description
libfixbuf aims to be a compliant implementation of the IPFIX Protocol
and message format, from which IPFIX Collecting Processes and
IPFIX Exporting Processes may be built.

%package devel
Summary:        Unversioned shared libraries and C header files for libfixbuf
Group:          Applications/System
Provides:       libfixbuf-devel = %{version}
Requires:       %{name} = %{version}
Requires:       glib2-devel >= 2.34.0
Requires:       pkgconfig >= 0.16
%if %{with openssl}
Requires:       openssl-devel >= 1.0.2
%endif
%if %{with sctp}
Requires:       lksctp-tools-devel
%endif
%if %{with spread}
Requires:       libspread-devel
%endif

%description devel
Unversioned shared libraries and C header files for libfixbuf. Also includes
HTML documentation of the library API calls.

%prep
%setup -q -n %{name}-%{version}

%build
%configure \
    --disable-doxygen-doc \
    --with-openssl=%{?with_openssl:yes}%{!?with_openssl:no} \
    --with-sctp=%{?with_sctp:yes}%{!?with_sctp:no} \
    --with-spread=%{?with_spread:yes}%{!?with_spread:no}
%{__make}

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{_bindir}
%make_install
rm -f $RPM_BUILD_ROOT/%{_libdir}/*.la

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS LICENSE.txt NEWS README
%{_libdir}/*.a
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root)
%doc doc/html
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/pkgconfig/*

%changelog
