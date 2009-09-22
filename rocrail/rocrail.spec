Summary: Model Railroad Control System
Name: rocrail
Vendor: rocrail.net
Version: 1.2
Release: 0
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
make all DEBUG=
cd ..

%install
#rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/opt/rocrail
mkdir -p $RPM_BUILD_ROOT/opt/rocrail/default
mkdir -p $RPM_BUILD_ROOT/opt/rocrail/icons
mkdir -p $RPM_BUILD_ROOT/opt/rocrail/svg
mkdir -p $RPM_BUILD_ROOT/opt/rocrail/stylesheets
mkdir -p $RPM_BUILD_ROOT/opt/rocrail/symbols
mkdir -p $RPM_BUILD_ROOT/usr/share/applications
mkdir -p $RPM_BUILD_ROOT/usr/share/desktop-directories
mkdir -p $RPM_BUILD_ROOT/etc/init.d

install -s -m 755 Rocrail/unxbin/rocrail $RPM_BUILD_ROOT/opt/rocrail/rocrail
install -s -m 755 Rocrail/unxbin/lcdriver.so $RPM_BUILD_ROOT/opt/rocrail/lcdriver.so
install -s -m 755 Rocrail/unxbin/barjut.so $RPM_BUILD_ROOT/opt/rocrail/barjut.so
install -s -m 755 Rocrail/unxbin/hsi88.so $RPM_BUILD_ROOT/opt/rocrail/hsi88.so
install -s -m 755 Rocrail/unxbin/p50.so $RPM_BUILD_ROOT/opt/rocrail/p50.so
install -s -m 755 Rocrail/unxbin/p50x.so $RPM_BUILD_ROOT/opt/rocrail/p50x.so
install -s -m 755 Rocrail/unxbin/srcp.so $RPM_BUILD_ROOT/opt/rocrail/srcp.so
install -s -m 755 Rocrail/unxbin/dinamo.so $RPM_BUILD_ROOT/opt/rocrail/dinamo.so
install -s -m 755 Rocrail/unxbin/om32.so $RPM_BUILD_ROOT/opt/rocrail/om32.so
install -s -m 755 Rocrail/unxbin/lenz.so $RPM_BUILD_ROOT/opt/rocrail/lenz.so
install -s -m 755 Rocrail/unxbin/roco.so $RPM_BUILD_ROOT/opt/rocrail/roco.so
install -s -m 755 Rocrail/unxbin/zimo.so $RPM_BUILD_ROOT/opt/rocrail/zimo.so
install -s -m 755 Rocrail/unxbin/ddx.so $RPM_BUILD_ROOT/opt/rocrail/ddx.so
install -s -m 755 Rocrail/unxbin/slx.so $RPM_BUILD_ROOT/opt/rocrail/slx.so
install -s -m 755 Rocrail/unxbin/loconet.so $RPM_BUILD_ROOT/opt/rocrail/loconet.so
install -s -m 755 Rocrail/unxbin/opendcc.so $RPM_BUILD_ROOT/opt/rocrail/opendcc.so
install -s -m 755 Rocrail/unxbin/rocview $RPM_BUILD_ROOT/opt/rocrail/rocview
install -s -m 755 Rocrail/unxbin/virtual.so $RPM_BUILD_ROOT/opt/rocrail/virtual.so

install -g users -m 666 Rocrail/rocrail/package/Rocrail.directory $RPM_BUILD_ROOT/usr/share/desktop-directories
install -g users -m 666 Rocrail/rocrail/package/Roc*.desktop $RPM_BUILD_ROOT/usr/share/applications

install -m 755 Rocrail/rocrail/package/roc*.sh $RPM_BUILD_ROOT/opt/rocrail
install -g users -m 666 Rocrail/rocrail/package/rocraild.png $RPM_BUILD_ROOT/opt/rocrail
install -g users -m 666 Rocrail/rocrail/package/rocraild $RPM_BUILD_ROOT/etc/init.d
install -g users -m 666 Rocrail/rocrail/package/rocrail.xpm $RPM_BUILD_ROOT/opt/rocrail
install -g users -m 666 Rocrail/rocrail/package/roc*.ini $RPM_BUILD_ROOT/opt/rocrail/default
install -g users -m 666 Rocrail/rocrail/package/plan.xml $RPM_BUILD_ROOT/opt/rocrail/default
install -g users -m 666 Rocrail/rocrail/package/neustadt.xml $RPM_BUILD_ROOT/opt/rocrail/default

install -g users -m 666 Rocrail/rocview/icons/*.* $RPM_BUILD_ROOT/opt/rocrail/icons
install -g users -m 666 Rocrail/stylesheets/*.* $RPM_BUILD_ROOT/opt/rocrail/stylesheets
install -d -g users -m 666 Rocrail/rocview/svg/* $RPM_BUILD_ROOT/opt/rocrail/svg
install -g users -m 666 Rocrail/rocrail/symbols/*.* $RPM_BUILD_ROOT/opt/rocrail/symbols

%clean
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc

/opt/rocrail/rocrail
/opt/rocrail/lcdriver.so
/opt/rocrail/hsi88.so
/opt/rocrail/p50.so
/opt/rocrail/p50x.so
/opt/rocrail/srcp.so
/opt/rocrail/dinamo.so
/opt/rocrail/om32.so
/opt/rocrail/zimo.so
/opt/rocrail/lenz.so
/opt/rocrail/roco.so
/opt/rocrail/ddx.so
/opt/rocrail/virtual.so
/opt/rocrail/slx.so
/opt/rocrail/barjut.so
/opt/rocrail/loconet.so
/opt/rocrail/opendcc.so
/opt/rocrail/ecos.so
/opt/rocrail/cs2.so
/opt/rocrail/nec.so
/opt/rocrail/rocview
/opt/rocrail/default/plan.xml
/opt/rocrail/default/neustadt.xml
/opt/rocrail/default/rocrail.ini
/opt/rocrail/default/rocview.ini
/opt/rocrail/rocrail.sh
/opt/rocrail/rocview.sh
/opt/rocrail/rocraild.png
/etc/init.d/rocraild
/opt/rocrail/rocrail.xpm
/opt/rocrail/icons/*.*
/opt/rocrail/stylesheets/*.*
/opt/rocrail/svg
/opt/rocrail/symbols/*.*
/usr/share/desktop-directories/Rocrail.directory
/usr/share/applications/Rocrail.desktop
/usr/share/applications/RocrailGUI.desktop

%changelog
