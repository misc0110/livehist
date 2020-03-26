#!/bin/bash
name=livehist
if test `whoami` != "root"; then echo "You need to run this target using fakeroot: fakeroot -u make deb"; exit 1; fi
mkdir -pv deb/usr/bin
mkdir -pv deb/usr/share/doc/$name/
cp $name deb/usr/bin
strip deb/usr/bin/$name
mkdir -p deb/DEBIAN
sed "s/%VERSION%/$(cat VERSION)/" debian-control > deb/DEBIAN/control
echo "initial version" > deb/usr/share/doc/$name/changelog
echo "Copyright 2020, Michael Schwarz" > deb/usr/share/doc/$name/copyright
gzip -c -9 -n deb/usr/share/doc/$name/changelog > deb/usr/share/doc/$name/changelog.gz
rm deb/usr/share/doc/$name/changelog
chmod -R 0755 deb/usr
chmod 0644 deb/usr/share/doc/$name/copyright
chmod 0644 deb/usr/share/doc/$name/changelog.gz
chown -R root:root deb/
dpkg-deb --build deb
rm -rf deb
lintian deb.deb 
mkdir dist
mv deb.deb dist/${name}_$(cat VERSION)_amd64.deb
