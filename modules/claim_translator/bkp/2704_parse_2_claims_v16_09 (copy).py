#!/usr/bin/env python

"""
Author: Herbert O. Rocha
Email: herberthb12@gmail.com
Year: 2014
Version: 2
Goal: Parse the claims proved by the ESBMC tool and then translate to
      new form, where this form is possible to executed in the C code
      (e.g., using FORTES and MAP2CHECK)

Usage way: ./parse_2_claims.py -i <file_list_claims.t> -c <C_Program>


[WARNNING]
TO DO: 
    1) NOT TRANSLATED:
        - 71 ; Claim 26 ; ?  file memsafety/test-0217_true.c line 71 function create_item?  dereference failure: memory model not applicable (got `void * [2]', expected `void * [2] *') ;   !(SAME-OBJECT(cursor, &(void * [2])(list[0][0])))
"""

from pyparsing import *
from itertools import islice
import sys
import re
import csv
import argparse



#---------------------------------------------------------------------

parser = argparse.ArgumentParser(description='Translator to Claims')
parser.add_argument('-i','--input', required=True, help='Input claims file')
parser.add_argument('-c','--code', required=True, help='Input C program')
parser.add_argument('-f','--list-function', required=True, help='Input function scope')
parser.add_argument('-o','--output', required=False, help='Output file')
parser.add_argument('-b','--bug-test', type=int, default=0, help='Number of test cases failed')
parser.add_argument("-d", "--debug-mode", type=int, choices=[0, 1], default=0, help="Enable the debug mode")
#parser.add_argument('-t','--trying', type=int, default=0, help='Only test translator - NO ABS input')


args = parser.parse_args()


#---------------------------------------------------------------------


"""
Flags to set specifics styles to asserts
"""
dicFlagIndentifyCL = {
    'NOT_CL'            : 0,
    'OR_CL'             : 0,            
    'overflow_CL'       : 0, # This is temporary (Work is ongoing)
    'invalidPointer_CL' : 0,
    'pointerOffset_CL'  : 0,
    'sameObject_CL'     : 0,
    'pointerObject_CL'  : 0,
    'dynamicSize_CL'    : 0,
    'return_f_CL'       : 0
}

#receives the translated claim
newClaimTranslated=""

#Variable to SET and print DEBUG values
DEBUG_PRINT=0
if args.debug_mode == 1:
    DEBUG_PRINT=1

#Variable to save temporary the name of the tmp VAR
TMP_VAR=""

#Variable to count the number of the tmp VAR
countTMPVAR=0


number_of_translated = 0
printFLag = 1
actual_nr_line_CL = 0

##################################################################
#Utils
BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE = range(8)

#following from Python cookbook, #475186
def has_colours(stream):
    if not hasattr(stream, "isatty"):
        return False
    if not stream.isatty():
        return False # auto color only on TTYs
    try:
        import curses
        curses.setupterm()
        return curses.tigetnum("colors") > 2
    except:
        # guess false in case of error
        return False
has_colours = has_colours(sys.stdout)


def printout(text, colour=WHITE):
        if has_colours:
                seq = "\x1b[1;%dm" % (30+colour) + text + "\x1b[0m"
                sys.stdout.write(seq)
        else:
                sys.stdout.write(text)
                
                
def isEmptyString(recString):
    if recString is not None and len(recString) > 0:
        return False
    else:
        return True

#End - Utils 
##################################################################               

##################################################################               
# Handling Claims
def checkScopeFunction(recFunctionName):
    #[TO DO]
    #true if is in the proper scope
    return 1
    
#set string with claim translated
def setNewClaim(claim):
    global newClaimTranslated
    newClaimTranslated = claim

#reset variable to set claim translated
def resetVarNewClaim():
    global newClaimTranslated
    newClaimTranslated = ""
    
def writeAssertion(recClaimTranslated):
    global TMP_VAR
    
    if not(isEmptyString(TMP_VAR)):
        print TMP_VAR
        TMP_VAR=""
    
    if DEBUG_PRINT == 1:
        print ">>>> "+str(printFLag)
    return ""+str(recClaimTranslated)+""

def checkReturnTranslate(str_assert_conc):
    global printFLag
    
    if printFLag == 1:
        return str_assert_conc
    else:
        return ""
        printFLag = 1


def translateClaim(rec_Parsed_CL,typeOfClaim):
    #global number_of_translated
    global printFLag
    str_assert_conc=""          
    
    """
    Identify what is the type of the claim and them apply its
    respectively rule of transformation
    """
    rec_listParsed_CL = rec_Parsed_CL.asList()
    rec_items = rec_Parsed_CL.items()
    rec_dictItems = rec_Parsed_CL.asDict()  

    #[BUG] nao esta sendo classificado o S&SNF --- LOC 221
        
    if len(typeOfClaim) > 0:
        if DEBUG_PRINT ==  1:
            for eachTClaim in typeOfClaim:          
                if "invalidPointer_CL" == eachTClaim:
                    print "IS a invalidPointer"
                if "pointerOffset_CL" == eachTClaim:
                    print "IS a pointerOffset"
                if "sameObject_CL" == eachTClaim:
                    print "IS a sameObject"
                if "pointerObject_CL" == eachTClaim:
                    print "IS a pointerObject"
                if "dynamicSize_CL" == eachTClaim:
                    print "IS a dynamicSize"
                if "is_dynamic_object_CL" == eachTClaim:
                    print "IS a is_dynamic_object"  
                if "return_f_CL" == eachTClaim:
                    print "IS a return function"                
                
        for each_token in rec_listParsed_CL:                                    
            str_assert_conc = str_assert_conc + each_token          
        setNewClaim( checkReturnTranslate(str_assert_conc) )
        
    else:               
        #Only for Simple or Simple Negation form
        #print "Simple or Simple Negation form"      
        for each_token in rec_listParsed_CL:
            str_assert_conc = str_assert_conc + each_token      
        setNewClaim( checkReturnTranslate(str_assert_conc) )
    
        
    

"""
Goal of this functions:
    Verify is the claim has only one function property
"""
def checkIsOneFuncProperty(recDicFlags):
    
    countFuncProperty=0     
    getResults=[] #receive 1 - TRUE; 2- name of the function
    
    for name in recDicFlags.keys():
        if not(name == "NOT_CL") and not(name == "OR_CL"):                      
            if recDicFlags[name] == 1:
                getResults.append(1)
                getResults.append(name)
                countFuncProperty = countFuncProperty + 1
            
    if countFuncProperty == 0 and len(getResults) == 0:
        """
        Simple Form or Simple Negation Form
        """
        return getResults
    elif countFuncProperty >= 1 and len(getResults) >= 2:
        """
        Complexy Form: only with one clause 
        """
        return getResults   
        

def handle_2_translate_CL(rec_parsed_claim_TMP,recResultCheckOne_TMP):

    if len(recResultCheckOne_TMP) == 0:
        """
        Simple Form and Simple Negation Form
        """        
        translateClaim( rec_parsed_claim_TMP , recResultCheckOne_TMP )
        
    elif len(recResultCheckOne_TMP) >= 1:
        """
        Complexy Form: only with one clause 
        """        
        translateClaim( rec_parsed_claim_TMP , recResultCheckOne_TMP )
            

def identify2MarkAssertsClaim(parsed_claim):
    
    #Get index position to split the grammar list, in this case
    #we adopt a list to use both to == 1 OR as to > 1 OR
    getPositionOr = [] 
    #count for OR condition
    countORCl   = 0
    rec_items   = parsed_claim.items()  
    
    if DEBUG_PRINT == 1:
        print "rec_items: "+str(rec_items)
    
    count_items=0           
    for item in rec_items:                                  
        if "NOT" in item[0]:
            dicFlagIndentifyCL['NOT_CL'] = 1
            #identify_NOT_CL=1          
        
        if "OR" in item[0]:
            dicFlagIndentifyCL['OR_CL'] = 1
            for eachItem in parsed_claim:
                if eachItem == "||":                    
                    getPositionOr.append(count_items)
                    countORCl = countORCl + 1
                count_items = count_items + 1
                        
        if "OVERFLOW" in item[0]:
            dicFlagIndentifyCL['overflow_CL'] = 1
        
        if "invalidPointer" in item[0]:
            dicFlagIndentifyCL['invalidPointer_CL'] = 1 
        
        if "pointerOffset" in item[0]:
            dicFlagIndentifyCL['pointerOffset_CL'] = 1
            
        if "sameObject" in item[0]:
            dicFlagIndentifyCL['sameObject_CL'] = 1 
            
        if "pointerObject" in item[0]:
            dicFlagIndentifyCL['pointerObject_CL'] = 1
            
        if "dynamicSize" in item[0]:
            dicFlagIndentifyCL['dynamicSize_CL'] = 1
            
        if "is_dynamic_object" in item[0]:
            dicFlagIndentifyCL['is_dynamic_object_CL'] = 1
            
        if "return_f" in item[0]:           
            dicFlagIndentifyCL['return_f_CL'] = 1           
        
    
    return getPositionOr
    
    
"""
"""
def translateToFortes(rec_parsed_claim):
    global dicFlagIndentifyCL
    global printFLag
        
    """
    1ST STEP - Identify and Mark the flags of the claims
    """
    rec_PositionOR = identify2MarkAssertsClaim(rec_parsed_claim)
        
    
    # This is temporary (Work is ongoing)
    if  dicFlagIndentifyCL['overflow_CL'] == 0:
        """
        2ST STEP - Identify if the claim is:
            (1) Simple form         : A expr B, where A and B are simple values
            (2) Simple Negation form: !(A expr B)
            (3) Complexy form       : X | !(X|Y); !(X expr Y); !(X || Y);
                                      !(X(K())); !(X expr Y || Y expr X)
                                      where X, Y and K are function properties, e.g., 
                                      POINTER_OFFSET
        """
        # Think about to remove this line below
        recResultCheckOne = checkIsOneFuncProperty( dicFlagIndentifyCL )
        #print str(recResultCheckOne)
        handle_2_translate_CL(rec_parsed_claim,recResultCheckOne)
        #handle_2_translate_CL(rec_parsed_claim)
        
            
    else:
        print "Overflow - Not supported yet!"       
        printFLag = 0
    
    #reset dictionary
    for item in dicFlagIndentifyCL:
        dicFlagIndentifyCL[item] = 0
    
# END Handling Claims
##################################################################               

"""
###################################################################
Utils functions for generic Parse Actions
###################################################################
"""
def removeQuotedArg_RLV(recString):     
    conc_str_token=""
    for token in recString:
        conc_str_token = conc_str_token+str(token)
    new_str = conc_str_token.replace('\'','')
    return new_str


"""
Utils functions for <refLocValue>
"""
def checkScopeAndReplace(recString):    
    global printFLag
    conc_str_token=""  
    
    #print "+++++++++++++"+args.list_function+" >> "+actual_nr_line_CL+" << "+recString[0]
    f = open(args.list_function)
    lines_data_function = f.readlines()
    f.close()
    
    #printFLag = 0
    
    flag_assert_OKAY = False
    for eachLine in lines_data_function:
        data_function = re.split(";",eachLine)        
        if data_function[0] == recString[0]:
            if actual_nr_line_CL >= data_function[1] and actual_nr_line_CL <= data_function[2]:
                #print "+++++++++++++"+args.list_function+" >> "+actual_nr_line_CL+" << "+recString[0]
                flag_assert_OKAY = True
    
    if flag_assert_OKAY:
        printFLag = 1        
    else:
        printFLag = 0
        
        

def getVarFromRefValue(recString):
    global printFLag
    conc_str_token="" 
    
    if printFLag == 1:
        for token in recString:
            conc_str_token = conc_str_token + str(token)
        
        get_result = re.split("::",conc_str_token)           
        #print "getVarFromRefValue "+str(printFLag)+" >> "+get_result[(len(get_result)-1)]
        return get_result[(len(get_result)-1)]
        
def triggerCheckKeyword(recString):
    global printFLag
    conc_str_token=""
    for token in recString:
        conc_str_token = conc_str_token + str(token)
        
    if conc_str_token in ["SAME","INVALID"]:
        printFLag = 0
    

def setAlwaysTrue( rec_tmp ):
    result = "1"
    return result
    
    
def mountString(rec_tokens):
    mount=""
    for token in rec_tokens:
        tmp = token.strip()
        mount = mount + tmp
    return mount
        
"""
Utils functions for <funPointOffset>
"""
def setDoubleRef_IP(recString):
    pre_str="(void *)&"
    pos_str="(void *)(intptr_t)"
    conc_str_token=""   
            
    for token in recString:
        conc_str_token = conc_str_token + str(token)
    
    new_str = pre_str + conc_str_token + ", " + pos_str + conc_str_token + " )"
    return new_str


def applyFuncIsPO(rec):
    pre_str="IS_POINTER_OBJECT_FORTES( list_LOG_mcf, (void *)&"
    pos_str="(void *)(intptr_t)"
    conc_str_token=""   
    
    for token in recString:
        conc_str_token = conc_str_token+str(token)
    
    new_str = pre_str + conc_str_token + ", " + pos_str + conc_str_token + " )"
    return new_str

"""
Utils functions for <IS_DYNAMIC_OBJECT>
"""
def applyFuncIsDynObj(recString):   
    pre_str="IS_DYNAMIC_OBJECT_FORTES( list_LOG_mcf, (void *)&"
    pos_str="(void *)(intptr_t)"
    conc_str_token=""   
    
    for token in recString:
        conc_str_token = conc_str_token+str(token)
    
    new_str = pre_str + conc_str_token + ", " + pos_str + conc_str_token + " )"
    return new_str
    
    
def addDerefenceSymb(recString):  
    pre_str="*"
    conc_str_token=""   
    
    for token in recString:
        conc_str_token = conc_str_token+str(token)
    
    new_str = pre_str + conc_str_token    
    return new_str
    
    
    
def addTypeCast_intptr(recString):
    pre_str="(intptr_t)"
    conc_str_token=""   
    
    for token in recString:
        conc_str_token = conc_str_token+str(token)
    
    new_str = pre_str + conc_str_token    
    return new_str
    


"""
Utils functions for <DYNAMIC_SIZE> AND <IS_DYNAMIC_OBJECT>
"""
def getDataLine(recString, idType):
    global actual_nr_line_CL    
    saveDynSize = ""
    
    f_code = open(args.code)
    lines = f_code.readlines()  
    f_code.close()
    
    
    #"""
    #Gathering the dynamic size from variable assigment
    #"""            
    #<DYNAMIC_SIZE>
    if idType == 0:             
        regex = recString[0]+"[ ]*=[ ]*.*?(?:malloc|calloc|realloc)[ ]*\((.*?)\)[ ]*"
        pattern = re.compile(regex, re.M|re.I)
        # BUG when the allocation is in other part of the code
        for line in lines:
            #print ">>>>>>"+lines[int(actual_nr_line_CL)-1]
            #matchObj = pattern.search(lines[int(actual_nr_line_CL)-1])
            matchObj = pattern.search(line)
            if matchObj:
                #print ">>>>>>> "+matchObj.group(1)+")"
                
                saveDynSize = "(intptr_t)("+recString[0]+") + ("+matchObj.group(1)+") )"
                return saveDynSize
            
    
    #<IS_DYNAMIC_OBJECT>
    if idType == 1:
        regex = "("+recString[0]+"\(.*?\))"     
        pattern = re.compile(regex, re.M|re.I)
        
        matchObj = pattern.search(lines[int(actual_nr_line_CL)-1])
        
    if matchObj:
        saveDynSize = matchObj.group(1)     
        
    return saveDynSize
    
    

"""
Utils functions for <DYNAMIC_SIZE>
""" 
def applyTransfDynSize(recString):
    idType = 0
    #call function to gather data from the specific line
    return getDataLine(recString, idType)
    
def applyTransRetFunc(recString):
    idType = 1
    #call function to gather data from the specific line    
    return getDataLine(recString, idType)

"""
Utils functions for <var invalid_object>
"""     
def setVarIOBChecker(recString):
    return "invalidObjectFortes"
    
"""
Utils functions for claims
"""
def setBlankSpace(recString):
    for token in recString:
        return " "+str(token)
        
        
def setWhiteSpaceToken( rec_token ):
    mountString=" " 
    for token in rec_token:     
        mountString = mountString + str(token)
    
    mountString = mountString + " "
        
    return mountString
    
    
def checkIfIsVoidCast( rec_token ):    
    
    for token in rec_token:     
        if token == "void":
            return ""
    

def set_cast_intptr_t( rec_token ):    
    mountString = "intptr_t"
    return mountString
    


# Warnning this should be improved
def replacePointerIndex( rec_token ):     
    mountString=""
    mountStringLvalue="" 
    mountStringRvalue="" 
    flagPoint = False
    flagLPointIndex = False    
    flagRPointIndex = False
    
    
    
    # Splitting the pointer refence eg. a+2 -> arr + i : output lpart = a+2 and rpart = arr + i    
    mountString = ''.join(rec_token)
    matchToken = re.search(r'->', mountString)    
    
    if matchToken:
        
        parts = mountString.rsplit("->")
        #lvalue = list(parts[0])
        
        matchTokenPI = re.search(r'[+]', parts[0])
        if matchTokenPI: 
            #print("---------------------------------- PART1")
            flagLPointIndex = True       
            # Update string lvalue    
            mountStringLvalue += "("+ str(parts[0]) +")"
        
        
        # Update string rvalue    
        rvalue = list(parts[1])
        matchTokenPI = re.search(r'[+]', parts[1])
        if matchTokenPI:
            #print("---------------------------------- PART2")
            flagRPointIndex = True
            lastIndexR = 0
            for index,token in enumerate(rvalue):
                if token == "+":
                    rvalue[index] = "["
                if token == ")":
                    lastIndexR = index
            rvalue.insert((lastIndexR), "]")  
            
            tmp = ''.join(rvalue)
            mountStringRvalue +=  tmp
            
        if flagLPointIndex and flagRPointIndex:
            mountString=""
            mountString = mountStringLvalue + "->" + mountStringRvalue
            return mountString
        elif flagLPointIndex and not flagRPointIndex:
            mountString=""
            mountString = mountStringLvalue + "->" + str(parts[1])
            return mountString
        elif not flagLPointIndex and flagRPointIndex:
            mountString=""
            mountString = str(parts[0]) + "->" + mountStringRvalue
            return mountString
 

    
"""
###################################################################
"""

"""
Begin BNF Grammar to claims
"""

"""
Goal:       Defening BNF grammar to Claims from ESBMC
Warning:    This grammar provide suport to Claims from ESBMC v1.18 to v1.21
e.g.:
    (1) j < 81
    (2) !(INVALID-POINTER(i + str)), !(INVALID-POINTER(job))
    (3) !((POINTER_OFFSET(str) + i) - 0 < 0) || !(SAME-OBJECT(str, &buffer[0]))
    (4) (_Bool)Climb_Inhibit => !overflow("+", Up_Separation, 100)
"""

opAddress       = Literal("&")
deferenceSymb   = Literal("*")
symbPoiterRef   = Literal("->") | Literal(".")
value           = Regex(r"([A-Za-z0-9\._\[\]]+)").setParseAction( triggerCheckKeyword ) ^ opAddress + Regex(r"([A-Za-z0-9\._\[\]]+)") ^\
                  Regex(r"([A-Za-z0-9\._\[\]]+)") + symbPoiterRef + Regex(r"([A-Za-z0-9\._\[\]]+)")
lParen          = Literal("(")
rParen          = Literal(")")
lBrace          = Literal("[")
rBrace          = Literal("]")
sinalToCalc     = Regex(r"([\+\-\*\<\>\!\=]+)")
startNegOp      = Literal("!").setResultsName("NOT")
condicionalOp   = Literal("?").setParseAction( setWhiteSpaceToken )
chooseOp        = Literal(":").setParseAction( setWhiteSpaceToken )
symbolOr        = Literal("||").setResultsName("OR")
comma           = Literal(",")
quotationMarks  = Literal("\"")
symbolRef       = Literal("::")
refNameFile     = Regex(r"([A-Za-z0-9\._\-]+)")
refNameFunct    = Regex(r"([A-Za-z0-9_]+)")
refValue        = Regex(r"([A-Za-z0-9_']+)").setParseAction( removeQuotedArg_RLV )
refID           = Regex(r"([0-9]+)")
invalidObject   = Literal("invalid_object").setParseAction( setVarIOBChecker ) + Suppress(Regex(r"([0-9]+)"))
boolSignal      = Keyword("_Bool")
boolOp          = lParen + boolSignal + rParen

signalTypeVar   = Keyword("unsigned") | Keyword("signed") 
sizeTypeVar     = Keyword("short") | Keyword("long")

"""
Type: int                   
Type: char
Type: float
Type: float
"""                     
typeVar         = Keyword("int") ^ signalTypeVar + Keyword("int") ^\
                  sizeTypeVar + Keyword("int") ^ signalTypeVar + sizeTypeVar + Keyword("int") ^\
                  Keyword("char") ^ signalTypeVar + Keyword("char") ^\
                  Keyword("float") ^\
                  Keyword("void") ^\
                  Keyword("_Bool").setParseAction( set_cast_intptr_t ) ^\
                  Keyword("double") ^ sizeTypeVar + Keyword("double") ^ (value | deferenceSymb + value)
                    

"""
(int)(arr[0]+j)
"""
toTypeCast      = (lParen + (deferenceSymb | Empty()) + typeVar + (deferenceSymb | Empty()) + rParen).setParseAction( checkIfIsVoidCast )

#---------------------------------------------------------------
"""
Goal:   Tests for toTypeCast
Status: PASSED
"""
#tests = """\
#(int) 
#(unsigned int)
#(signed int)
#(short int)
#(long int)
#(signed short int)
#(signed long int)
#(char)
#(long int)
#(double)
#""".splitlines()

#for lineClaim in tests:    
    #print (lineClaim, "->", toTypeCast.parseString( lineClaim ))
    #print ""
#---------------------------------------------------------------


"""
return_value_get_pat$1  
return_value__IO_getc$1 <-- [TO DO] create translation rules
"""
valueRF = value.copy() 
esbmcReturnFunc = Literal("return_value_").setResultsName("return_f").setParseAction(replaceWith("")) + valueRF.setParseAction( applyTransRetFunc ) + Suppress( Literal("$")  + Regex(r"([0-9]+)") ) |\
                  Literal("return_value__IO").setResultsName("return_f") + value + Suppress( Literal("$") ) + Regex(r"([0-9]+)")
                  


"""
c::schedule2::current_job
c::replace::patsize::pat
c::test-0217_true::is_empty::list
c::replace::main::1::pat - the index of the vector is a property
&c::replace::main::1::pat[0]
&c::test-0217_true::main::1::list[0]
&c::argv'[0]
&c::tot_info::f[0] 
"""
#refLocValue = Forward()
varRefLocValue  = refValue + (lBrace + refValue + rBrace ^ Empty())
refLocValue     = (opAddress | Empty()) + Suppress(Literal("c") + symbolRef) +\
                   ( varRefLocValue ^\
                     (refNameFile + symbolRef) + varRefLocValue ^\
                     (refNameFile + symbolRef) + (refNameFunct.setParseAction( checkScopeAndReplace ) + symbolRef) +\
                     ( refValue + (lBrace + refValue + rBrace ^ Empty()) ^\
                       (refID + symbolRef) + refValue + (lBrace + refValue + rBrace ^ Empty())
                     )\
                   ) 

                   #( Empty() ^  ) ^\
                   #( (refNameFile + symbolRef) + (refNameFunct.setParseAction( checkScopeAndReplace ) + symbolRef) ) +\
                   #( varRefLocValue | (refID + symbolRef) + (refValue + (lBrace + refValue + rBrace ^ Empty())) ) # end
                     
                     
                     
                  #+ (refNameFunct + symbolRef ^ refNameFunct + symbolRef + refID + symbolRef))) \
                  #+ (refValue + (lBrace + refValue + rBrace ^ Empty())) )

"""
invalid_object6 == delim
i * j
j >= 0
j > 80
i - k * y
(invalidObject | value) + OneOrMore(sinalToCalc + (invalidObject | value)) |\

DOING in : (_Bool)link => (int)term_field >= 0  TO -> 
"""
#toTypeCast + value + sinalToCalc #+ value 
#(Empty() ^ lParen) + (Empty() ^ toTypeCast) + (invalidObject ^ value ^ refLocValue ^ esbmcReturnFunc) + OneOrMore(sinalToCalc + (invalidObject ^ value ^ refLocValue ^ esbmcReturnFunc ^ invalidObject)) + (Empty() ^ rParen) |\
expression2Calc = (lParen | lParen + toTypeCast | toTypeCast) + (invalidObject ^ value ^ refLocValue ^ esbmcReturnFunc) + OneOrMore(sinalToCalc + (invalidObject ^ value ^ refLocValue ^ esbmcReturnFunc ^ invalidObject)) + rParen |\
                  sinalToCalc  + value + OneOrMore(sinalToCalc + value) |\
                  Suppress((toTypeCast + value + Literal("=>"))) + OneOrMore(toTypeCast + value | OneOrMore(sinalToCalc + (value | toTypeCast + value))) |\
                  lParen + sinalToCalc + value + OneOrMore(sinalToCalc + value) + rParen |\
                  value + OneOrMore(sinalToCalc + value)
                  
                  
                  
#---------------------------------------------------------------
"""
Goal:   Tests for esbmcReturnFunc and expression2Calc
Status: PASSED
"""
#---------------------------------------------------------------                 


                  
# Here I clone the rule <refLocValue> to apply parse action for POINTER OBJECT property
# Especial case
refLocValueFPO  = refLocValue.copy()


#---------------------------------------------------------------
"""
Goal:   Tests for refLocValue
Status: PASSED
"""
#tests = """\
#c::replace::patsize::pat
#&c::replace::main::1::pat[0]
#c::replace::main::1::pat
#&c::argv'[0]
#c::schedule2::current_job
#""".splitlines()
#---------------------------------------------------------------

"""                 
tmp$1
tmp$100
tmp$80
"""
varTmpEsbmc     = Literal("tmp$") + Regex(r"([0-9]+)")

#---------------------------------------------------------------
"""
Goal:   Tests for varTmpEsbmc
Status: PASSED
"""
#---------------------------------------------------------------
"""
Utils functions for <funPointOffset>
"""

        
"""
- POINTER_OFFSET(str)               
- POINTER_OFFSET(c::replace::patsize::pat)
- !((i * c + POINTER_OFFSET(c::tot_info::InfoTbl::f) + j) - 0 < 0)
""" 
funPointOffset = Forward()  #.setParseAction( addDerefenceSymb )
value_tmp = value.copy()
funPointOffset << (Keyword("POINTER_OFFSET").setParseAction(replaceWith(" ")).setResultsName("pointerOffset") + (lParen + value + rParen).setParseAction( addTypeCast_intptr ) ^\
                  Keyword("POINTER_OFFSET").setParseAction(replaceWith(" ")).setResultsName("pointerOffset") + Suppress(lParen + toTypeCast) + (value_tmp).setParseAction( addTypeCast_intptr ) + Suppress(rParen) ^\
                  Keyword("POINTER_OFFSET").setParseAction(replaceWith(" ")).setResultsName("pointerOffset") + Suppress(lParen + toTypeCast) + (expression2Calc).setParseAction( addTypeCast_intptr ) + Suppress(rParen) ^\
                  Keyword("POINTER_OFFSET").setParseAction(replaceWith(" ")).setResultsName("pointerOffset") + Suppress(lParen) + (refLocValue).setParseAction( addTypeCast_intptr ) + Suppress(rParen) ^\
                  lParen + value + OneOrMore(sinalToCalc + value ^ sinalToCalc + funPointOffset) + rParen + expression2Calc)


"""
---------------------------------------------------------------
Goal:   Tests for funPointOffset
Status: PASSED

Warning: Up to here isn't need to use <structArrayVar>
- &line[0]
- line[0]
- c::replace::main::1::pat[(POINTER_OFFSET(c::replace::patsize::pat) + n) - 0]

Note: IN IMPROVEMENT [DOING]
---------------------------------------------------------------
"""
structArrayVar  = opAddress + refValue + lBrace + refValue + rBrace  |\
                  refValue + lBrace + refValue + rBrace |\
                  (refLocValue | refValue) + lBrace + lParen + funPointOffset + ZeroOrMore(sinalToCalc + refValue) + rParen + ZeroOrMore(sinalToCalc + refValue) + rBrace
                  
"""
&line                 
"""
simplePointRef = (opAddress + refValue).setParseAction( mountString )

#---------------------------------------------------------------
"""
Goal:   Tests for structArrayVar
Status: PASSED
"""
#---------------------------------------------------------------

"""
- (int)(c::replace::main::1::pat[(POINTER_OFFSET(c::replace::patsize::pat) + n) - 0]) == 33
"""
exprUseArrayVar = toTypeCast + lParen + structArrayVar + rParen + sinalToCalc + value

#---------------------------------------------------------------
"""
Goal:   Tests for exprUseArrayVar
Status: PASSED
"""
#---------------------------------------------------------------

"""
INVALID-POINTER(job) |
INVALID-POINTER(i + str) -> (A + B)* | (n + c::replace::patsize::pat)
INVALID-POINTER(invalid_object7 + (int)(buffer[0]))) 
                                (int)(buffer[(POINTER_OFFSET(str) + i) - 0])
Warning: the last rule has only the name of the  function                                                               
"""
expression2Calc_IP = expression2Calc.copy()
funInvalidPoint = Keyword("INVALID-POINTER").setParseAction(replaceWith("IS_VALID_POINTER_FORTES( list_LOG_mcf, ")).setResultsName("invalidPointer") + (lParen + invalidObject + rParen ^lParen + value + rParen ^ expression2Calc_IP).setParseAction( setDoubleRef_IP ) ^\
                  Keyword("INVALID-POINTER").setParseAction(replaceWith("IS_VALID_POINTER_FORTES( list_LOG_mcf, ")).setResultsName("invalidPointer") + (lParen + toTypeCast + value + rParen ^ expression2Calc_IP.setParseAction( replacePointerIndex )).setParseAction( setDoubleRef_IP ) ^\
                  Keyword("INVALID-POINTER").setParseAction(replaceWith("IS_VALID_POINTER_FORTES( list_LOG_mcf,")).setResultsName("invalidPointer") + (lParen + invalidObject + sinalToCalc + toTypeCast + lParen + structArrayVar + rParen + rParen).setParseAction( setDoubleRef_IP ) ^\
                  Keyword("INVALID-POINTER").setParseAction(replaceWith("IS_VALID_POINTER_FORTES( list_LOG_mcf, ")).setResultsName("invalidPointer") + (lParen + ( Empty() ^ Suppress(toTypeCast) ) + value + rParen).setParseAction( setDoubleRef_IP ) ^\
                  Keyword("INVALID-POINTER").setParseAction(replaceWith("IS_VALID_POINTER_FORTES( list_LOG_mcf, ")).setResultsName("invalidPointer") 
                  

                  
#---------------------------------------------------------------
"""
Goal:   Tests for funInvalidPoint
Status: PASSED
"""
#---------------------------------------------------------------

"""
SAME-OBJECT(lin, &line[0]))
SAME-OBJECT(c::replace::patsize::pat, &line[0])
SAME-OBJECT(c::tcas::main::argv, &c::argv'[0])      
SAME-OBJECT(prio_queue[0].head, NULL)

Warning: Possible related to supress of the ")"
"""             
#Keyword("SAME-OBJECT").setResultsName("sameObject").setParseAction(replaceWith("")) + Suppress(lParen) + refLocValue.setParseAction( getVarFromRefValue ) + comma.setParseAction(replaceWith(" == ")) + refLocValue.setParseAction( getVarFromRefValue ) + Suppress(rParen)
funSameObject   = Keyword("SAME-OBJECT").setResultsName("sameObject").setParseAction(replaceWith("")) + Suppress(lParen) + value + comma.setParseAction(replaceWith(" == ")) + (structArrayVar | simplePointRef) + Suppress(rParen) ^\
                  Keyword("SAME-OBJECT").setResultsName("sameObject").setParseAction(replaceWith("")) + Suppress(lParen) + refLocValue.setParseAction( getVarFromRefValue ) + comma.setParseAction(replaceWith(" == ")) + (structArrayVar | simplePointRef) + Suppress(rParen) ^\
                  Keyword("SAME-OBJECT").setResultsName("sameObject").setParseAction(replaceWith("")) + Suppress(lParen) + refLocValue.setParseAction( getVarFromRefValue ) + comma.setParseAction(replaceWith(" == ")) + refLocValue.setParseAction( getVarFromRefValue ) + Suppress(rParen) ^\
                  Keyword("SAME-OBJECT").setResultsName("sameObject").setParseAction(replaceWith("")) + Suppress(lParen) + value + comma.setParseAction(replaceWith(" == ")) + value + Suppress(rParen)

#---------------------------------------------------------------
"""
Goal:   Tests for funSameObject
Status: PASSED
"""
#---------------------------------------------------------------

"""
POINTER_OBJECT(c::schedule2::current_job)
POINTER_OBJECT(next)
# [TO DO] put on this CLAIM in the test cases
POINTER_OBJECT(SAME-OBJECT(c::schedule2::get_process::job, &c::schedule2::unblock::1::job) ? c::schedule2::unblock::1::job : SAME-OBJECT(c::schedule2::get_process::job, &c::schedule2::upgrade_prio::1::job)                   
"""                 
funPointObject  = Keyword("POINTER_OBJECT").setResultsName("pointerObject").setParseAction(replaceWith("IS_VALID_POINTER_FORTES( list_LOG_mcf, ")) + (lParen + refLocValue.setParseAction( getVarFromRefValue ) + rParen).setParseAction( setDoubleRef_IP ) ^\
                  Keyword("POINTER_OBJECT").setResultsName("pointerObject").setParseAction(replaceWith("IS_VALID_POINTER_FORTES( list_LOG_mcf, ")) + (lParen + value + rParen).setParseAction( setDoubleRef_IP ) 
                  

#---------------------------------------------------------------
"""
Goal:   Tests for funPointObject
Status: PASSED
"""
#---------------------------------------------------------------

""" 
DYNAMIC_SIZE(xj)
"""
value_DS = value.copy() 
funDynamicSize  = Keyword("DYNAMIC_SIZE").setResultsName("dynamicSize").setParseAction(replaceWith("")) + lParen + value_DS.setParseAction(applyTransfDynSize) + rParen


""" 
v1.18 IS_DYNAMIC_OBJECT(xj)
""" 
value_DO = value.copy() 
funOIsDynamic   = Keyword("IS_DYNAMIC_OBJECT").setResultsName("is_dynamic_object").setParseAction(replaceWith(" ")) + lParen + value_DO.setParseAction(applyFuncIsDynObj) + rParen ^\
                  Keyword("IS_DYNAMIC_OBJECT").setResultsName("is_dynamic_object").setParseAction(replaceWith(" ")) + lParen + expression2Calc.setParseAction(applyFuncIsDynObj) + rParen
                  


"""                                     
v1.21 __ESBMC_is_dynamic[POINTER_OBJECT(job)]
In improvement
""" 
funNIsDynamic   = Keyword("__ESBMC_is_dynamic") + lBrace + funPointObject + rBrace

#---------------------------------------------------------------
"""
Goal:   Tests for funNIsDynamic
Status: PASSED
"""
#---------------------------------------------------------------

""" 
VALID_OBJECT(job)
"""                     
funValidObject  = Keyword("VALID_OBJECT").setParseAction(replaceWith("IS_VALID_OBJECT_FORTES( list_LOG_mcf, ")).setResultsName("invalidPointer") + (lParen + value + rParen).setParseAction( setDoubleRef_IP )


"""
Warning: the second possibility it was greedy
"""
firstOpFuncOver = quotationMarks + sinalToCalc + quotationMarks |\
                  quotationMarks + value + ZeroOrMore(Literal("-")) + ZeroOrMore(value) + quotationMarks
                    

"""
!overflow("unary-", 1)
!overflow("typecast-8", return_value_getpat$1)
!overflow("+", i, 1)  or !overflow("+", totdf, infodf) or
    !overflow("+", j, return_value_patsize$2)
!overflow("+", i * c, j)
!overflow("+", (int)(*(1 + n + pat)), 2)
"""
funOverflow     = Keyword("overflow").setResultsName("OVERFLOW") + lParen + firstOpFuncOver + comma + esbmcReturnFunc + rParen |\
                  Keyword("overflow").setResultsName("OVERFLOW") + lParen + firstOpFuncOver + comma + expression2Calc + comma + value + rParen |\
                  Keyword("overflow").setResultsName("OVERFLOW") + lParen + firstOpFuncOver + comma + ZeroOrMore(esbmcReturnFunc) + ZeroOrMore(value) + comma + ZeroOrMore(esbmcReturnFunc) + ZeroOrMore(value) + rParen |\
                  Keyword("overflow").setResultsName("OVERFLOW") + lParen + firstOpFuncOver + comma + toTypeCast + lParen + deferenceSymb + lParen + expression2Calc + rParen + rParen + comma + value + rParen |\
                  Keyword("overflow").setResultsName("OVERFLOW") + lParen + firstOpFuncOver + comma + value + rParen


#---------------------------------------------------------------
"""
Goal:   Tests for funOverflow
Status: PASSED
"""
#---------------------------------------------------------------

"""
-------------------------------- Claims
"""
mathFuncClaims =  expression2Calc | exprUseArrayVar

# { | } OR { ^ } meaning "match first alternative" or "match longest alternative"
allFuncClaims   = funPointOffset 
#| funInvalidPoint | funSameObject |\
                  #funPointObject | funDynamicSize | funOIsDynamic | funNIsDynamic |\
                  #funValidObject 




values2CondOp = (value ^ refLocValue.setParseAction( getVarFromRefValue ) ^ allFuncClaims)
condCompOp = Forward()
condCompOp << (values2CondOp + condicionalOp + values2CondOp + chooseOp + condCompOp ^ values2CondOp)

"""
Rules:

<funcClaim> + i
(<funcClaim> + i) - 0 >= 81
Warning: Generic clause
(<funcClaim> "?" [value | refLocValue] chooseOp [value | refLocValue])  
Warning: Generic clause                 
((<funcClaim> "?" [value | refLocValue] chooseOp [value | refLocValue]) sinalToCalc <funcClaim>) (sinalToCalc [value | refLocValue])*
4st -> 
!(INVALID-POINTER((SAME-OBJECT(c::replace::omatch::i, &offset) ? offset : c::replace::amatch::1::i) + lin))
5st ->
!(((SAME-OBJECT(c::replace::addstr::j, &c::replace::makesub::1::j) ? c::replace::makesub::1::j : SAME-OBJECT(c::replace::addstr::j, &c::replace::makepat::1::j) ? c::replace::makepat::1::j : jt) + POINTER_OFFSET(outset)) - 0 < 0)

!(POINTER_OFFSET(xi) + i >= DYNAMIC_SIZE(xi))
"""             
complCalOpClaim =   allFuncClaims + sinalToCalc + value |\
                    lParen + lParen + allFuncClaims + sinalToCalc + value + rParen + expression2Calc + rParen |\
                    lParen + allFuncClaims + value + rParen + expression2Calc |\
                    lParen + funInvalidPoint + ( lParen + lParen + condCompOp + rParen + sinalToCalc + (value ^ refLocValue.setParseAction( getVarFromRefValue )) + rParen).setParseAction( setDoubleRef_IP ) + rParen |\
                    lParen + lParen + lParen + condCompOp + rParen + sinalToCalc + funPointOffset + rParen + expression2Calc + rParen |\
                    lParen + funPointOffset + sinalToCalc + value + sinalToCalc + funDynamicSize + rParen |\
                    lParen + funPointOffset + sinalToCalc + value + sinalToCalc + value + rParen |\
                    lParen + sinalToCalc + value + sinalToCalc + funPointOffset + sinalToCalc + (funDynamicSize | value) + rParen |\
                    lParen + funPointOffset + sinalToCalc + funDynamicSize + rParen |\
                    lParen + funPointOffset + sinalToCalc + value + rParen
                    
                    
                    
                    

#---------------------------------------------------------------
"""
Goal:   Tests for complCalOpClaim
Status: PASSED
"""
#---------------------------------------------------------------                    
                                
                    
"""
<funcClaim> (<funcClaim>)   
Rules:
1st -       
2st -   
3st -   
"""   

# BUG -> complCalOpClaim
  
# Correct way -> startNegOp + lParen + allFuncClaims + rParen |\
opBaseFuncClaim = startNegOp + complCalOpClaim


#( (startNegOp | Empty()) + varTmpEsbmc).setParseAction( setAlwaysTrue ) |\
                  #startNegOp + funOverflow |\
                  #startNegOp + lParen + allFuncClaims + rParen  |\
                  #startNegOp + complCalOpClaim |\
                  #funValidObject |\
                  #startNegOp + mathFuncClaims |\
                  #lParen + mathFuncClaims + rParen |\
                  #mathFuncClaims  
                  
"""
The Claims and its possible forms
- j < 81
- Functions
- <expr> (|| <expr>)+
- !((POINTER_OFFSET(str)+i) - 0 >= 81) || !((POINTER_OFFSET(str) + i) - 0 >= 81)    
                        || !(SAME-OBJECT(str, &buffer[0]))
                        || (int)(c::replace::main::1::pat[(POINTER_OFFSET(c::replace::patsize::pat) + n) - 0]) == 33
Greedy                                          
- POINTER_OFFSET(next) - POINTER_OFFSET(&prio_queue[0].head) == 0                       
"""

operationClaim  = opBaseFuncClaim |\
                  funPointOffset  + sinalToCalc + funPointOffset + sinalToCalc.setParseAction( setBlankSpace ) + value 

#---------------------------------------------------------------
########################################################################


# -------------------------------------------------
# Main python program
# -------------------------------------------------

if __name__ == "__main__":    


    # CVS file to update
    claims_csv_file = open(args.input)
    claims_lines_csv = claims_csv_file.readlines()
    claims_csv_file.close()

    """
    Variables gathing from input file
    number line ; name/ID ; comments ; property
    """
    list_lines_CL       = []
    list_name_CL        = []
    list_comment_CL     = []
    list_property_CL    = []


    
    for lineClaim in claims_lines_csv:
        
        tmp = re.split(";",lineClaim)
        
        tmp[0] = tmp[0].replace(" ","")
        tmp[1] = tmp[1].replace("^\s+","")
        tmp[2] = tmp[2].replace("^\s+","")            
        tmp[3] = tmp[3].replace("^\s+","")
        
        list_lines_CL.append(tmp[0])
        list_name_CL.append(tmp[1])
        list_comment_CL.append(tmp[2])
        list_property_CL.append(tmp[3])
            
    #else:        
        #list_property_CL = claims_lines_csv

    count_tests=0

    for lineClaim in list_property_CL:
        #if args.trying == 0:
        actual_nr_line_CL = list_lines_CL[count_tests]        
        
        if args.debug_mode:            
            
            print "#############################################################"
            printout("[debug]   Claim: "+str(lineClaim), GREEN)
            print ""    
            try:
                """
                Verify if (1) the claim has more than one assertion and then (2) split this one
                """
                #(1) the claim has more than one assertion
                match_OR = re.search(r'\|\|',lineClaim)
                if match_OR:
                    """
                    Complexy Form: 
                        - Only more than one clause, but WITH choose condition
                    Warning:
                        - Generic way both to ==1 and >1 OR in the claim            
                    """
                    
                    if DEBUG_PRINT == 1:
                        print "----> OR"
                        
                    get_split_OR = re.split("\|\|", lineClaim)  
                    recNewClaimStrans=""
                    
                    for each_part_CL in get_split_OR:               
                        #remove inicial whitespace
                        neach_part_CL = each_part_CL.strip()     
                        
                        # The parse moment           
                        parsed_claim = operationClaim.parseString( neach_part_CL )  
                        
                        if DEBUG_PRINT == 1:
                            print neach_part_CL
                            printout(str(parsed_claim.asList()), BLUE)
                            print ""
                        
                        """
                        Unificar saidas e entao montar a assertiva
                        """
                        translateToFortes( parsed_claim )
                        
                        if  recNewClaimStrans == "":
                            recNewClaimStrans = newClaimTranslated                  
                        else:
                            recNewClaimStrans = recNewClaimStrans +" || "+ newClaimTranslated
                        
                    
                    writeAssertion( recNewClaimStrans )
                    number_of_translated = number_of_translated + 1
                            
                        
                else:                   
                    parsed_claim = operationClaim.parseString( lineClaim )      
                    
                    if DEBUG_PRINT == 1:
                        printout(str(parsed_claim.asList()), BLUE)
                        print ""                    
                        
                    """
                    Translate the Claims
                    """         
                    print "----------------------------------------------------------"
                    printout("The Claim has been translate to: ", GREEN)
                    #call function to translate 
                    print "\n"          
                    translateToFortes( parsed_claim )
                    
                    if printFLag == 1:
                        writeAssertion( newClaimTranslated )
                    else:
                        printFLag = 1
                        
                    number_of_translated = number_of_translated + 1
                    print ""                    
                        
            ##--------------------Improve this
            except ParseBaseException, pe:
                printout(str(pe.msg), RED)
                print ""
                printout(str(pe.pstr), RED)
                print ""            
                print " "*pe.loc+"^"
                    
            
            print "#############################################################"
        
            print "============================================================="
            #if args.bug_test > 0 :
                #Because the test cases have a false test
            #    number_of_translated = number_of_translated + 1
            #print "LOG:"
            #print "Number of claims to test    : "+str(count_tests)
            #print "Number of claims translated : "+str(number_of_translated)+" i.e., "+str(number_of_translated*100/count_tests)+"%"
            #print "============================================================="
        
        else:
            
            try:
                """
                Verify if (1) the claim has more than one assertion and then (2) split this one
                """
                #(1) the claim has more than one assertion
                match_OR = re.search(r'\|\|',lineClaim)
                if match_OR:
                    """
                    Complexy Form: 
                        - Only more than one clause, but WITH choose condition
                    Warning:
                        - Generic way both to == 1 and > 1 OR in the claim
                    """                        
                    get_split_OR = re.split("\|\|", lineClaim)
                    recNewClaimStrans=""
                    
                    for each_part_CL in get_split_OR:
                        #remove inicial whitespace
                        neach_part_CL = each_part_CL.strip()
                        parsed_claim = operationClaim.parseString( neach_part_CL )                        
                        
                        """
                        Unificar saidas e entao montar a assertiva
                        """
                        translateToFortes( parsed_claim )
                        
                        if  recNewClaimStrans == "":
                            recNewClaimStrans = newClaimTranslated
                        else:
                            recNewClaimStrans = recNewClaimStrans +" || "+ newClaimTranslated
                                        
                    # Claim translated [OKAY]
                    new_property = writeAssertion( recNewClaimStrans ) 
                    
                    # Write new line of the CSV file with claims translated                    
                    print list_lines_CL[count_tests]+" ; "+list_name_CL[count_tests]+" ; "+list_comment_CL[count_tests]+" ; "+new_property
                    
                else:
                    
                    parsed_claim = operationClaim.parseString( lineClaim )                        
                    """
                    Translate the Claims
                    """                    
                    # Call function to translate
                    
                    translateToFortes( parsed_claim )
                    
                    if printFLag == 1:
                        # Claim translated [OKAY]
                        new_property = writeAssertion( newClaimTranslated )
                        # Write new line of the CSV file with claims translated                    
                        print list_lines_CL[count_tests]+" ; "+list_name_CL[count_tests]+" ; "+list_comment_CL[count_tests]+" ; "+new_property
                
                    else:
                        printFLag = 1                        
                    
                    #print ""
                        
            ##--------------------Improve this
            except ParseBaseException, pe:
                
                # Improve this, but at this time
                # we just translate the claim to assert(1)                
                print list_lines_CL[count_tests]+" ; "+list_name_CL[count_tests]+" ; "+list_comment_CL[count_tests]+" ; (1)"
                
        
        count_tests = count_tests + 1
    #---------------------------------------------------------------
