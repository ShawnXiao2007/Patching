//block ID, signal based sync
#include "BPatch.h"
#include "BPatch_edge.h"
#include "BPatch_image.h"
#include <stdio.h>
#include "BPatch_function.h"
#include "BPatch_basicBlock.h"
#include "BPatch_flowGraph.h"
#include "BPatch_point.h"
#include "Expression.h"
#include "Instruction.h"
#include <iostream>
#include <string>
#include "snippetGen.h"

using namespace std;
using namespace Dyninst;
int main(int argc, char* argv[])
{
	//open a file, output the mapping from id to addresses to it
	FILE * pFile;
	pFile=fopen(argv[3],"w");
	if(pFile==NULL)perror("Error opening file.\n");
	fprintf(pFile,"shortID\tfullID\tentryAddr\texitAddr\n");

	//open argv[1]
	BPatch bpatch;
	BPatch_binaryEdit *binedit=bpatch.openBinary(argv[1]);
	BPatch_image *appImage=binedit->getImage();	
	std::vector<BPatch_function *> *funcs_ptr;
	funcs_ptr=appImage->getProcedures();
	std::vector<BPatch_function *> funcs=*funcs_ptr;

	//open libc and find printf
	std::vector<BPatch_function *> printfFuncs;
	BPatch_binaryEdit *binedit_libc=bpatch.openBinary("/lib/libc.so.6");
	BPatch_image *appImage_libc=binedit_libc->getImage();
	appImage_libc->findFunction("printf",printfFuncs);

	//all points
	std::vector<BPatch_point *> * points=new std::vector<BPatch_point *>;
	//create a buf and a index
	BPatch_type *typeShort=appImage_libc->findType("unsigned short");
	BPatch_type *typeUL=appImage_libc->findType("unsigned long");
	//std::string bufname=std::string("buf");
	//BPatch_variableExpr *bufExpr=binedit->malloc(65536,bufname);
	//BPatch_snippet *basePtr=new BPatch_constExpr(bufExpr->getBaseAddr());
	BPatch_variableExpr *index=binedit->malloc(*typeShort);
	index=binedit->createVariable(std::string("index"),(Dyninst::Address)index->getBaseAddr(),typeShort);
	unsigned short indexIni=0;
	index->writeValue(&indexIni);
	BPatch_variableExpr *basePtr=binedit->malloc(*typeUL);
	index=binedit->createVariable(std::string("basePtr"),(Dyninst::Address)basePtr->getBaseAddr(),typeUL);
	BPatch_snippet* basePtrAddr=new BPatch_constExpr(basePtr->getBaseAddr());

	//open PtrDeref
	BPatch_binaryEdit *binedit_PtrDeref=bpatch.openBinary("/usr/lib/libPtrDeref.so");
	BPatch_image *appImage_PtrDeref=binedit_PtrDeref->getImage();
	std::vector<BPatch_function *> RegSig;
        appImage_PtrDeref->findFunction("shm_sig",RegSig);	
	std::vector<BPatch_snippet *> RegSigArgs;
	RegSigArgs.push_back(basePtrAddr);
	BPatch_funcCallExpr RegSigCall(*(RegSig[0]),RegSigArgs);
        std::vector<BPatch_function *> stoplogger;
        appImage_PtrDeref->findFunction("stoplogger",stoplogger);
        std::vector<BPatch_snippet *> stoploggerArgs;  
        BPatch_funcCallExpr stoploggerCall(*(stoplogger[0]),stoploggerArgs);


	printf("begin inserting to start...\n");
	//find _start function and insert
	std::vector<BPatch_function *> startFuncs;
	appImage->findFunction("_start",startFuncs);
	std::vector<BPatch_point *> *startPoints=new std::vector<BPatch_point *>;
	startPoints=startFuncs[0]->findPoint(BPatch_entry);
	binedit->insertSnippet(BPatch_arithExpr(BPatch_assign,*basePtr,RegSigCall),startPoints[0]);
	//find fini function and insert
        std::vector<BPatch_function *> finiFuncs;
        appImage->findFunction("_fini",finiFuncs);
        std::vector<BPatch_point *> *finiPoints=new std::vector<BPatch_point *>;
        finiPoints=finiFuncs[0]->findPoint(BPatch_exit);
        binedit->insertSnippet(stoploggerCall,finiPoints[0]);


	unsigned int mod=65535;
	unsigned int id=0; 

	for(std::vector<BPatch_function *>::iterator fi=funcs.begin();
		fi!=funcs.end();fi++)
	{
		BPatch_flowGraph *fg=(*fi)->getCFG();
	
		std::set<BPatch_basicBlock *> blocks;
		fg->getAllBasicBlocks(blocks);

		std::set<BPatch_basicBlock *>::iterator block_iter;
		for(block_iter=blocks.begin();block_iter!=blocks.end();++block_iter)
		{
			BPatch_basicBlock *block=*block_iter;
			BPatch_point *entry_point=NULL;
			unsigned long blockstart=0;
			unsigned long blockend=0;
			unsigned short partial_id=65534;
			if(block!=NULL)
			{
				entry_point=block->findEntryPoint();
				blockstart=block->getStartAddress();
                                blockend=block->getEndAddress();
				partial_id=id%mod+1;	
				fprintf(pFile,"%u\t%u\t%p\t%p\n",partial_id,id,blockstart,blockend);
				id++;
			}
			void *entryAddr=NULL;
			if(entry_point!=NULL)
			{
				entryAddr=entry_point->getAddress();
				points->push_back(entry_point);
				std::vector<BPatch_snippet *> printfArgs3;
				BPatch_snippet *fmt=new BPatch_constExpr("entrypoint first instruction address: %p\n");
				printfArgs3.push_back(fmt);
				BPatch_snippet *entryAddr=new BPatch_constExpr(blockstart);
				printfArgs3.push_back(entryAddr);
				BPatch_funcCallExpr printfCall3(*(printfFuncs[0]),printfArgs3);	
				//binedit->insertSnippet(printfCall3,*entry_point);
				std::string s=entry_point->getFunction()->getName();
				printf("entry point is in:%s\n",s.c_str());
				std::vector<BPatch_snippet *> printfArgs4;
				BPatch_snippet *fmt_m=new BPatch_constExpr("memory log entrypoint: %p\tid: %hu\t func: %s\n");
				printfArgs4.push_back(fmt_m);
				BPatch_snippet entryAddr_m=BPatch_arithExpr(BPatch_plus,*basePtr,/**index*/BPatch_constExpr(0));
				printfArgs4.push_back(&entryAddr_m);
				//block partial id
				BPatch_snippet *idsnippet=new BPatch_constExpr(partial_id);
				printfArgs4.push_back(idsnippet);
				BPatch_constExpr *funcname=new BPatch_constExpr(s.c_str());
				printfArgs4.push_back(funcname);

				BPatch_arithExpr log(BPatch_seq,
						BPatch_funcCallExpr(*(printfFuncs[0]),printfArgs4),						
						BPatch_arithExpr(BPatch_seq,
							BPatch_arithExpr(BPatch_assign,BPatch_arithExpr(BPatch_plus,*basePtr,/**index*/BPatch_constExpr(0)),*idsnippet),
							BPatch_arithExpr(BPatch_assign,*basePtr/**index*/,BPatch_arithExpr(BPatch_plus,*basePtr/**index*/,BPatch_constExpr(2)))
								)
							);
				BPatch_arithExpr log_s=BPatch_arithExpr(BPatch_seq,
							BPatch_arithExpr(BPatch_assign,BPatch_arithExpr(BPatch_plus,*basePtr,/**index*/BPatch_constExpr(0)),*idsnippet),
							BPatch_arithExpr(BPatch_assign,*basePtr/**index*/,BPatch_arithExpr(BPatch_plus,*basePtr/**index*/,BPatch_constExpr(2)))
								);
				binedit->insertSnippet(log_s,*entry_point);	
				//binedit->insertSnippet(printfCall3,*entry_point);		
			}
		}
	}
	//binedit->insertSnippet(printfCall, *points);
	binedit->writeFile(argv[2]);
	fclose(pFile);

}
	
