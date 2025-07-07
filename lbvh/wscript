import glob
import os
import platform
from waflib.TaskGen import extension, feature, task_gen
from waflib.Task import Task
from waflib import Build
from shutil import copyfile

VERSION = '0.0'
APPNAME = 'lbvhtool'

top = '.'


def options(opt):
    pass

def configure(conf):
    if ('COMSPEC' in os.environ):
        conf.env.MSVC_TARGETS = ['x64']
        conf.load('msvc')
        conf.env.PLATFORM = ['windows']
    else:
        conf.load('clang++')

def build(bld):

    bld.post_mode = Build.POST_LAZY

    if ('COMSPEC' in os.environ):
        winlibroot = '%ProgramFiles%Windows Kits/10/Lib//10.0.19041.0/'
        winincroot = '%ProgramFiles%Windows Kits/10/Include/10.0.19041.0/'
        win_sdk_lib_path = os.path.expandvars(winlibroot+'um/x64/')
        win_sdk_include_path = os.path.expandvars(winincroot+'um/x64/')
        win_sdk_include_path_shared = os.path.expandvars(winincroot+'shared')

        platform_defines = ['PLATFORM_WINDOWS', '_CRT_SECURE_NO_WARNINGS']
        includes = ['source', 'includes', 'SDL2/include', win_sdk_include_path, win_sdk_include_path_shared]
        sdk_lib_path = [win_sdk_lib_path, os.path.abspath('SDL2/lib/x64')]
        #RELEASE
        compile_flags = ['/permissive-', '/arch:AVX', '/GL', '/WX', '/Ox', '/Ot', '/Oy', '/fp:fast', '/Qfast_transcendentals', '/Zi', '/EHsc', '/FS', '/D_SECURE_SCL 0', '/Fdlbvhtool']
        linker_flags = ['/SUBSYSTEM:CONSOLE', '/LTCG', '/RELEASE']
        #DEBUG
        #compile_flags = ['/permissive-', '/arch:AVX', '/WX', '/Od', '/DDEBUG', '/Qfast_transcendentals', '/Zi', '/GS', '/EHsc', '/FS']
        #linker_flags = ['/SUBSYSTEM:CONSOLE', '/LTCG', '/DEBUG']
        libs = ['ws2_32', 'shell32', 'user32', 'Comdlg32', 'gdi32', 'ole32', 'kernel32', 'winmm', 'SDL2main', 'SDL2']
        sdlsourcepath = os.path.abspath('./SDL2/lib/x64/SDL2.dll')
        sdltargetpath = os.path.abspath('./build/release/SDL2.dll')
        print (sdlsourcepath + " -> " + sdltargetpath)
        copyfile(sdlsourcepath, sdltargetpath)
    else:
        platform_defines = ['PLATFORM_LINUX', '_CRT_SECURE_NO_WARNINGS']
        includes = ['source', 'includes', '/usr/include/SDL2']
        sdk_lib_path = []
        compile_flags = ['-Ofast', '-ffast-math', '-lclang', '-std=c++17', '-msse4.1']
        #compile_flags = ['-O0', '-ffast-math', '-lclang', '-std=c++17', '-g', '-msse4.1'] # -ggdb
        linker_flags = []
        libs = ['SDL2']

    # Build risctool
    bld.program(
        source=glob.glob('*.cpp'),
        cxxflags=compile_flags,
        ldflags=linker_flags,
        target='lbvhtool',
        defines=platform_defines,
        includes=includes,
        libpath=sdk_lib_path,
        lib=libs )
