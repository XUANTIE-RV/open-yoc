#! /bin/env python

import os
import sys

sys.path.append("./tools/yoc")
import toolchain

defconfig = toolchain.DefaultConfig()

Export('defconfig')

paths = [
    'tools',
    'csi',
    'boards',
    'components',
]

defconfig.build_package(paths)
