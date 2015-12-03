#!/bin/bash

path=`pwd`
${path}/LoginServer/LoginServer -t 2 -d -f script/login.lua
#sleep 1
${path}/SuperServer/SuperServer -t 2 -d -f script/Super.lua
sleep 0.1
${path}/RecordServer/RecordServer -t 2 -d -f script/login.lua
sleep 0.1
${path}/SessionServer/SessionServer -t 2 -d -f script/login.lua
sleep 0.1
${path}/SceneServer/SceneServer -t 2 -d -f script/sceneserver.lua
sleep 0.1
${path}/SceneServer/SceneServer -t 2 -d -f script/sceneserver.lua
sleep 0.1
${path}/GatewayServer/GatewayServer -t 2 -d -f script/gate.lua
