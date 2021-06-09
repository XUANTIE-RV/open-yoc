from SCons.Tool import cc,ar,link
import importlib
assembler = importlib.import_module('SCons.Tool.as')

def generate(env):
    cc.generate(env)
    assembler.generate(env)
    ar.generate(env)
    link.generate(env)
    env['CC'] = 'riscv-none-embed-gcc'
    env['AS'] = 'riscv-none-embed-as'
    env['AR'] = 'riscv-none-embed-ar'
    env['OBJDUMP'] = 'riscv-none-embed-objdump'
    env['OBJCOPY'] = 'riscv-none-embed-objcopy'
    env['NM'] = 'riscv-none-embed-nm'
    env['READELF'] = 'riscv-none-embed-readelf'
    env['PROGSUFFIX'] = '.elf'    
    
def exists(env):
    return True