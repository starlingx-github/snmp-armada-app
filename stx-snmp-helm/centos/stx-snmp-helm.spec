# Application tunables (maps to metadata)
%global app_name snmp
%global helm_repo stx-platform

%global armada_folder /usr/lib/armada

# Install location
%global app_folder /usr/local/share/applications/helm

# Build variables
%global helm_folder /usr/lib/helm
%global toolkit_version 0.1.0

Summary: StarlingX SNMP Armada Helm Charts
Name: stx-snmp-helm
Version: 1.0
Release: %{tis_patch_ver}%{?_tis_dist}
License: Apache-2.0
Group: base
Packager: Wind River <info@windriver.com>
URL: unknown

Source0: helm-charts-snmp-0-1-0.tar.gz

BuildArch: noarch

BuildRequires: helm
BuildRequires: python-k8sapp-snmp
BuildRequires: python-k8sapp-snmp-wheels

%description
StarlingX SNMP Helm Charts

%package fluxcd
Summary: StarlingX SNMP FluxCD Helm Charts
Group: base
License: Apache-2.0

%description fluxcd
StarlingX SNMP FluxCD Helm Charts

%prep
%setup -n helm-charts-snmp-0-1-0-1.0.0

%build

cd helm-charts
make

# switch back to source root
cd -

# Create a chart tarball compliant with sysinv kube-app.py
%define app_staging %{_builddir}/staging
%define app_tarball %{app_name}-%{version}-%{tis_patch_ver}.tgz
%define app_tarball_fluxcd %{app_name}-fluxcd-%{version}-%{tis_patch_ver}.tgz

# Setup staging
mkdir -p %{app_staging}
cp files/metadata.yaml %{app_staging}
cp manifests/*.yaml %{app_staging}
mkdir -p %{app_staging}/charts
cp helm-charts/*.tgz %{app_staging}/charts

# Populate metadata
cd %{app_staging}
sed -i 's/@APP_NAME@/%{app_name}/g' %{app_staging}/metadata.yaml
sed -i 's/@APP_VERSION@/%{version}-%{tis_patch_ver}/g' %{app_staging}/metadata.yaml
sed -i 's/@HELM_REPO@/%{helm_repo}/g' %{app_staging}/metadata.yaml

# Copy the plugins: installed in the buildroot
mkdir -p %{app_staging}/plugins
cp /plugins/%{app_name}/*.whl %{app_staging}/plugins

# calculate checksum of all files in app_staging
find . -type f ! -name '*.md5' -print0 | xargs -0 md5sum > checksum.md5
# package it up
tar -zcf %{_builddir}/%{app_tarball} -C %{app_staging}/ .

# switch back to source root
cd -

# Prepare app_staging for fluxcd package
rm -f %{app_staging}/snmp_manifest.yaml

cp -R fluxcd-manifests %{app_staging}/

# calculate checksum of all files in app_staging
cd %{app_staging}
find . -type f ! -name '*.md5' -print0 | xargs -0 md5sum > checksum.md5
# package fluxcd app
tar -zcf %{_builddir}/%{app_tarball_fluxcd} -C %{app_staging}/ .

# switch back to source root
cd -

# Cleanup staging
rm -fr %{app_staging}

%install
install -d -m 755 %{buildroot}/%{app_folder}
install -p -D -m 755 %{_builddir}/%{app_tarball} %{buildroot}/%{app_folder}
install -p -D -m 755 %{_builddir}/%{app_tarball_fluxcd} %{buildroot}/%{app_folder}

%files
%defattr(-,root,root,-)
%{app_folder}/%{app_tarball}

%files fluxcd
%defattr(-,root,root,-)
%{app_folder}/%{app_tarball_fluxcd}
