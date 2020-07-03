#!/usr/bin/env python2.7
'''
    CSeq 1.9 wrapper script for Lazy Configuration, SV-COMP 2020

    previous versions:
        CSeq 1.6 wrapper script for Lazy Configuration, SV-COMP 2019
        Lazy-CSeq 1.1 SV-COMP 2017 wrapper script

        Lazy-CSeq 1.1 SV-COMP 2016 wrapper script

        October 2015  SV-COMP 2016 VERSION
        July 2015  adapted to CSeq-1.0 new command-line format
        October 2014  SV-COMP 2015 VERSION
        November 2013  original version
'''
VERSION = "CSeq-1.9--lazy +SV-COMP2020"

import sys, getopt, os.path, shlex, subprocess, time, resource
import core.utils as utils
import re

svcompparams = []
'''
lazy-cseq 1.0 SVCOMP2017
svcompparams.append('--time=120  --svcomp --stop-on-fail --depth=1400  --unwind-for=1  --unwind-while=1 --unwind-for-max=50 --softunwindbound --rounds=2 -l lazy --atomicparameter')
svcompparams.append('--time=90 --svcomp --stop-on-fail --depth=350 --unwind-for=2  --unwind-while=2 --rounds=2 -l lazy --atomicparameter')
svcompparams.append('--time=60  --svcomp --stop-on-fail --depth=250 --unwind-for=16 --unwind-while=3 --rounds=1 -l lazy --atomicparameter')
svcompparams.append('--time=90  --svcomp --stop-on-fail --depth=500 --softunwindbound --unwind-while=1 --unwind-for-max=11 --rounds=11 --norobin -l lazy --atomicparameter')
svcompparams.append('--time=240 --svcomp --stop-on-fail --depth=560 --softunwindbound --unwind-while=1 --unwind-for=2 --unwind-for-max=3 --rounds 4 -l lazy --atomicparameter')

lazy-cseq-abs SVCOMP2017
svcompparams.append('--time=120  --svcomp --stop-on-fail --depth=1400  --unwind-for=1  --unwind-while=1 --unwind-for-max=50 --softunwindbound --rounds=2 -l lazy+ai')
svcompparams.append('--time=70 --svcomp --stop-on-fail --depth=350 --unwind-for=2  --unwind-while=2 --rounds=2 -l lazy+ai')
svcompparams.append('--time=60  --svcomp --stop-on-fail --depth=250 --unwind-for=16 --unwind-while=3 --rounds=1 -l lazy+ai')
svcompparams.append('--time=70  --svcomp --stop-on-fail --depth=500 --softunwindbound --unwind-while=1 --unwind-for-max=11 --rounds=11 --norobin -l lazy+ai')
svcompparams.append('--time=200 --svcomp --stop-on-fail --depth=560 --softunwindbound --unwind-while=1 --unwind-for=2 --unwind-for-max=3 --rounds 4 -l lazy+ai')
svcompparams.append('--time=200 --svcomp --depth=1000 --unwind=1 --rounds=2 -l ase --path-backend=./core/cde-package/cbmc-5.4')
'''

'''
svcompparams.append('--sv-comp -X -l lazy    --rounds=2  --time=1 --depth=1400 --atomicparameters --svcomp --softunwindbound --unwind-while=1 --unwind-for=1 --unwind-for-max=50')
svcompparams.append('--sv-comp    -l lazy    --rounds=2  --time=1  --depth=350  --atomicparameters --svcomp                   --unwind-while=2 --unwind-for=2')
svcompparams.append('--sv-comp    -l lazy    --rounds=1  --time=1  --depth=250  --atomicparameters --svcomp                   --unwind-while=3 --unwind-for=16')
svcompparams.append('--sv-comp    -l lazy    --rounds=11 --time=1  --depth=500  --atomicparameters --svcomp --softunwindbound --unwind-while=1 --unwind-for-max=11 --norobin')
svcompparams.append('--sv-comp    -l lazy    --rounds 4  --time=1 --depth=560  --atomicparameters --svcomp --softunwindbound --unwind-while=1 --unwind-for=2 --unwind-for-max=3')
'''

# due to condvar
#####svcompparams.append('--cex --sv-comp   -l lazy    --rounds=2  --time=150 --depth=1400 --well-nested-locks --softunwindbound --unwind-while=1 --unwind-for=1 --unwind-for-max=50')
svcompparams.append('--cex --deep-propagation --extra-tracking --sv-comp   -l lazy    --rounds=2  --time=200 --depth=1400 --nondet-condvar-wakeups --softunwindbound --unwind-while=1 --unwind-for=1 --unwind-for-max=100')

####svcompparams.append('--cex --sv-comp    -l lazy    --rounds=2  --time=90  --depth=350  --atomic-parameters --svcomp                   --unwind-while=2 --unwind-for=2')
##########svcompparams.append('--cex --sv-comp    -l lazy    --rounds=2  --time=125  --depth=350  --atomic-parameter --well-nested-locks                   --unwind-while=2 --unwind-for=2')
svcompparams.append('--cex --deep-propagation --extra-tracking --sv-comp    -l lazy    --rounds=2  --time=150  --depth=1400  --atomic-parameter --well-nested-locks                   --unwind-while=2 --unwind-for=2')

svcompparams.append('--cex --deep-propagation --extra-tracking --sv-comp    -l lazy    --rounds=1  --time=60  --depth=400  --atomic-parameters --well-nested-locks                   --unwind-while=3 --unwind-for=16')

####svcompparams.append('--cex --sv-comp    -l lazy    --rounds=11 --time=90  --depth=500  --atomic-parameters --svcomp --softunwindbound --unwind-while=1 --unwind-for-max=11 --norobin')
svcompparams.append('--cex --deep-propagation --extra-tracking --sv-comp    -l lazy    --rounds=20 --time=250  --depth=750  --atomic-parameter --well-nested-locks --softunwindbound --unwind-while=1 --unwind-for-max=20 --norobin')

# time 240->500 due to safestack (needs longer timeout)
####svcompparams.append('--cex --sv-comp    -l lazy    --rounds 4  --time=240 --depth=560  --atomic-parameters --well-nested-locks --softunwindbound --unwind-while=1 --unwind-for=2 --unwind-for-max=3 --simplify-args')
svcompparams.append('--cex --deep-propagation --sv-comp    -l lazy    --rounds 4  --time=500 --depth=560  --atomic-parameters --well-nested-locks --softunwindbound --unwind-while=1 --unwind-for=2 --unwind-for-max=3 --simplify-args')


''' --stop-on-fail      -----parameters when preprocessing with abstract-interpretation
svcompparams.append('-l lazy+ai --rounds=2  --time=120 --depth=1400 --atomicparameter --svcomp --softunwindbound --unwind-while=1 --unwind-for=1 --unwind-for-max=50')
svcompparams.append('-l lazy+ai --rounds=2  --time=70  --depth=350  --atomicparameter --svcomp                   --unwind-while=2 --unwind-for=2')
svcompparams.append('-l lazy+ai --rounds=1  --time=60  --depth=250  --atomicparameter --svcomp                   --unwind-while=3 --unwind-for=16')
svcompparams.append('-l lazy+ai --rounds=11 --time=70  --depth=500  --atomicparameter --svcomp --softunwindbound --unwind-while=1 --unwind-for-max=11 --norobin')
svcompparams.append('-l lazy+ai --rounds=4  --time=200 --depth=560  --atomicparameter --svcomp --softunwindbound --unwind-while=1 --unwind-for=2 --unwind-for-max=3')
svcompparams.append('-l ase     --rounds=2  --time=200 --depth=1000 --unwind=1 --path-backend=./core/cde-package/cbmc-5.4')
'''



def usage(cmd, errormsg):
    print "Lazy-Cseq SV-COMP2020 wrapper script\n"

    print "Warning:  This is a wrapper script for the software verification competition."
    print "          For any other purpose, please use CSeq's main command-line frontend (cseq.py) instead."
    print "         (also see README file).\n"

    print " Usage:"
    print "   -h, --help                            display this help screen"
    print ""
    print "   -i<filename>, --input=<filename>      read input from the filename"
    print "   -s<specfile>, --spec=<specfile>       SV-COMP specfile (default:ALL.prp)"
    print "   -w<logfile>, --witness=<logfile>      counterexample output file (default:a.log)"
    print "   -q, --quick                           only run the first step with timeout 2 seconds"
    print "   -V, --version                         print version"
    print '\n' + errormsg + '\n'
    sys.exit(1)

def indent(s):
    t = ''
    for l in s.splitlines(): t += '   |'+l+'\n'
    return t

def main(args):    
    quick = False

    realstarttime = time.time()    # save wall time
    cmd = args[0]

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hi:s:w:qV", ["help", "input=", "spec=", "witness=", "quick", "version"])
    except getopt.GetoptError, err:
        print "error"
        usage(cmd, 'error: ' +str(err))

    inputfile = spec = witness = ''

    for o, a in opts:
        if o == "-v": verbose = True
        elif o in ("-h", "--help"): usage(cmd, "")
        elif o in ("-w", "--witness"): witness = a
        elif o in ("-s", "--spec"): spec = a
        elif o in ("-i", "--input"): inputfile = a
        elif o in ("-q", "--quick"):
            quick = True
            global svcompparams
            svcompparams[0] = re.sub(' --time=[0-9]+ ', ' --time=2 ', svcompparams[0])
            svcompparams = svcompparams[:1]
            print svcompparams
        elif o in ("-V", "--version"):
            print VERSION
            sys.exit(0)
        else: assert False, "unhandled option"

    # Check parameters
    if inputfile == '':
        print "Warning:  This is a wrapper script for the software verification competition."
        print "          For any other purpose, please use CSeq's main command-line frontend (cseq.py) instead."
        print "         (also see README file).\n"

        print 'error: input file name not specified'
        sys.exit(1)

    if not os.path.isfile(inputfile):
        print 'error: unable to open input file (%s)' % inputfile
        sys.exit(1)

    if witness == '': witness = inputfile + '.log'

    if spec == '': spec = 'ALL.prp'

    # if not os.path.isfile(spec):
    #     print 'error: unable to open spec file (%s)' % spec
    #     sys.exit(1)

    # Start the algorithm
    print VERSION +"\n"
    
    print "Warning:  This is a wrapper script for the software verification competition."
    print "          For any other purpose, please use CSeq's main command-line frontend (cseq.py) instead."
    print "         (also see README file).\n"


    last = lazyout = ''
    maxmem = 0

    try:
        resource.setrlimit(resource.RLIMIT_STACK, (resource.RLIM_INFINITY, resource.RLIM_INFINITY))
        resource.setrlimit(resource.RLIMIT_AS, (resource.RLIM_INFINITY, resource.RLIM_INFINITY))
    except:
        print ("Warning: unable to set resource limits, current limits:")
        print ("   RLIMIT_DATA: soft:%s hard:%s"  % resource.getrlimit(resource.RLIMIT_DATA)  )
        print ("   RLIMIT_STACK: soft:%s hard:%s"  % resource.getrlimit(resource.RLIMIT_STACK)  )
        print ("   RLIMIT_RSS: soft:%s hard:%s"  % resource.getrlimit(resource.RLIMIT_RSS)  )
        print ("   RLIMIT_MEMLOCK: soft:%s hard:%s"  % resource.getrlimit(resource.RLIMIT_MEMLOCK)  )
        print ("   RLIMIT_AS: soft:%s hard:%s"  % resource.getrlimit(resource.RLIMIT_AS)  )



    for i,params in enumerate(svcompparams):
        # After the first analysis pass,
        # reuse the sanitised input file saved previously.
        #if i>0: inputfile = inputfile[:-2] + '.c'

        cmdline = './cseq.py %s --input %s --witness %s' % (params, inputfile, witness)

        # Move on to the next input file
        # as soon as a reachable bug is found, or
        # the tool crashes.
        if last == 'UNSAFE':
            lazyout += '0.00,0.00, '
            continue
        elif last == 'UNKNOWN': 
            lazyout += '0.00,0.00, '
            continue
        elif last == 'BACKENDREJECT': 
            lazyout += '0.00,0.00, '
            continue

        indentedout = ''

        try:
            print cmdline
    
            p = subprocess.Popen(shlex.split(cmdline), stdout=subprocess.PIPE)
            out, err = p.communicate()

            print indent(out)

            # Only get the last line
            out = out.splitlines()[-1]
            out = out.replace(utils.colors.BLINK, '')
            out = out.replace(utils.colors.GREEN, '')
            out = out.replace(utils.colors.DARKRED, '')
            out = out.replace(utils.colors.RED, '')
            out = out.replace(utils.colors.BLACK, '')
            out = out.replace(utils.colors.NO, '')

            #print "OUT -> " + out
            lasttime = float(out[out[:out.rfind(', ')].rfind(", ") + 2:out.rfind(', ')].strip())
            lastmem = out[out.rfind(', ')+2:].replace('MB','')
            #print "MEM -> --" + lastmem + '--'
            if float(lastmem) > maxmem: maxmem = float(lastmem)
            str = '%2.2f,%s, ' % (float(lasttime), lastmem)
            lazyout += str

            if 'UNSAFE' in out:
                last = 'UNSAFE'
                continue

            if 'BACKENDREJECT' in out:
                last = 'BACKENDREJECT'
                continue

            last = 'SAFE'  
        except: ### subprocess.CalledProcessError, err:
            last = 'UNKNOWN';
            lazyout += '0.00,0.00, '
            continue

    # Change SAFE and UNSAFE into TRUE and FALSE, for SVCOMP
    if last == 'SAFE': last = 'TRUE'
    elif last == 'UNSAFE': last = 'FALSE'

    # If the tool crashed on the first attempt conclude that the file cannot be analysed.
    # If the tool crashed on the last attempt (with experimental ptr simplification),
    # conclude that no bugs can be found.
    ###elif last == 'BACKENDREJECT' and lazyout.endswith('0.00,0.00, '*(len(svcompparams)-1)): last = 'UNKNOWN'
    elif last == 'BACKENDREJECT': last = 'UNKNOWN'

    # Also conclude that we have been unable to find bugs if the analysis does not even start.
    elif last == 'UNKNOWN' and lazyout != '0.0,0.0, '*len(svcompparams): last = 'UNKNOWN'


    print "%s %s, %s, %2.1f,%2.1f" % (lazyout, last, witness, (time.time() - realstarttime), maxmem)


if __name__ == "__main__":
    main(sys.argv[0:])

