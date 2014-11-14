#!/usr/bin/env python
# -*- coding: latin1 -*-

from __future__ import print_function

import argparse
import sys
import os
import commands
import re
#import csv
#import ConfigParser
import shutil
import time
from pipes import quote
import subprocess, datetime, signal




# DEPENDENCY PARAMETERS
ABS_PATH_FILE = os.path.dirname(os.path.abspath(__file__)) + "/../../"

IS_PRE_CODE_i = False

# PATH_FILE_SETTINGS = ABS_PATH_FILE+'../../settings.cfg'
# if not os.path.isfile(PATH_FILE_SETTINGS):
#     print('Error: unable to find the settings.cfg file')
#     print('Please run ./configure to check the prerequisites to use Map2Check-FORTES tool')
#     sys.exit()

# config = ConfigParser.ConfigParser()
# config.read(PATH_FILE_SETTINGS)
# check_status_path = config.get('MAPFORTES_TOOL', 'mapfortes_path', 0)
# if check_status_path == 'empty' :
#     print("Sorry, you need to set up the Map2Check-FORTES path in settings.cfg file. See REAME file.")
#     sys.exit()


PATH_MAP_2_CHECK_FORTES = ABS_PATH_FILE + 'map2check.py'
if not os.path.isfile(PATH_MAP_2_CHECK_FORTES):
    print('Error: unable to find the map2check.py file')
    sys.exit()


# API library location
PATH_API_LIBRARY = ABS_PATH_FILE + 'modules/map2check/library/'
if not os.path.isdir(PATH_API_LIBRARY):
    print('Error: unable to find the path to API library')
    sys.exit()

list_to_write_in_html = []
list_csv_generation = []
id_count = 1
list_delete_tmp_file = []


# Options to code generation
FLAG_TRACK_ALL = False
GENERATE_GRAPHML = False



# Number of time that the analyzed program will be executed to perform the analysis
NUMEBER_OF_RE_EXEC_EXP = 0


def timeout_command(command, timeout):
    """call shell-command and either return its output or kill it
    if it doesn't normally exit within timeout seconds and return None"""

    # generating the cammad to be executed
    stdout = commands.getoutput("mktemp")
    cmd_shell = "timeout " + str(timeout) + " " + quote(command) + " > " + stdout

    start_t = time.time()
    result_only_stderr = commands.getoutput(cmd_shell)
    end_t = time.time()

    tt = end_t - start_t

    # generating data to report
    # output str to list
    result_stdrr_list = result_only_stderr.split("\n")
    len_stderr_list = len(result_stdrr_list)
    # removing possible timeout msg when we have a stop execution by asserts
    for index, line in enumerate(result_stdrr_list):
        match_to = re.search("(timeout: the monitored command dumped core)", line)
        if match_to:
            del result_stdrr_list[index]
            break

    if tt > timeout:
        return "TIME OUT",[]


    #sh:
    match_sh = re.search("^sh: ", result_stdrr_list[-1])
    if match_sh:
        del result_stdrr_list[-1]

    # generating stdout list
    result_sdtout = commands.getoutput("cat "+stdout)
    os.remove(stdout)
    result_sdtout_list = result_sdtout.split("\n")


    #cmd = command.split(" ")
    # cmd = quote(command)
    # start = datetime.datetime.now()
    # process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

    list_out_exec = []

    #
    # #Save PID from command
    # #pid_command_exec = commands.getoutput("pidof \""+command+"\"")
    # pid_command_exec = process.pid
    #
    # while process.poll() is None:
    #     #time.sleep(0.1)
    #     now = datetime.datetime.now()
    #     if (now - start).seconds > timeout:
    #         os.kill(int(pid_command_exec), signal.SIGKILL)
    #         os.kill(process.pid, signal.SIGKILL)
    #         os.waitpid(-1, os.WNOHANG)
    #         return "TIME OUT",[]


    #list_out_exec.append(process.stdout)
    list_out_exec.append(result_sdtout_list)
    #list_out_exec.append(process.stderr)
    list_out_exec.append(result_stdrr_list)

    return list_out_exec




def set_codes_to_experiment(_pathcprogram):
    
    global list_csv_generation
    global id_count
    global ACTUAL_MEM_USED_EXEC_inMB
    global tmp_FINAL_TIME
    global list_delete_tmp_file
    global NUMEBER_OF_RE_EXEC_EXP

        
    #--------------------------------------------------------
    # Read each program in the folder benchmarks and then
    # run the tool in two phases:
    #   (1) generate the new instance of the program with the tests cases
    #   (2) execute the new program instance generated in phase (1)
    #--------------------------------------------------------

                
    # >> Run (1) generate the new instance of the program with the tests cases
    #print("(1) Generating test cases and new program instance")
    list_result_gen = only_generate_code(_pathcprogram)



    # >> Run (2) execute the new program instance generated in phase (1)
    if list_result_gen[0]:

        #name_file_result = list_result_gen[1].replace("._mcf2check", "._out")
        name_file_result = ""

        cmd_bin_exec = str(list_result_gen[1])

        count_exe = 1
        check_exec_status_FAILED = False
        tmp_last_FAILED = False
        property_SVCOMP = "" # FALSE(p) p in {valid-free, valid-deref, valid-memtrack}

        # Here we consider the first failed identified
        # Maybe stop the while when we have a timeout
        while count_exe <= NUMEBER_OF_RE_EXEC_EXP and not check_exec_status_FAILED:
            flag_TIME_OUT = False

            # TODO: Let the user define this timeout value
            get_OUT_result_exec = timeout_command(cmd_bin_exec, 890)

            # Check if has a TIME OUT
            if not(str(get_OUT_result_exec[0]) == 'TIME OUT'):

                #tmp_list_OUT_STDOUT = get_OUT_result_exec[0].readlines()
                tmp_list_OUT_STDOUT = get_OUT_result_exec[0]
                tmp_list_OUT_STDERR = get_OUT_result_exec[1]

                # From this executions we ALWAYS save the failed execution
                # Check in the result of the bin execution
                matchSTDOUT_FAILED = re.search(r'(\[Failed\])', str(tmp_list_OUT_STDOUT))
                matchSTDERR_FAILED = re.search(r'(\[Failed\])', str(tmp_list_OUT_STDERR))


                if matchSTDOUT_FAILED or matchSTDERR_FAILED:
                    check_exec_status_FAILED = True
                    tmp_last_FAILED = check_exec_status_FAILED

                if tmp_last_FAILED:
                    check_exec_status_FAILED = True


                #save the execution result
                if check_exec_status_FAILED:
                    name_file_result = commands.getoutput("mktemp")
                    outputRun = open(str(name_file_result), "a+b")
                    outputRun.write("-----------------------------------------------------------------------\n")
                    outputRun.write(">>> Trace of " + _pathcprogram + "\n")
                    outputRun.write(">>> Generated at " + str(datetime.datetime.now()) + "\n")
                    outputRun.write("\n")
                    for index, line in enumerate(tmp_list_OUT_STDOUT):
                        # from Map Lib
                        matchprp = re.search(r"^[ ](FALSE\(.*\))", line)
                        if matchprp:
                            property_SVCOMP = matchprp.group(1)
                        outputRun.write(str(line)+"\n")

                    for line in tmp_list_OUT_STDERR:
                        # from asserts
                        matchprp = re.search(r"^[ ](FALSE\(.*\))", line)
                        if matchprp:
                            property_SVCOMP = matchprp.group(1)
                        outputRun.write(str(line)+"\n")
                    outputRun.close()
                #else:
                #    list_delete_tmp_file.append(name_file_result)
                    #property_SVCOMP = tmp_list_OUT_STDERR


            else:
                # We have a TIME OUT
                flag_TIME_OUT = True

            count_exe += 1
            time.sleep(3) # Wait for 3 second to the next execution



        if not flag_TIME_OUT:
            if check_exec_status_FAILED:
                # FAILED
                print("Status: VERIFICATION FAILED")
                print("The trace log is in < " + name_file_result + " >")
                print(property_SVCOMP)

            else:
                #SUCCESSFUL
                print("VERIFICATION SUCCESSFUL")
        else:
            # TIMEOUT
            print("Status: TIME OUT")
            print("In 890s")

    else:
        # ERROR to code generation
        print("Status: UNKNOW")
        print("ERROR - Generating test cases and new program instance")


    remove_tmp_files(list_delete_tmp_file)
    
    

def only_generate_code(cProgram):
    
    global list_csv_generation
    global id_count
    global PATH_MAP_2_CHECK_FORTES
    global list_delete_tmp_file
    global FLAG_TRACK_ALL
    global IS_PRE_CODE_i
    
    result_this_step = [] # 1 True or False; 2 bin name file

    if IS_PRE_CODE_i:
        new_c_program_name = cProgram.replace(".i","__mcf_new.c")
    else:
        new_c_program_name = cProgram.replace(".c","__mcf_new.c")

    list_delete_tmp_file.append(new_c_program_name)
    
    # Take time to test case generation
    cmd = None
    if FLAG_TRACK_ALL and GENERATE_GRAPHML:
        cmd = [PATH_MAP_2_CHECK_FORTES, "--only-assert", "--track-all", "--graphml-output", cProgram]
    elif FLAG_TRACK_ALL:
        cmd = [PATH_MAP_2_CHECK_FORTES, "--only-assert", "--track-all", cProgram]
    else:
        cmd = [PATH_MAP_2_CHECK_FORTES, "--only-assert", cProgram]

    #debug
    #    os.system("cat "+new_c_program_name)


    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    #Save result in file
    writenewcode = open(new_c_program_name, "w")
    for line in process.stdout.readlines():
        writenewcode.write(line)
    writenewcode.close()


    # Checking if the generation of the code was complete sucessful    
    get_result_compile = only_compile_code(new_c_program_name)
        
    bin_name_file = new_c_program_name.replace("__mcf_new.c","__mcf_new._mcf2check")
    list_delete_tmp_file.append(bin_name_file)
    
    if get_result_compile:
        result_this_step.append(True)
        result_this_step.append(bin_name_file)
    else:
        result_this_step.append(False)
        result_this_step.append(bin_name_file)
        
    return result_this_step
        
        


def only_compile_code(cProgram):
    global IS_PRE_CODE_i

    COMPILE_SCRIPT = './build_2_check.sh'
    
    head, tail = os.path.split(cProgram)
    
    # Now change the directory
    os.chdir( head )

    get_result = commands.getoutput(COMPILE_SCRIPT+" "+quote(tail))
    
    has_bin_file = False
    
    # Check if exist the bin file
    # First of all get the name bin file
    if IS_PRE_CODE_i:
        tail = tail.replace(".i","._mcf2check")
    else:
        tail = tail.replace(".c","._mcf2check")

    if os.path.isfile(tail):
        has_bin_file = True
    else:
        has_bin_file = False
    
    # Return directory
    os.chdir( ".." )
    
    #return get_result
    return has_bin_file


def copy_api_library(_cprogramfile):
    global list_delete_tmp_file
    global PATH_API_LIBRARY
    global IS_PRE_CODE_i

    pathofcprogram = os.path.dirname(_cprogramfile)
    pathscriptcompile = ""
    pathheaderapi = ""
    pathlibapi = ""

    #The script to compile
    if IS_PRE_CODE_i:
        pathscriptcompile = PATH_API_LIBRARY+'build_i/build_2_check.sh'
        pathheaderapi = PATH_API_LIBRARY+'header_i/check_safety_memory_FORTES.h'
    else:
        pathscriptcompile = PATH_API_LIBRARY+'build_2_check.sh'
        pathheaderapi = PATH_API_LIBRARY+'check_safety_memory_FORTES.h'

    pathlibapi = PATH_API_LIBRARY+'check_safety_memory_FORTES.o'

    try:
        shutil.copy2(pathscriptcompile, pathofcprogram)
        list_delete_tmp_file.append(pathofcprogram+'/build_2_check.sh')

        shutil.copy2(pathheaderapi, pathofcprogram)
        list_delete_tmp_file.append(pathofcprogram+'/check_safety_memory_FORTES.h')

        shutil.copy2(pathlibapi, pathofcprogram)
        list_delete_tmp_file.append(pathofcprogram+'/check_safety_memory_FORTES.o')

    except shutil.Error as e:
        print('Error: %s' % e)
        raise
        # eg. source or destination doesn't exist
    except IOError as e:
        print('Error: %s' % e.strerror)
        raise


def remove_tmp_files(list_path):
    for path in list_path:
        os.remove(path)
        #print(path)





# -------------------------------------------------
# Main python program
# -------------------------------------------------

if __name__ == "__main__":
    #global NUMEBER_OF_RE_EXEC_EXP
    
    ############# Parse args options
    parser = argparse.ArgumentParser(description='Run experiment using Map2Check-FORTES v3')
    parser.add_argument(dest='pathCProgram', metavar='cprogram', type=str,
               help='the path of the C program')
    parser.add_argument('-n','--number-check', type=int, dest='setNumberCheck', nargs='?',
                        default=3, help='set the number of the program execution to check')
    parser.add_argument('-t','--track-all', action="store_true" , dest='setTrackAll',
                        help='tracking all variables value')
    parser.add_argument('-g','--graphml-output', action="store_true" , dest='setGraphOut',
                       help='generate the output of the tool in GraphML format (experimental)', default=False)
    parser.add_argument('-i','--is-preprocessed', action="store_true" , dest='isPreCode',
                       help='identify that is a preprocessed code', default=False)

    group = parser.add_mutually_exclusive_group()
    group.add_argument('-o','--only-generated-code', action="store_true" , dest='setOnlyCode',
               help='generating only the code with the test cases')
    group.add_argument('-c','--only-compile', action="store_true" , dest='setOnlyCompile', 
               help='only compile the code generated by Map2Check-FORTES')

    args = parser.parse_args()        
    
    ############# Check options in the args
    ### vars to save data option
    
    pathCPrograms=''

    if args.pathCProgram:
        if not os.path.isfile(args.pathCProgram):
            print('Error: unable to open find the file (%s)' % args.pathCProgram)
            parser.parse_args(['-h'])
            sys.exit()
        else:

            if args.setTrackAll:
                FLAG_TRACK_ALL = True
            if args.setGraphOut:
                GENERATE_GRAPHML = True

            NUMEBER_OF_RE_EXEC_EXP = args.setNumberCheck

            if args.isPreCode:
                IS_PRE_CODE_i = True

            pathCPrograms = args.pathCProgram
            copy_api_library(pathCPrograms)

            if not args.setOnlyCode and not args.setOnlyCompile:
                set_codes_to_experiment(pathCPrograms)

    # improve this - TODO not work
    if args.setOnlyCode:
        only_generate_code(os.path.abspath(pathCPrograms))
    if args.setOnlyCompile:
        only_compile_code(os.path.abspath(pathCPrograms))
    
