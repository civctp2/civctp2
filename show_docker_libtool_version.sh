#!/bin/sh

tag="civctp2/civctp2:show_docker_libtoolize_version"
echo "Building $tag"
docker build -t "$tag" . -f Dockerfile
