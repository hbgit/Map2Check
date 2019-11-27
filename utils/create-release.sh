#!/bin/bash

echo "Creating deplot dir ..."
mkdir "deploy"

echo "Generating TAR.GZ file ..."
tar -czvf map2check-rc-$1.tar.gz release/
mv map2check-rc-$1.tar.gz deploy/

echo "Generating zip file ..."
7z a map2check-rc-$1.zip release/
mv map2check-rc-$1.zip deploy/
