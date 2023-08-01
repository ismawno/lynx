#!/bin/bash

SOURCE=${BASH_SOURCE[0]}
while [ -L "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )
  SOURCE=$(readlink "$SOURCE")
  [[ $SOURCE != /* ]] && SOURCE=$DIR/$SOURCE # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )

mkdir -p "$DIR/../shaders/bin"

/usr/local/bin/glslc "$DIR/../shaders/shader2D.vert" -o "$DIR/../shaders/bin/shader2D.vert.spv"
/usr/local/bin/glslc "$DIR/../shaders/shader2D.frag" -o "$DIR/../shaders/bin/shader2D.frag.spv"

/usr/local/bin/glslc "$DIR/../shaders/shader3D.vert" -o "$DIR/../shaders/bin/shader3D.vert.spv"
/usr/local/bin/glslc "$DIR/../shaders/shader3D.frag" -o "$DIR/../shaders/bin/shader3D.frag.spv"