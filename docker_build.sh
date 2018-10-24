#!/bin/sh

tag="civctp2/civctp2:build"
echo "Building $tag"
docker build -t "$tag" . -f Dockerfile
