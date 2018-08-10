# Script based on
# https://raw.githubusercontent.com/smackers/smack/master/test/regtest.py

#usr/bin/env python
from os import path
from multiprocessing.pool import ThreadPool
import multiprocessing
import os
import signal
import logging
import yaml
import psutil
import argparse
from os import path
import subprocess
import re
import glob
import time
import sys
import shlex

OVERRIDE_FIELDS = ['memory', 'time-limit', 'memory-limit', 'skip', 
                    'property', 'expect', 'check-by']

def bold(text):
    return '\033[1m' + text + '\033[0m'


def red(text, log_file):
    if log_file:
        return text
    else:
        return '\033[0;31m' + text + '\033[0m'


def green(text, log_file):
    if log_file:
        return text
    else:
        return '\033[0;32m' + text + '\033[0m'


def merge(metadata, yamldata):
    for key in OVERRIDE_FIELDS:
        if key in yamldata:
            metadata[key] = yamldata[key]


def metadata(file):
    m = {}
    prefix = []
    flag_is_yml = False

    for d in path.dirname(file).split('/'):
        prefix += [d]
        yaml_file = path.join(*(prefix + ['config.yml']))
        if path.isfile(yaml_file):
            flag_is_yml = True
            with open(yaml_file, "r") as f:
                data = yaml.safe_load(f)
                merge(m,data)

    # Get expect result from file name
    filename = path.basename(file)

    match = re.search(r'false', filename)
    if match:
        m['expect'] = "FALSE"
    else:
        m['expect'] = "TRUE"

    return m


def process_test(cmd, test, expect, checkby, log_file):
    """
    This is the worker function for each process. This function process the supplied
    test and returns a tuple containing  indicating the test results.

    :return: A tuple with the
    """    
    str_result = "{0:>20}\n".format(test)           
    
    t0 = time.time()    
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)    
    out, err  = p.communicate()    
    elapsed = time.time() - t0
    status = 0    
    

    # get the test results    
    result = (out + err).decode('utf-8')
    
    # generating pattern to check result    
    if expect == "FALSE":
        if checkby == "MEMTRACK":
            expect = "FALSE_MEMTRACK"
        elif checkby == "DEREF":
            expect = "FALSE_DEREF"
        elif checkby == "FREE":
            expect = "FALSE_FREE"
        elif checkby == "OVERFLOW":
            expect = "FALSE_OVERFLOW"
        else:
            expect = "FALSE"
    
    # checking the result    
    match_r = re.search(r'RESULT: (.[^_]*)', result)   
    ck_result = match_r.group(1).strip()
    
    if ck_result == expect and status == 0:
        str_result += green('PASSED ', log_file)
    elif ck_result == 'Timed out':
        str_result += red('TIMEOUT', log_file)
    elif result == 'UNKNOWN':
        str_result += red('UNKNOWN', log_file)
    else:
        str_result += red('FAILED ', log_file)
    
    """
    if result == expect and status == 0:
        str_result += green('PASSED ', log_file)
    elif result == 'Timed out':
        str_result += red('TIMEOUT', log_file)
    elif result == 'UNKNOWN':
        str_result += red('UNKNOWN', log_file)
    else:
        str_result += red('FAILED ', log_file)
    """
    
    str_result += '  [%.2fs]' % round(elapsed, 2)
    
    return str_result    


passed = failed = timeouts = unknowns = 0

def tally_result(result):
    """
    Tallies the result of each worker. This will only be called by the main thread.
    """
    # log the info    
    logging.info(result)    

    global passed, failed, timeouts, unknowns
    if "PASSED" in result:
        passed += 1
    elif "FAILED" in result:
        failed += 1
    elif "TIMEOUT" in result:
        timeouts += 1
    elif "UNKNOWN" in result:
        unknowns += 1

def main():
    """
    Main entry point for the test suite.
    """
    t0 = time.time()
    num_cpus = multiprocessing.cpu_count()
    mem_total = psutil.virtual_memory().total / (1024 * 1024)

    # configure the CLI
    parser = argparse.ArgumentParser()
    parser.add_argument("--folder", action="store", default="regression", type=str,
                      help="sets the regressions folder to run")
    parser.add_argument("--threads", action="store", dest="n_threads", default=num_cpus, type=int,
                      help="execute regressions using the selected number of threads in parallel")
    parser.add_argument("--log", action="store", dest="log_level", default="DEBUG", type=str,
                      help="sets the logging level (DEBUG, INFO, WARNING)")
    parser.add_argument("--output-log", action="store", dest="log_path", type=str,
                      help="sets the output log path. (std out by default)")
    args = parser.parse_args()

    # configure the logging
    log_format = ''
    log_level = logging.DEBUG

    # add more log levels later (if needed)
    if args.log_level.upper() == "INFO":
        log_level = logging.INFO
    elif args.log_level.upper() == "WARNING":
        log_level = logging.WARNING

    # if the user supplied a log path, write the logs to that file.
    # otherwise, write the logs to std out.
    if args.log_path:
        logging.basicConfig(filename=args.log_path, format=log_format, level=log_level)
    else:
        logging.basicConfig(format=log_format, level=log_level)

    logging.debug("Creating Pool with '%d' Workers" % args.n_threads)
    p = ThreadPool(processes=args.n_threads)

    try:
        # start the tests
        logging.info("Running regression tests...")
        logging.info("")

        # start processing the tests.
        results = []

        for test in sorted(glob.glob("./" + args.folder + "/**/*.c", recursive=True)):
            # get the meta data for this test
            meta = metadata(test)

            if 'memory-limit' in meta:
                if meta['memory-limit'] > mem_total:
                    continue
            
            if not 'property' in meta:
                logging.info("You not defined the property to be checked! Checking default: Memory Safety")                
                meta['property'] = ['../../properties/memsafety.prp']

            if 'skip' in meta:
                if meta['skip'] == True:
                    continue
            else:
                # build up the subprocess command
                cmd = ['./map2check-wrapper.py']
                # build up the subprocess command
                cmd += ['--timeout', str(meta['time-limit'])]
                cmd += ['--propertyfile', str(meta['property'][0])]
                cmd += [str(test)]
                # name = path.splitext(path.basename(test))[0]                
                # process_test(cmd, test, memory, expect, log_file):
                """
                print(cmd[:])
                print(test)
                print(meta['expect'])
                print(meta['check-by'])
                print(args.log_path)  
                """
                # ./map2check-wrapper.py --timeout 60 --propertyfile properties/memsafety.prp regression/MEM/test_false.c                                              
                
                r = p.apply_async(process_test,
                args=(cmd[:], test, meta['expect'], meta['check-by'], args.log_path,),
                callback=tally_result)
                results.append(r)                
                
                # process_test(cmd[:], test, meta['expect'], meta['check-by'], args.log_path)

        # keep the main thread active while there are active workers
        #print(results)
        for r in results:            
            r.wait()

    except KeyboardInterrupt:
        logging.debug("Caught KeyboardInterrupt, terminating workers")
        p.terminate() # terminate any remaining workers
        p.join()
    else:
        logging.info("")
        logging.debug("Quitting normally")        
        # close the pool. this prevents any more tasks from being submitted.
        p.close()
        p.join() # wait for all workers to finish their tasks

    # log the elapsed time
    elapsed_time = time.time() - t0
    logging.info("")
    logging.info("Total result:")
    logging.info("---------------------------")
    logging.info(' ELAPSED TIME [%.2fs]' % round(elapsed_time, 2))

    # log the test results
    logging.info(' PASSED count: %d' % passed)
    logging.info(' FAILED count: %d' % failed)
    logging.info(' TIMEOUT count: %d' % timeouts)
    logging.info(' UNKNOWN count: %d' % unknowns)
    logging.info("---------------------------")

    # if there are any failed tests or tests that timed out, set the system
    # exit code to a failure status
    if timeouts > 0 or failed > 0 or unknowns > 0:
        sys.exit(1)

if __name__=="__main__":
    main()
