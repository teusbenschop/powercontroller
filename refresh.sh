#!/bin/bash
set -e
/opt/local/bin/rsync -av --exclude "cmake-build-debug" ../powercontroller/ '[2a02:a444:1de1:0:ae87:a3ff:fe38:a281]':powercontroller/
