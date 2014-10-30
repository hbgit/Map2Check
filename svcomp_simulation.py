#!/usr/bin/env python
# -*- coding: latin1 -*-

from __future__ import print_function

import argparse
import sys
import os
import commands
import re
import csv
import ConfigParser
import shutil
import time
import string
import random
from pipes import quote
import subprocess, datetime, os, time, signal


# DEPENDENCY PARAMETERS
ABS_PATH_FILE = os.path.dirname(os.path.abspath(__file__))


# Name of the final report
OUTPUT_REPORT_FILE = ABS_PATH_FILE+"/<name_>report.html"
# Name of the tmp report that is generated according to tests execution
id = random.choice(string.letters)
TMP_REPORT_FILE = ABS_PATH_FILE+'/tmp_result_'+id.strip()+'.html'

# HTML Model Report
PATH_HTML_MODEL_REPORT = ABS_PATH_FILE+'/modules/html_report/report_model.html'
if not os.path.isfile(PATH_HTML_MODEL_REPORT):
    print('Error: unable to find the report_model.html file')
    sys.exit()


list_to_write_in_html = []
list_csv_generation = []
id_count = 1
list_delete_tmp_file = []



# CPAChecker
CPACHECKER_PATH = '/home/hbazure/drive/Docs/CPAChecker/trunk/'
#CPACHECKER_PATH = '/home/hrocha/Documents/Projects_DEV/CPAChecker/trunk/'
#CPACHECKER_PATH = '/home/nhb/Documents/ON_DEV/CPAChecker/trunk/'
CPACHECKER_OPTIONS = 'scripts/cpa.sh -sv-comp14--memorysafety -spec config/specification/cpalien-leaks.spc ' \
                     '-spec config/specification/TerminatingFunctions.spc -setprop cfa.useMultiEdges=false ' \
                     '-setprop parser.transformTokensToLines=true -spec'

# Just for not generate errors
if not os.path.exists(CPACHECKER_PATH):
    print("It is necessary to set the correct path to CPAChecker tool")
    sys.exit()

# TODO: test cmd form svocmp site
#scripts/cpa.sh -sv-comp14--memory-safety -setprop cfa.useMultiEdges=false -setprop parser.transformTokensToLines=true -setprop spec.matchSourcecodeData=false -spec witness.graphml -spec PropertyERROR.prp test.c


# Variables to memory usage
ACTUAL_MEM_USED_EXEC_inMB = 0.0
ACTUAL_TC_GEN_inMB = 0.0
TOTAL_TC_GEN_inMB = 0.0
ACTUAL_TIME_TC_GEN = 0
TOTAL_TIME_TC_GEN = 0
ACTUAL_GEN_GRAPH_inMB = 0.0
TOTAL_GEN_GRAPH_inMB = 0.0
ACTUAL_TIME_GEN_GRAPH = 0.0
TOTAL_TIME_GEN_GRAPH = 0.0


def get_mem_usage(_listtxtoutput):
    global ACTUAL_MEM_USED_EXEC_inMB
    #txt_result = _listtxtoutput.split("\n")

    ru_maxrss = float(_listtxtoutput[-1].strip())
    ACTUAL_MEM_USED_EXEC_inMB = ru_maxrss * float(0.0009765625)


tmp_FINAL_TIME = 0
ACTUAL_FILE_PATH = ''



def set_codes_to_experiment(pathCPrograms):
    
    global list_csv_generation
    global id_count
    global OUTPUT_REPORT_FILE
    global ACTUAL_MEM_USED_EXEC_inMB
    global tmp_FINAL_TIME
    global list_delete_tmp_file
    global ACTUAL_GEN_GRAPH_inMB
    global ACTUAL_TIME_GEN_GRAPH
    global ACTUAL_FILE_PATH
    
        
    # Map2Check PARAMS
    MAP2CHECK_WRAPPER_SCRIPT_PATH = "wrapper_script_map2check.sh"
    MAP2CHECK_VERSION = "version 3"
    MAP2CHECK_PARAMS = "--complete-check 3 --graphml-output --witnesspath"
    
    # SYSTEM INFO
    DATE_EXECUTION = commands.getoutput("date")    
    CPU_CORE_NUMBER = commands.getoutput("cat /proc/cpuinfo | grep processor | wc -l")
    OS = commands.getoutput("uname -sr")
    CPU_INFO = "CPU:"+commands.getoutput("cat /proc/cpuinfo | grep \"model name\" | tail -n1 | cut -d \":\" -f2")
    MEM_INFO = "RAM:"+commands.getoutput("cat /proc/meminfo | grep \"MemTotal\" | cut -d \":\" -f2").strip()
    LIMIT_EXP = "timelimit: 900 s, memlimit: 5120 MB, CPU core limit: 8"
    INITIAL_TIMESTAMP = time.time()

    OUTPUT_REPORT_FILE = OUTPUT_REPORT_FILE.replace("<name_>",str(os.path.basename(pathCPrograms))+"_")

    # HTML CONTENT
    HTML_TABLE_HEADER = "<thead> \n " \
                        "<tr id=\"tool\">" \
                          "<td style=\"width: 60%\">Tool</td>" \
                          "<td colspan=\"3\">"+MAP2CHECK_VERSION+"</td>" \
                        "</tr>"\
                        "<tr id=\"limits\">" \
                          "<td>Limits</td>" \
                          "<td colspan=\"3\">"+LIMIT_EXP+"</td>"+\
                        "</tr>" \
                        "<tr id=\"os\">" \
                          "<td>OS</td>" \
                          "<td colspan=\"3\">"+OS+"</td>"+\
                        "</tr>" \
                        "<tr id=\"system\">" \
                          "<td>System</td>" \
                          "<td colspan=\"3\">"+CPU_INFO+" - "+MEM_INFO+"</td>" \
                        "</tr>" \
                        "<tr id=\"date\">" \
                          "<td>Date of run</td>" \
                          "<td colspan=\"3\">"+DATE_EXECUTION+"</td>" \
                        "</tr>" \
                        "<tr id=\"options\">" \
                          "<td>Options</td>" \
                          "<td colspan=\"3\">"+MAP2CHECK_PARAMS+"</td>" \
                        "</tr>" \
                        "<tr id=\"columnTitles\">" \
                          "<td class=\"clickable\" "+"title=\"Click here to toggle visibility of columns\">"+str(pathCPrograms)+\
                          "</td>" \
                          "<td colspan=\"1\" class=\"clickable\" title=\"Click here to show a graph of this column\">" \
                          "status</td>" \
                          "<td colspan=\"1\" class=\"clickable\" title=\"Click here to show a graph of this column\">" \
                          "CPAChecker re-check status</td>" \
                          "<td colspan=\"1\" class=\"clickable\" title=\"Click here to show a graph of this column\">" \
                          "total time(s)</td>" \
                        "</tr>" \
                        "</thead> \n <tbody> \n"
    #print(HTML_TABLE_HEADER)
    #html_report.write(HTML_TABLE_HEADER)
    #list_to_write_in_html.append(HTML_TABLE_HEADER)
    html_tmp_report = open(TMP_REPORT_FILE, "a+b")
    html_tmp_report.write(HTML_TABLE_HEADER)                
    html_tmp_report.close()
    
    # REPORT CONTROL
    TOTAL_FILES         = 0
    TOTAL_MEM_IN_EXE    = 0
    CORRECT_RESULTS     = 0
    TOTAL_MEMO_CORRECT  = 0
    TIME_TOTAL_CORRECT  = 0
    TCEXCMB_TOTAL_CORRECT = 0
    CORRECT_TRUES       = 0
    CPACHECK_POSITIVE_cont   = 0
    CORRECT_FALSES      = 0
    CPACHECK_NEGATIVE_cont   = 0
    FALSE_POSITIVES     = 0
    TCEXCMB_TOTAL_FPOSITI = 0
    TOTAL_MEMO_FPOSITI  = 0
    TIME_TOTAL_FPOSITI  = 0
    FALSE_NEGATIVES     = 0
    TCEXCMB_TOTAL_FNEGATI = 0
    TIME_TOTAL_FNEGATI  = 0
    TOTAL_MEMO_FNEGATI  = 0
    NUM_UNKNOW_AND_TO   = 0
    MAX_SCORE           = 0
    TOTAL_POINTS        = 0
    COUNT_EXP_TRUE      = 0
    COUNT_EXP_FALSE     = 0

    TOTAL_EXECUTION_TIME = 0.0

    STATUS_GRAPHML_GEN  = True
    
    #cp ./REPORT/header.html $OUTPUT_REPORT_FILE
    #echo $HTML_TABLE_HEADER >> $OUTPUT_REPORT_FILE
    
    print("*** Map2Check-Fortes Benchmark Runner (Single-Core) v0.1 ***")
    print( "Tool: "+MAP2CHECK_VERSION)
    print( "Date of run: "+str(commands.getoutput("date")))
    print( "System: "+CPU_INFO+" -- "+MEM_INFO)
    print( "Options: only the program")
    print( "Source: "+pathCPrograms)
    
        
    #--------------------------------------------------------
    # Read each program in the folder benchmarks and then
    # run the tool in two phases:
    #   (1) generate the new instance of the program with the tests cases
    #   (2) execute the new program instance generated in phase (1)
    #--------------------------------------------------------

    print()
    
    total_num_programs = 0
    
    # For internal log
    #header_csv_generation = "ID ; Program ; Status Generation ; Time" LOG_INT
    #list_csv_generation.append(header_csv_generation) LOG_INT
    
    # Get number total of the programs 
    for root, dirs, files in os.walk(pathCPrograms):                
        for file in files:
            if file.endswith(".c"):
                total_num_programs += 1
                
    TOTAL_FILES = total_num_programs
    
    
    for root, dirs, files in os.walk(pathCPrograms):                
        for file in files:
            if file.endswith(".c"):
                print(file, end=" -> ")
                print(" "+str(id_count)+" from "+str(total_num_programs))
                get_path_program = os.path.join(root, file)
                
                FILENAME = get_path_program

                CPACHECKER_STATUS = "-"
                CPACHECKER_OUTPUT_PATH = ''
                
                
                # Get the expected result from benchmark program
                #EXPECTED_FAILED_RESULT = commands.getoutput("echo "+file+" | grep 'false'| wc -l")  
                matchRe_FAILED = re.search(r'false-(.[^\.]*)', str(file))
                matchRe_UNSAFE = re.search(r'_(unsafe)', str(file))
                EXPECTED_FAILED_RESULT = False
                if matchRe_FAILED or matchRe_UNSAFE:
                    if matchRe_FAILED:
                        print(">> EXPECTED < FALSE > - label: "+matchRe_FAILED.group(1))
                    elif matchRe_UNSAFE:
                        print(">> EXPECTED < FALSE > - label: "+matchRe_UNSAFE.group(1))
                    COUNT_EXP_FALSE += 1
                    EXPECTED_FAILED_RESULT = True
                    MAX_SCORE = MAX_SCORE + 1
                else:
                    print(">> EXPECTED < TRUE >")
                    COUNT_EXP_TRUE += 1
                    EXPECTED_FAILED_RESULT = False
                    MAX_SCORE = MAX_SCORE + 2

                ACTUAL_FILE_PATH = get_path_program
                # generating the name for witness file to save the log
                witnessfilepath = get_path_program.replace(".c",".graphml")
                CPACHECKER_OUTPUT_PATH = get_path_program.replace(".c",".cpachecker_out")

                # Running Map2Check
                cmd = "./"+MAP2CHECK_WRAPPER_SCRIPT_PATH + " " + get_path_program + " " + witnessfilepath
                #print(">>>", cmd)
                print("\t\t >>> Runnning Map2Check")
                INITIAL_EXECUTION_TIMESTAMP = time.time()

                result_exec = commands.getoutput(cmd)

                FINAL_EXECUTION_TIMESTAMP = time.time()

                print("\t\t\t Result: "+str(result_exec).strip())
                print("\t\t\t Actual time: %1.5s" % (FINAL_EXECUTION_TIMESTAMP-INITIAL_EXECUTION_TIMESTAMP))

                # Flag Status
                FAILED = False
                SUCCESS = False
                flag_TIME_OUT = False

                # >> Analysing the result to write the log
                print("\t\t Finished execution")
                # Check if has a TIME OUT
                if not(str(result_exec).strip() == 'UNKNOWN'):

                    # -- The result is True or False
                    # Checking it was False
                    if str(result_exec) == "FALSE":

                        FAILED = True
                        SUCCESS = False

                        if CPACHECKER_PATH:
                            print("\t\t >>> Runnning CPAChecker")
                            cwd = os.getcwd()
                            os.chdir(CPACHECKER_PATH)

                            #print("CMD: "+CPACHECKER_OPTIONS+" "+result_gen_graphml[1]+" "+get_path_program)
                            result_recheck = commands.getoutput(CPACHECKER_OPTIONS+" "+witnessfilepath+" "+get_path_program)
                            # Analysing CPAChecker result
                            list_recheck = result_recheck.split("\n")
                            flag_cpa = False
                            for line in list_recheck:
                                match_failed_CPA = re.search(r"Verification result: FALSE", line)
                                match_nobug_CPA = re.search(r"Verification result: TRUE", line)
                                if match_failed_CPA:
                                    print("\t\t\t CPAChecker --- POSITIVE")
                                    CPACHECKER_STATUS = "positive"
                                    CPACHECK_POSITIVE_cont += 1
                                    flag_cpa = True
                                    break
                                elif match_nobug_CPA:
                                    print("\t\t\t CPAChecker --- NEGATIVE")
                                    CPACHECKER_STATUS = "negative"
                                    CPACHECK_NEGATIVE_cont += 1
                                    flag_cpa = True
                                    break


                            if not flag_cpa:
                                print("\t\t\t CPAChecker --- UNKNOWN")
                                CPACHECKER_STATUS = "unknown"

                            os.chdir(cwd)
                            # Write cpachecker log
                            commands.getoutput("echo \""+result_recheck+"\" > "+CPACHECKER_OUTPUT_PATH)

                    elif str(result_exec) == "TRUE":
                        FAILED = False
                        SUCCESS = True


                    print("\t\t Finished the analisys of this execution")


                else:
                    # We have a TIME OUT or Crash tool
                    flag_TIME_OUT = True
                    FAILED = False
                    SUCCESS = False


                TIME = FINAL_EXECUTION_TIMESTAMP - INITIAL_EXECUTION_TIMESTAMP
                TOTAL_EXECUTION_TIME += TIME
                
                #print("DEBUG: EXP_FALSE: "+str(EXPECTED_FAILED_RESULT)+" - FAILED: "+str(FAILED)+" - SUCCESS: "+str(SUCCESS))
                
                CSS_CLASS   = ""
                RESULT_TEXT = ""
                
                #flag_TIME_OUT
                
                if not FAILED and not SUCCESS and flag_TIME_OUT:
                    CSS_CLASS         = "status error"
                    #RESULT_TEXT       = "timeout"
                    RESULT_TEXT       = "unknown"
                    NUM_UNKNOW_AND_TO += 1
                    #TOTAL_MEM_IN_EXE  += ACTUAL_MEM_USED_EXEC_inMB
                    #echo $(echo -e"\033[0;33munknown\033[0m" | cut -d " " -f2) "in $TIME""s"
                    print(">> ACTUAL: TIMEOUT in "+str(TIME)+" s")
                    #print("\t\t Memo: %1.2f" % ACTUAL_MEM_USED_EXEC_inMB)
                    
                if not FAILED and not SUCCESS and not flag_TIME_OUT:
                    CSS_CLASS         = "status unknown"
                    RESULT_TEXT       = "unknown"
                    NUM_UNKNOW_AND_TO += 1
                    #TOTAL_MEM_IN_EXE  += ACTUAL_MEM_USED_EXEC_inMB
                    #echo $(echo -e"\033[0;33munknown\033[0m" | cut -d " " -f2) "in $TIME""s"
                    print(">> ACTUAL: UNKNOWN in "+str(TIME)+" s")
                    #print("\t\t Memo: %1.2f" % ACTUAL_MEM_USED_EXEC_inMB)
                  
                elif EXPECTED_FAILED_RESULT and FAILED and not flag_TIME_OUT:
                    #CSS_CLASS         = "correctProperty"
                    CSS_CLASS         = "status correct false"
                    RESULT_TEXT       = "false(label)"
                    CORRECT_RESULTS   = CORRECT_RESULTS + 1
                    CORRECT_FALSES    = CORRECT_FALSES + 1
                    #TOTAL_MEM_IN_EXE  += ACTUAL_MEM_USED_EXEC_inMB
                    #TCEXCMB_TOTAL_CORRECT += ACTUAL_MEM_USED_EXEC_inMB + ACTUAL_TC_GEN_inMB
                    #TOTAL_MEMO_CORRECT  += ACTUAL_MEM_USED_EXEC_inMB

                    TIME_TOTAL_CORRECT += TIME
                    #echo $(echo -e "\033[0;32mfalse(label)\033[0m" | cut -d " " -f2) "in $TIME""s"
                    print(">> ACTUAL: false(label) in "+str(TIME)+" s")
                    #print("\t\t Memo: %1.2f" % ACTUAL_MEM_USED_EXEC_inMB)
                  
                elif EXPECTED_FAILED_RESULT and not FAILED and not flag_TIME_OUT:
                    #CSS_CLASS         = "wrongProperty"
                    CSS_CLASS         = "status wrong true"
                    RESULT_TEXT       = "true"
                    FALSE_POSITIVES   = FALSE_POSITIVES + 1
                    #TOTAL_MEM_IN_EXE  += ACTUAL_MEM_USED_EXEC_inMB
                    #TOTAL_MEMO_FPOSITI += ACTUAL_MEM_USED_EXEC_inMB
                    #TCEXCMB_TOTAL_FPOSITI += ACTUAL_MEM_USED_EXEC_inMB + ACTUAL_TC_GEN_inMB
                    TIME_TOTAL_FPOSITI += TIME
                    #echo $(echo -e "\033[0;31mtrue\033[0m" | cut -d " " -f2) "in $TIME""s"
                    print(">> ACTUAL: true in "+str(TIME)+" s")
                    #print("\t\t Memo: %1.2f" % ACTUAL_MEM_USED_EXEC_inMB)
                  
                elif not EXPECTED_FAILED_RESULT and FAILED and not flag_TIME_OUT:
                    #CSS_CLASS         = "wrongProperty"
                    CSS_CLASS         = "status wrong false"
                    RESULT_TEXT       = "false(label)"
                    FALSE_NEGATIVES   = FALSE_NEGATIVES + 1
                    #TOTAL_MEM_IN_EXE  += ACTUAL_MEM_USED_EXEC_inMB
                    #TOTAL_MEMO_FNEGATI += ACTUAL_MEM_USED_EXEC_inMB
                    #TCEXCMB_TOTAL_FNEGATI += ACTUAL_MEM_USED_EXEC_inMB + ACTUAL_TC_GEN_inMB
                    TIME_TOTAL_FNEGATI += TIME
                    #echo $(echo -e "\033[0;31mfalse(label)\033[0m" | cut -d " " -f2) "in $TIME""s"
                    print(">> ACTUAL: false(label) in "+str(TIME)+" s")
                    #print("\t\t Memo: %1.2f" % ACTUAL_MEM_USED_EXEC_inMB)
                  
                elif not EXPECTED_FAILED_RESULT and not FAILED and not flag_TIME_OUT:
                    #CSS_CLASS         = "correctProperty"
                    CSS_CLASS         = "status correct true"
                    RESULT_TEXT       = "true"
                    CORRECT_RESULTS   = CORRECT_RESULTS + 1
                    CORRECT_TRUES     = CORRECT_TRUES + 1
                    #TOTAL_MEM_IN_EXE  += ACTUAL_MEM_USED_EXEC_inMB
                    #TOTAL_MEMO_CORRECT  += ACTUAL_MEM_USED_EXEC_inMB
                    #TCEXCMB_TOTAL_CORRECT += ACTUAL_MEM_USED_EXEC_inMB + ACTUAL_TC_GEN_inMB
                    TIME_TOTAL_CORRECT += TIME
                    #echo $(echo -e "\033[0;32mtrue\033[0m" | cut -d " " -f2) "in $TIME""s"
                    print(">> ACTUAL: true in "+str(TIME)+" s")
                    #print("\t\t Memo: %1.2f" % ACTUAL_MEM_USED_EXEC_inMB)
                  

                HTML_ENTRY="\t <tr><td>"+FILENAME+"</td><td class=\""+CSS_CLASS+"\">"+RESULT_TEXT+"</td>" \
                           "<td align=\"center\">" + str(CPACHECKER_STATUS) + "&nbsp;</td>" \
                           "<td align=\"center\">" + str(("%1.2f" % TIME)) + "&nbsp;</td>" \
                           "</tr> \n"

                
                # write in tmp file the results
                #list_to_write_in_html.append(HTML_ENTRY)
                html_tmp_report = open(TMP_REPORT_FILE, "a+b")
                html_tmp_report.write(HTML_ENTRY)                
                html_tmp_report.close()
                
                id_count += 1
                print(" ")

            #print("")
                 
    # end
    print()
    print("------------------------------------------------------")
    print()
    
    FINAL_TIMESTAMP = time.time()
    
    # CALCULATE POINTS
    TOTAL_POINTS = (TOTAL_POINTS + 2 * CORRECT_TRUES)   
    print("CORRECT_TRUES  : ",str(CORRECT_TRUES)) 
    TOTAL_POINTS = (TOTAL_POINTS + CORRECT_FALSES)    
    print("CORRECT_FALSES : ",str(CORRECT_FALSES)) 
    TOTAL_POINTS = (TOTAL_POINTS - 8 * FALSE_POSITIVES)    
    print("FALSE_POSITIVES: ",str(FALSE_POSITIVES)) 
    TOTAL_POINTS = (TOTAL_POINTS - 4 * FALSE_NEGATIVES)    
    print("FALSE_NEGATIVES: ",str(FALSE_NEGATIVES)) 
    print("UNKNOW and TO  : ",str(NUM_UNKNOW_AND_TO))
    print()
    print("COUNT_EXP_FALSE: ",str(COUNT_EXP_FALSE))
    print("COUNT_EXP_TRUE: ",str(COUNT_EXP_TRUE))
    print()
    print("COUNT CPAChecker POS: ",str(CPACHECK_POSITIVE_cont))
    print("COUNT CPAChecker NEG: ",str(CPACHECK_NEGATIVE_cont))

#    TOTAL_EXECUTION_TIME = FINAL_TIMESTAMP - INITIAL_TIMESTAMP
    
    # HTML CONTENT
    HTML_TABLE_FOOTER="</tbody> \n <tfoot>" \
                      "<tr>" \
                        "<td>total files</td>" \
                        "<td>"+str(TOTAL_FILES)+"</td>" \
                        "<td>"+str(CPACHECK_POSITIVE_cont+CPACHECK_NEGATIVE_cont)+"</td>" \
                        "<td>"+str(("%1.2f" % TOTAL_EXECUTION_TIME))+"&nbsp;</td>" \
                      "</tr>" \
                      "<tr>" \
                        "<td title=\"(no bug exists + result is SAFE) OR "+\
                        "(bug exists + result is UNSAFE) OR (property is violated + violation is found)\">correct results</td>" \
                        "<td>"+ str(CORRECT_RESULTS)+"</td>" \
                        "<td>"+ str(CPACHECK_POSITIVE_cont) +"</td>" \
                        "<td>"+str(("%1.2f" % TIME_TOTAL_CORRECT))+"</td>" \
                      "</tr>" \
                      "<tr>" \
                        "<td title=\"bug exists + result is SAFE\">false negatives</td>" \
                        "<td>"+ str(FALSE_NEGATIVES)+"</td>" \
                        "<td class=\"score\">-</td>" \
                        "<td>"+str(("%1.2f" % TIME_TOTAL_FNEGATI))+"</td>" \
                      "</tr>" \
                      "<tr>" \
                        "<td title=\"no bug exists + result is UNSAFE\">false positives"+"</td>" \
                        "<td>"+str(FALSE_POSITIVES)+"</td>" \
                        "<td class=\"score\">-</td>" \
                        "<td>"+str(("%1.2f" % TIME_TOTAL_FPOSITI))+"</td>" \
                      "</tr>" \
                      "<tr>" \
                        "<td title=\"17 safe files, 15 unsafe files\">score ("+\
                        str(TOTAL_FILES)+" files, max score: "+str(MAX_SCORE)+")</td>" \
                        "<td class=\"score\">"+str(TOTAL_POINTS)+"</td>" \
                        "<td>"+str(CPACHECK_POSITIVE_cont+CPACHECK_NEGATIVE_cont)+"</td>" \
                        "<td>"+str(("%1.2f" % TOTAL_EXECUTION_TIME))+"&nbsp;</td>" \
                      "</tr>" \
                      "</tfoot> \n </table></center></body></html> \n"
   
    #html_report.write(HTML_TABLE_FOOTER)
    #list_to_write_in_html.append(HTML_TABLE_FOOTER)
    html_tmp_report = open(TMP_REPORT_FILE, "a+b")
    html_tmp_report.write(HTML_TABLE_FOOTER)                
    html_tmp_report.close()
    
    print()
    print("*** RESULTS *** ")
    print("Total Files: "+str(TOTAL_FILES)+" in "+str(TOTAL_EXECUTION_TIME)+" s")
    print("Total Memory (MB): "+str(("%1.2f" % TOTAL_MEM_IN_EXE)))
    print("Correct Results: "+str(CORRECT_RESULTS))
    print("False Negatives: "+str(FALSE_NEGATIVES))
    print("False Positives: "+str(FALSE_POSITIVES))
    print("Score ( "+str(TOTAL_FILES)+" files, max score: "+str(MAX_SCORE)+"): "+str(TOTAL_POINTS))

    #commands.getoutput("echo \""+HTML_TABLE_FOOTER+"\" >> "+OUTPUT_REPORT_FILE)
    print()
    try:
        model_report = open(PATH_HTML_MODEL_REPORT, "r")
        model_report_lines = model_report.readlines()
        model_report.close()        


        html_report = open(OUTPUT_REPORT_FILE, "w")
        for index, line in enumerate(model_report_lines):
            # line to insert the results
            if (index+1) == 1242:
                #for lineTable in list_to_write_in_html:
                html_report.write(open(TMP_REPORT_FILE,"rb").read())
            else:
                html_report.write(line)
                
        html_report.close()
        
        #os.remove(PATH_HTML_MODEL_REPORT)
        os.remove(TMP_REPORT_FILE)

        
    except IOError:
            print("Could not read file: report_model.html")
            sys.exit()

    
    print("Report file generated: "+OUTPUT_REPORT_FILE)
    remove_tmp_files(list_delete_tmp_file)
    
    


def remove_tmp_files(list_path):
    #return True
    for path in list_path:
        os.remove(path)





# -------------------------------------------------
# Main python program
# -------------------------------------------------

if __name__ == "__main__": 
    
    ############# Parse args options
    parser = argparse.ArgumentParser(description='Run experiment using Map2Check-FORTES v3')
    parser.add_argument(dest='dirCPrograms', metavar='program_directory', type=str, 
               help='the C program directory to be analyzed')    

    args = parser.parse_args()        
    
    ############# Check options in the args
    ### vars to save data option
    
    pathCPrograms=''
    
    if args.dirCPrograms:                 
        if not os.path.isdir(args.dirCPrograms): 
            print('Error: unable to open find the dir (%s)' % args.dirCPrograms)
            parser.parse_args(['-h'])
            sys.exit()
        else:
            pathCPrograms = os.path.abspath(args.dirCPrograms)
            set_codes_to_experiment(pathCPrograms)

    
