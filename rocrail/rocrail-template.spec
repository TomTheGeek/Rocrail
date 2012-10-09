Summary: Model Railroad Control System
Name: rocrail
Vendor: rocrail.net
Version: <VERSION>
Release: <BZR>
Group: Amusements/Games
License: gpl
Packager: Rob Versluis <support@rocrail.net>
URL: http://www.rocrail.net/
Source: rocrail-%version-%release.tar.gz
%description
RocRail is a 2 tier application written for Linux and Windows in C/C++ based on the wxWidgets class library.


%prep
%setup

%build
cd Rocrail
# make fromtar DEBUG=
## make fromtar DEBUG=
# if you have more than 1 cpu kernel than use -j<num> to speedup compiling.
# My best prcatice is <num> = 1 + #kernels
MY_MAKEOPTS=`cat /proc/cpuinfo | grep processor | tail -1 | awk '{if(0 != $NF) {print " -j"2+$NF}}'`
make $MY_MAKEOPTS fromtar DEBUG=
cd ..

%install
#rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/opt/rocrail
mkdir -p $RPM_BUILD_ROOT/opt/rocrail/default
mkdir -p $RPM_BUILD_ROOT/opt/rocrail/svg
mkdir -p $RPM_BUILD_ROOT/opt/rocrail/stylesheets
mkdir -p $RPM_BUILD_ROOT/opt/rocrail/decspecs
mkdir -p $RPM_BUILD_ROOT/opt/rocrail/symbols
mkdir -p $RPM_BUILD_ROOT/usr/share/applications
mkdir -p $RPM_BUILD_ROOT/usr/share/desktop-directories
mkdir -p $RPM_BUILD_ROOT/etc/init.d

install -s -m 755 Rocrail/unxbin/rocrail $RPM_BUILD_ROOT/opt/rocrail/rocrail
install -s -m 755 Rocrail/unxbin/rocview $RPM_BUILD_ROOT/opt/rocrail/rocview
install -s -m 755 Rocrail/unxbin/*.so $RPM_BUILD_ROOT/opt/rocrail

install -g users -m 666 Rocrail/rocrail/package/Rocrail.directory $RPM_BUILD_ROOT/usr/share/desktop-directories
install -g users -m 666 Rocrail/rocrail/package/Roc*.desktop $RPM_BUILD_ROOT/usr/share/applications

install -m 755 Rocrail/rocrail/package/roc*.sh $RPM_BUILD_ROOT/opt/rocrail
install -g users -m 666 Rocrail/rocrail/package/rocraild.png $RPM_BUILD_ROOT/opt/rocrail
install -g users -m 666 Rocrail/rocrail/package/rocraild $RPM_BUILD_ROOT/etc/init.d
install -g users -m 666 Rocrail/rocrail/package/rocrail.xpm $RPM_BUILD_ROOT/opt/rocrail
install -g users -m 666 Rocrail/rocrail/package/plan.xml $RPM_BUILD_ROOT/opt/rocrail/default

install -g users -m 666 Rocrail/stylesheets/*.* $RPM_BUILD_ROOT/opt/rocrail/stylesheets
install -g users -m 666 Rocrail/decspecs/*.* $RPM_BUILD_ROOT/opt/rocrail/decspecs

### install does not handle directory (sub-)trees, so we have to do this manually
# install -d -g users -m 666 Rocrail/rocview/svg/* $RPM_BUILD_ROOT/opt/rocrail/svg
cp -pr Rocrail/rocview/svg $RPM_BUILD_ROOT/opt/rocrail/
chown -R :users  $RPM_BUILD_ROOT/opt/rocrail/svg
find $RPM_BUILD_ROOT/opt/rocrail/svg -type d -exec chmod 755 {} \;
find $RPM_BUILD_ROOT/opt/rocrail/svg -type f -exec chmod 666 {} \;

install -g users -m 666 Rocrail/rocrail/symbols/*.* $RPM_BUILD_ROOT/opt/rocrail/symbols

%clean
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc

/opt/rocrail/rocrail
/opt/rocrail/rocview
/opt/rocrail/*.so
/opt/rocrail/default/plan.xml
/opt/rocrail/rocrail.sh
/opt/rocrail/rocview.sh
/opt/rocrail/rocraild.png
/etc/init.d/rocraild
/opt/rocrail/rocrail.xpm
/opt/rocrail/stylesheets/*.*
/opt/rocrail/decspecs/*.*
/opt/rocrail/svg
/opt/rocrail/symbols/*.*
/usr/share/desktop-directories/Rocrail.directory
/usr/share/applications/Rocrail.desktop
/usr/share/applications/Rocview.desktop

%changelog
