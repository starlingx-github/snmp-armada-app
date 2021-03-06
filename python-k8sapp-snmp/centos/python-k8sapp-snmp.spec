%global app_name snmp
%global pypi_name k8sapp-snmp
%global sname k8sapp_snmp

Name:           python-%{pypi_name}
Version:        1.0
Release:        %{tis_patch_ver}%{?_tis_dist}
Summary:        StarlingX sysinv extensions: Snmp

License:        Apache-2.0
Source0:        %{name}-%{version}.tar.gz

BuildArch:      noarch

BuildRequires: python-setuptools
BuildRequires: python-pbr
BuildRequires: python2-pip
BuildRequires: python2-wheel

%description
StarlingX sysinv extensions: SNMP K8S app

%prep
%setup
# Remove bundled egg-info
rm -rf %{pypi_name}.egg-info

%build
export PBR_VERSION=%{version}
%{__python2} setup.py build

%py2_build_wheel

%install
export PBR_VERSION=%{version}.%{tis_patch_ver}
export SKIP_PIP_INSTALL=1
%{__python2} setup.py install --skip-build --root %{buildroot}
mkdir -p ${RPM_BUILD_ROOT}/plugins/%{app_name}
install -m 644 dist/*.whl ${RPM_BUILD_ROOT}/plugins/%{app_name}/

%files
%{python2_sitelib}/%{sname}
%{python2_sitelib}/%{sname}-*.egg-info

%package wheels
Summary: %{name} wheels

%description wheels
Contains python wheels for %{name}

%files wheels
/plugins/*
