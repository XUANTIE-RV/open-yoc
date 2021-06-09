from SCons.Tool import cc,ar,link
import importlib
assembler = importlib.import_module('SCons.Tool.as')

def generate(env):

    cc.generate(env)
    assembler.generate(env)
    ar.generate(env)
    link.generate(env)
    env['CC'] = 'arm-none-eabi-gcc'
    env['AS'] = 'arm-none-eabi-as'
    env['AR'] = 'arm-none-eabi-ar'
    env['OBJDUMP'] = 'arm-none-eabi-objdump'
    env['OBJCOPY'] = 'arm-none-eabi-objcopy'
    env['NM'] = 'arm-none-eabi-nm'
    env['READELF'] = 'arm-none-eabi-readelf'
    env['PROGSUFFIX'] = '.elf'    
    
def exists(env):
    return True