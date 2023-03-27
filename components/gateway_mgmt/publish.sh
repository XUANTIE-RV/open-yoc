#!/bin/bash

find ./ -regex ".*\.c\|.*\.cpp\|.*\.cc" | xargs rm -rf

cp package.yaml.lib package.yaml
rm -rf package.yaml.lib package.yaml.src