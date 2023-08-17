#!/usr/bin/python3

import sys
import os

closed_src_comps = ['cx_lite_base', 'cx_lite_faceai', 'cx_lite_record', 'mpegts', 'tmedia_core', 'tmedia_backend_seno']
comps_path = '../../components'
root_path = os.getcwd()
publish_file_cmd = 'bash publish.sh' + ' '

for comp in closed_src_comps:
    print("generate %s component in sdk" % (comp))

    comp_path = os.path.join(comps_path, comp)
    os.chdir(comp_path)
    os.system(publish_file_cmd)
    os.chdir(root_path)
    