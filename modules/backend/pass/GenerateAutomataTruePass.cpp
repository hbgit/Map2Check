#include "GenerateAutomataTruePass.h"

const char* list_autotr_file = "list_true_automata.csv";

bool GenerateAutomataTruePass::runOnFunction(Function &F) {
    this->Ctx = &F.getContext();
    this->currentFunction = &F;  

    //errs() << "========== Function: " << this->currentFunction->getName() << "\n";
    this->st_currentFunctionName = this->currentFunction->getName();

    for(auto& B: F)
    {

        //BasicBlock basicB = i;  
        this->runOnBasicBlock(B, this->Ctx); 
        this->printStateData();

    }

    return true;
}

/**
void GenerateAutomataTruePass::resetStateData()
{
    st_lastBlockInst=;  
    std::string st_currentFunctionName;
    int st_numLineEntryPoint;
    int st_startline;
    int st_numLine2NextSt;
    std::string st_sourceCodeLine;
    std::string st_controlCode;
    bool st_isEntryPoint = false;

}**/


void GenerateAutomataTruePass::printStateData()
{
    if(this->enableDataBlk)
    {  
        ofstream filest;
        filest.open("automata_list_log.csv", std::ios_base::app);
        filest << this->st_currentFunctionName << "@";
        filest << this->st_startline << "@";
        filest << this->st_sourceCodeLine << "@";
        if(this->st_isControl){
            filest << this->st_controlCode << "@1@";
        }else{
            filest << "NONE@0@";
        }
        filest << this->st_isEntryPoint << "\n";
        filest.close();
    }

    //To write the automata the flow should be using the function name and its call
}


void GenerateAutomataTruePass::runOnBasicBlock(BasicBlock& B, LLVMContext* Ctx) 
{
    //B.dump();    
    //Instruction I = dyn_cast<Instruction>(&B.end());
    // Get the first and the last instruction of the BasicBlock 
    // Check if the Block has size greater then 1, otherwise the last instruction 
    // will be the first one 

    //BasicBlock::iterator instFromBB;
    this->firstBlockInst = B.begin();
    this->st_lastBlockInst = --B.end(); // -- is necessary to avoid the pointer to the next block
    this->enableDataBlk = false;
    bool isCond = false;
    

    //Identifying asserts on analayzed code
    this->identifyAssertLoc(B);   

    if(!this->checkBBHasLError(B))
    {

        // Create a method to get info from branches in the block for condition-true and false     
        isCond = this->isBranchCond(B);
        this->st_isControl = isCond;

        if(B.size() > 1)
        {
            //errs() << "First InstBB: " << this->firstBlockInst->getOpcodeName() << "\n";
            //DebugInfo debugInfoFi(this->Ctx, (Instruction*)this->firstBlockInst);
            //errs() << this->sourceCodeHelper->getLine(debugInfoFi.getLineNumberInt()) << "\n";

            //Checking if the last instruction in the basic block is a br instruction,i.e., 
            //this br instrucion 
            //Identify if the last line of the file is a valid code to witness
            if(auto* tI = dyn_cast<TerminatorInst>(&*this->st_lastBlockInst))
            {
                if(tI->getOpcodeName() == "br")
                {
                    --this->st_lastBlockInst;
                    //errs() << "Last  InstBB: " << this->lastBlockInst->getOpcodeName() << "\n";                
                    DebugInfo debugInfoLa(this->Ctx, (Instruction*)this->st_lastBlockInst);
                    //errs() << "startline  1: " << debugInfoLa.getLineNumberInt() << "\n"; 
                    this->st_startline = debugInfoLa.getLineNumberInt();
                    //errs() << "sourcecode 1: " << this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt()) << "\n";
                    this->st_sourceCodeLine = this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt());


                }else{
                    //errs() << "Last  InstBB: " << this->lastBlockInst->getOpcodeName() << "\n";                
                    //errs() << this->lastBlockInst->getOpcodeName() << "\n";
                    //empty line in the source code
                    DebugInfo debugInfoLa(this->Ctx, (Instruction*)this->st_lastBlockInst);

                    if(this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt()).empty())
                    {
                        //errs() << "Empty \n" ;
                        int numline = debugInfoLa.getLineNumberInt() - 1;
                        //errs() << "startline  2: " << numline << "\n";
                        this->st_startline = numline;
                        //errs() << "startline  2: " << debugInfoLa.getLineNumberInt() << "\n"; 
                        //errs() << "sourcecode 2: " << this->sourceCodeHelper->getLine(numline) << "\n";
                        this->st_sourceCodeLine = this->sourceCodeHelper->getLine(numline);

                    }else{
                        //errs() << "startline  2: " << debugInfoLa.getLineNumberInt() << "\n"; 
                        this->st_startline = debugInfoLa.getLineNumberInt();
                        //errs() << "sourcecode 2: " << this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt()) << "\n";
                        this->st_sourceCodeLine = this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt());
                                                
                    }

                }
            }
            this->enableDataBlk = true;

        }else{
            //In case this unique instruction be a "br" then we remove this basic block 
            if(auto* tI = dyn_cast<TerminatorInst>(&*this->st_lastBlockInst))
            {
                if(tI->getOpcodeName() != "br")
                {
                    //errs() << "First and Last InstBB: "<< this->firstBlockInst->getOpcodeName() << "\n";        
                    this->st_lastBlockInst = this->firstBlockInst;
                    DebugInfo debugInfoLa(this->Ctx, (Instruction*)this->st_lastBlockInst);
                    //errs() << "startline  3: " << debugInfoLa.getLineNumberInt() << "\n"; 
                    this->st_startline = debugInfoLa.getLineNumberInt();
                    //errs() << "sourcecode 3: " << this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt()) << "\n";
                    this->st_sourceCodeLine = this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt());
                    this->enableDataBlk = true;
                }
            }

        }
    }

    if(this->enableDataBlk)
    {
        //this->visitInstruction(i);
        if(this->currentFunction->getName() == "main") 
        {
            if(this->countEntryPoint == 1 && this->st_isEntryPoint == false)
            {
                this->st_isEntryPoint = true;
                this->countEntryPoint++;
            }else{
                this->st_isEntryPoint = false;
            }                        
        }
        //DebugInfo debugInfo(Ctx, (Instruction*)this->st_lastBlockInst);
        //errs() << "Line number to the next block: " << *debugInfo.getLineNumberValue() <<"\n";
        //errs() << ">>>Line number to the next block: " << debugInfo.getLineNumberInt() <<"\n";
        //this->st_numLine2NextSt = debugInfo.getLineNumberInt();
        //std::cout << "---This is a block divider--- block with: " << B.size() << " instructions" << std::endl;    
        //errs() << "---------------------- \n";
    }
}

void GenerateAutomataTruePass::identifyAssertLoc(BasicBlock& B)
{
    for( auto& I : B )
    {
        if(auto* cI = dyn_cast<CallInst>(&I))
        {
            //errs() << cI->getCalledValue()->getName() << "\n";
            if(cI->getCalledValue()->getName() == "__VERIFIER_assert")
            {
                DebugInfo debugInfoCi(this->Ctx, cI);
                //errs() << debugInfoCi.getLineNumberInt() << "\n";
                this->assertListLoc.push_back(debugInfoCi.getLineNumberInt());
            }

        }        
    }
}

// Identify if the block has a branch and define the condition to true and false
bool GenerateAutomataTruePass::isBranchCond(BasicBlock& B)
{
    //errs() << bbName << "\n";
    StringRef actualNameInt;
    std::vector<int>::const_iterator iT;

    for( auto& I : B )
    {      

        if(auto* bI = dyn_cast<ICmpInst>(&I))
        {
            //errs() << this->convertLLPredicatetoXmlText(I) << "\n";
            //
            DebugInfo debugInfoBi(this->Ctx, bI);
            //errs() << debugInfoBi.getLineNumberInt() << "\n";

            //Skiping when the Branch was an assertion
            iT =  std::find(this->assertListLoc.begin(), this->assertListLoc.end(), debugInfoBi.getLineNumberInt()); 
            if ( !(iT != this->assertListLoc.end()) )
            {
                //errs() << "control : " << this->convertLLPredicatetoXmlText(I) << "\n";
                this->st_controlCode = this->convertLLPredicatetoXmlText(I);
                return true;
            }
            /**else{
                //errs() << this->cprogram_path << "\n";
                errs() << debugInfoBi.getLineNumberInt() << "\n";
                errs() << this->sourceCodeHelper->getLine(debugInfoBi.getLineNumberInt()) << "\n";
            }**/
        }


        /**if(auto* tI = dyn_cast<TerminatorInst>(&I))
          {
          if(tI->getOpcodeName() == "br")
          {
        //errs() << tI->getSuccessor(1)->getValueName() << "\n";
        //errs() << tI->successors().begin()->getValueName() << "\n";

        errs() << tI->getOpcodeName() << ":::::::::::::::: \n";
        tI->dump();
        if(tI->getNumSuccessors() > 1)
        {
        BasicBlock::iterator trueCond = tI->getSuccessor(0)->begin();
        DebugInfo debugInfoT(this->Ctx, (Instruction*)trueCond);
        errs() << "True Cond in:" << debugInfoT.getLineNumberInt() << "\n";

        BasicBlock::iterator falseCond = tI->getSuccessor(1)->begin();
        DebugInfo debugInfoF(this->Ctx, (Instruction*)falseCond);
        errs() << "False Cond in:" << debugInfoF.getLineNumberInt() << "\n";

        }

        }
        }**/

        //errs() << I.getOpcodeName() << "\n";
    }

    return false;
}


std::string GenerateAutomataTruePass::convertLLPredicatetoXmlText(Instruction& I)
{
    std::string lvaluep;
    std::string rvaluep;
    std::string predicateInXml;
    std::string fullExpPredicateInXml;
    std::ostringstream osstrtmp;

    if(auto* bI = dyn_cast<ICmpInst>(&I))
    {

        //bI->dump();
        //errs() << bI->getSignedPredicate() << "\n";
        //errs() << this->getPredicateSymOnXmlText(*bI) << "\n";
        predicateInXml = this->getPredicateSymOnXmlText(*bI);
        //errs() << *bI->getOperand(0) << "\n";
        //errs() << bI->getOperand(0)->getValueName() << "\n";

        //Getting left value from predicate 
        if(isa<LoadInst>(bI->getOperand(0))) {
            LoadInst *LD100 = cast<LoadInst>(bI->getOperand(0));
            Value *C100 = LD100->getPointerOperand();
            lvaluep = C100->getName().str(); 
            //errs() << "LVALUEP: " << lvaluep << "\n";

        } else if (ConstantInt* CI = dyn_cast<ConstantInt>(bI->getOperand(0))) {
            if (CI->getBitWidth() <= 32) { //Of course, you can also change it to <= 64 if constIntValue is a 64-bit integer, etc.
                //constIntValue = CI->getSExtValue();
                osstrtmp << CI->getSExtValue();
                rvaluep = osstrtmp.str();
                //errs() << "LVALUEP:" << lvaluep << "\n";
            }
        }


        //Instruction tmpI = (Instruction) &*bI->getOperand(0);
        //errs() << *bI->getOperand(1) << "\n";

        if(isa<LoadInst>(bI->getOperand(1))) {
            LoadInst *LD101 = cast<LoadInst>(bI->getOperand(1));
            Value *C101 = LD101->getPointerOperand(); //HERE COMPILATION ERROR
            rvaluep = C101->getName().str();
            //errs() << "RVALUEP:" << rvaluep << "\n";
        }
        //errs() << bI->getOperand(1)->getName().str() << "\n";
        else if (ConstantInt* CI = dyn_cast<ConstantInt>(bI->getOperand(1))) {
            if (CI->getBitWidth() <= 32) { //Of course, you can also change it to <= 64 if constIntValue is a 64-bit integer, etc.
                //constIntValue = CI->getSExtValue();
                osstrtmp << CI->getSExtValue();
                rvaluep = osstrtmp.str();
                //errs() << "RVALUEP:" << rvaluep << "\n";
            }
        }


        //Generate full predicate expression
        fullExpPredicateInXml = "[" + lvaluep + " " + predicateInXml + " " + rvaluep + "]";
        //errs() << fullExpPredicateInXml << "\n";
        return fullExpPredicateInXml;

    }
    return "[]";
}

std::string GenerateAutomataTruePass::getPredicateSymOnXmlText(ICmpInst& icmpInst)
{
    std::string predicateText = "";

    ICmpInst::Predicate pr = icmpInst.getSignedPredicate();
    if(pr == ICmpInst::ICMP_EQ)
    {
        predicateText="==";
    }else if(pr == ICmpInst::ICMP_NE)
    {
        predicateText="!";
    }else if(pr == ICmpInst::ICMP_SGT)
    {
        predicateText="&gt;";
    }else if(pr == ICmpInst::ICMP_SGE)
    {
        predicateText="&ge;";
    }else if(pr == ICmpInst::ICMP_SLT)
    {
        predicateText="&lt;";
    }else if(pr == ICmpInst::ICMP_SLE)
    {
        predicateText="&le;";
    }
    /**switch(pr){
        case ICmpInst::ICMP_SGT: predicateText="&gt;"; break;
        case ICmpInst::ICMP_SLT: predicateText="&lt;"; break;
        case ICmpInst::ICMP_SGE: predicateText="&ge;"; break;
        case ICmpInst::ICMP_SLE: predicateText="&le;"; break;
    }**/

    return predicateText;

}

//To identify a block with a error location by __VERIFIER_error call function
bool GenerateAutomataTruePass::checkBBHasLError(BasicBlock& nowB)
{
    for( auto& I : nowB )
    {
        if(CallInst* callInst = dyn_cast<CallInst>(&I))
        {
            //errs() << callInst->getCalledFunction()->getName() << "\n";
            if(callInst->getCalledFunction()->getName() ==  "__VERIFIER_error")
            {
                return true;
            }
        }
    }
    //std::cout << i.begin << "\n";
    //std::cout << i->getOpcodeName(i->getOpcode()) << std::endl;
    return false;
}

char GenerateAutomataTruePass::ID = 5;
static RegisterPass<GenerateAutomataTruePass> X("generate_automata_true", "Generate Automata True");




