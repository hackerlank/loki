#!/bin/bash

./LoginServer/LoginServer -t 2 -d
#sleep 1
./SuperServer/SuperServer -t 2 -d
sleep 0.1
./RecordServer/RecordServer -t 2 -d
sleep 0.1
./SessionServer/SessionServer -t 2 -d
sleep 0.1
./SceneServer/SceneServer -t 2 -d
sleep 0.1
./SceneServer/SceneServer -t 2 -d
sleep 0.1
./GatewayServer/GatewayServer -t 2 -d
