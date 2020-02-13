#!/bin/bash

echo "Creating dir to deploy ..."
mkdir "deploy"

mv release map2check

echo "Generating TAR.GZ file ..."
tar -czvf map2check-rc-$1.tar.gz map2check/
mv map2check-rc-$1.tar.gz deploy/

echo "Generating zip file ..."
7z a map2check-rc-$1.zip map2check/
mv map2check-rc-$1.zip deploy/
