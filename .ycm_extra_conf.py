import sys
import os
import ycm_core

# These are the compilation flags that will be used in case there's no
# compilation database set.
flags = [
    '-Wall',
    '-std=c++11',
    '-x',
    'c++',
    '-I', '.',
    '-isystem', '/usr/include/c++/4.9',
    '-isystem', '/usr/lib/c++/4.8',
    '-isystem', '/usr/include',
    '-isystem', '/usr/local/include',
    '-isystem', '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../include/c++/v1',
    '-isystem', '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include'
] + [
    '-I', './src',
    '-isystem', '/usr/local/include/SDL2'
]

if sys.platform.startswith('darwin'):
    flags += ['-isystem', '-stdlib=libc++']

def DirectoryOfThisScript():
    return os.path.dirname(os.path.abspath(__file__))

def MakeRelativePathsInFlagsAbsolute(flags, working_directory):
    if not working_directory:
        return flags
    new_flags = []
    make_next_absolute = False
    path_flags = ['-isystem', '-I', '-iquote', '--sysroot=']
    for flag in flags:
        new_flag = flag

        if make_next_absolute:
            make_next_absolute = False
            if not flag.startswith('/'):
                new_flag = os.path.join(working_directory, flag)

        for path_flag in path_flags:
            if flag == path_flag:
                make_next_absolute = True
                break

            if flag.startswith(path_flag):
                path = flag[len(path_flag):]
                new_flag = path_flag + os.path.join(working_directory, path)
                break

        if new_flag:
            new_flags.append(new_flag)
    return new_flags


def FlagsForFile(filename):
    relative_to = DirectoryOfThisScript()
    final_flags = MakeRelativePathsInFlagsAbsolute(flags, relative_to)

    return {
        'flags': final_flags,
        'do_cache': True
    }
