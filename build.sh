#!/bin/sh
IMAGEID="noughts_and_crosses_build"
docker build -t=$IMAGEID .
container_id=$(docker create $IMAGEID)
docker cp $container_id:project/project.nds dist/.
docker rm -v $container_id
