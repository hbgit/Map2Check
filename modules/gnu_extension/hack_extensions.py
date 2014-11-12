#!/usr/bin/env python
# -*- coding: latin1 -*-

import sys
import re

_super_hack = '''typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;
    int __kind;
    unsigned int __nusers;
    __extension__ union
    {
      int __spins;
      __pthread_slist_t __list;
    };
  } __data;
  char __size[24];
  long int __align;
} pthread_mutex_t;'''

## data = output of C pre processor ##
def make_pycparser_compatible( data ):

    # kill white space #
    _d = ''
    for line in data.splitlines():
        #if line.strip():
        _d += line + '\n'
    #f = open('/tmp/xxx.c','wb')
    #f.write(_d); f.close()
    if _super_hack in _d:
        #print("Here")
        _d = _d.replace( _super_hack, 'typedef union pthread_mutexattr_t;\n' )
    #else:
    #    print("Here")
        #raise SystemError
    #    sys.exit()


    data = _d
    d = ''
    # TYPEDEF_HACKS = [
    #     ('signed', '__signed'),
    #     ('signed', '__signed__'),
    #     ('char *', '__builtin_va_list'),
    #     #('const', '__const'),
    #     ('const', '__const__'),
    #     #('restrict', '__restrict'),
    # ]
    #for type, name in TYPEDEF_HACKS: d += 'typedef %s %s;\n' %(type,name)

    for num, line in enumerate(data.splitlines()):
        # TODO: Improve this match by use regular expressions

        if '((__malloc__));' in line.split(): line = line.replace('((__malloc__));', ';')   # stdio.h:225
        if '((__malloc__))' in line.split(): line = line.replace('((__malloc__))', '')

        #__const
        # match_ext = re.search(r"(__extension__)* extern", line)
        # if match_ext or line.startswith('extern'):
        match_const = re.search(r"__const[^_]", line)
        if match_const:
            line = line.replace('__const', 'const')

            #print(line)
            #sys.exit()

        #print(">>>>>> "+ line)



        if '__attribute__((visibility("default")))' in line.split():
            line = line.replace('__attribute__((visibility("default")))', '')

        #
        if '__attribute__((noinline))' in line.split():  line = line.replace('__attribute__((noinline))', '')
        if '__attribute__((noinline));' in line.split():  line = line.replace('__attribute__((noinline));', ';')

        #
        if '__attribute__((packed))' in line.split(): line = line.replace('__attribute__((packed))', '')
        if '__attribute__((packed));' in line.split(): line = line.replace('__attribute__((packed));', ';')


        # __WAIT_STATUS
        if '__WAIT_STATUS' in line.split(): line = line.replace('__WAIT_STATUS', '')
        if '((__transparent_union__))' in line.split(): line = line.replace('((__transparent_union__))', '')
        if '((__transparent_union__));' in line.split(): line = line.replace('((__transparent_union__));', ';')
        if '((__transparent_union__)) ;' in line.split(): line = line.replace('((__transparent_union__)) ;', ';')

        #
        if '((__pure__))' in line.split(): line = line.replace('((__pure__))', '')
        if '((__pure__)) ;' in line.split(): line = line.replace('((__pure__)) ;', ';')

        #
        if '((__warn_unused_result__))' in line.split(): line = line.replace('((__warn_unused_result__))', '')
        if '((__warn_unused_result__)) ;' in line.split(): line = line.replace('((__warn_unused_result__)) ;', ';')
        if '((__warn_unused_result__));' in line.split(): line = line.replace('((__warn_unused_result__));', ';')

        # __nothrow__ and __leaf__
        match_nl = re.search(r"\(\(__nothrow__[ ]*,[ ]*__leaf__\)\)", line)
        if match_nl:
            #line = line.replace('((__nothrow__ , __leaf__))', '')
            line = re.sub(r"\(\(__nothrow__[ ]*,[ ]*__leaf__\)\)", "", line)

        #
        match_nnull = re.search(r"\(\(__nonnull__[ ]*[\(]([0-9]*,[ ]*)*([0-9]*)[\)]\)\)", line)
        if match_nnull:
            line = re.sub(r"\(\(__nonnull__[ ]*[\(]([0-9]*,[ ]*)*([0-9]*)[\)]\)\)", "", line)
            #line = line.replace('((__nonnull__ (1)))', '')



        if '__extension__' in line.split(): line = line.replace('__extension__','')
        if '__attribute__' in line.split(): line = line.replace('__attribute__', '')
        if '__THROW' in line.split(): line = line.replace('__THROW', '')

        if '__inline__' in line.split(): line = line.replace( '__inline__', '' )

        if '((__nothrow__))' in line.split(): line = line.replace('((__nothrow__))', '')    # inttypes.h
        if '((__nothrow__));' in line.split(): line = line.replace('((__nothrow__));', ';')

        if '((__const__))' in line.split(): line = line.replace('((__const__))', '')
        if '((__const__));' in line.split(): line = line.replace('((__const__));', ';')

        if '**__restrict' in line.split(): line = line.replace('**__restrict', '**')
        if '*__restrict' in line.split(): line = line.replace('*__restrict', '*' )
        if '__restrict' in line.split(): line = line.replace('__restrict', '' )

        if line.strip().startswith('((__format__ ('):       # stdio.h:385
            _s = line.strip()
            if _s.endswith( '))) ;' ) or _s.endswith(')));'): line = ';'

        if line.startswith('extern') and '((visibility("default")))' in line.split():       # SDL_cdrom.h
            line = line.replace( '((visibility("default")))', '' )


        if line.startswith('extern'):
            if '((__noreturn__))'  in line.split():
                line = line.replace( '((__noreturn__))', '' )
            if '((__noreturn__));' in line.split():
                line = line.replace( '((__noreturn__));', ';' )

        # Replace __VERIFIER_error() by out implementation
        if line.startswith('extern') and '__VERIFIER_error()' in line.split():       # SVCOMP
            line = line.replace( '__VERIFIER_error()', '__VERIFIER_error(int numline)' )
        if line.startswith('extern') and '__VERIFIER_error(void);' in line.split():       # SVCOMP
            line = line.replace( '__VERIFIER_error(void);', '__VERIFIER_error(int numline);' )


        # types.h #
        list_mode = ['((__mode__ (__QI__)));' , '((__mode__(__QI__)));' ,
                     '((__mode__ (__HI__)));' , '((__mode__(__HI__)));' ,
                     '((__mode__ (__SI__)));' , '((__mode__(__SI__)));' ,
                     '((__mode__ (__DI__)));' , '((__mode__(__DI__)));' ,
                     '((__mode__ (__word__)));' , '((__mode__(__word__)));']
        for ugly in list_mode:
            if line.endswith( ugly ): line = line.replace(ugly, ';')

        # if '__asm__' in line.split() and '__isoc99_' in line:
        #     if line.strip().endswith(';'): line = line.split('__asm__')[0] + ';'
        #     else: line = line.split('__asm__')[0]

        #
        match_asm = re.search(r"__asm__[ ]*\(\"\" \"__xpg_strerror_r\"\)", line)
        if match_asm:
            #line = line.replace('((__nothrow__ , __leaf__))', '')
            line = re.sub(r"__asm__[ ]*\(\"\" \"__xpg_strerror_r\"\)", "", line)

        #if '*__const' in line.split(): # sys_errlist.h
        #   pass

        d += line + '\n'

    return d


# -------------------------------------------------
# Main python program
# -------------------------------------------------

if __name__ == "__main__":
    #print(sys.argv[1])
    file = open(sys.argv[1], 'r')
    print(make_pycparser_compatible(file.read()))
    file.close()
