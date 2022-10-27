#!/bin/bash

find . -iname "*.c" | xargs rm -rf {} \;
find . -iname "*.S" | xargs rm -rf {} \;


cp package.yaml.lib package.yaml
rm -rf package.yaml.lib package.yaml.src
