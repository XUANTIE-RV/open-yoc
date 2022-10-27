#!/bin/bash

find . -iname "*.c" | xargs rm -rf {} \;
find . -iname "*.S" | xargs rm -rf {} \;
find melis/include/osal                                    -iname "*.h" | xargs rm -rf {} \;
find melis/include/hal  -path "melis/include/hal/sdmmc"                                   -iname "*.h" | xargs rm -rf {} \;
find melis/misc/video                                      -iname "*.h" | xargs rm -rf {} \;
find melis/misc/                                           -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source                                      -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/gpio                                 -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/mbus                                 -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/regulator                            -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/ccmu/sunxi-ng                        -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/ccmu/                                -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/rtc                                  -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/gpadc                                -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/lradc                                -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/dma                                  -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/pwm                                  -iname "*.h" | xargs rm -rf {} \;
find melis       -path "melis/include/hal/sdmmc"                                          -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/disp2/disp                           -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/disp2/disp/de                        -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/disp2/disp/de/lowlevel_v2x           -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/disp2/disp/lcd                       -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/disp2/soc                            -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/g2d_rcq                              -iname "*.h" | xargs rm -rf {} \;
find sys/d1                                                -iname "*.h" | xargs rm -rf {} \;
find sys                                                   -iname "*.h" | xargs rm -rf {} \;
find drivers/ll                                            -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/sound/platform/                      -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/sound/codecs/                        -iname "*.h" | xargs rm -rf {} \;
find melis/hal/source/sound/component/aw-alsa-utils/       -iname "*.h" | xargs rm -rf {} \;
# find melis/include/hal/sdmmc/hal                           -iname "*.h" | xargs rm -rf {} \;
# find melis/include/hal/sdmmc/sys                           -iname "*.h" | xargs rm -rf {} \;
# find melis/include/hal/sdmmc/osal/aos                      -iname "*.h" | xargs rm -rf {} \;


cp package.yaml.lib package.yaml
rm -rf package.yaml.lib
