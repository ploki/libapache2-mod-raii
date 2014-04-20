/* 
 * Copyright (c) 2005-2011, Guillaume Gimenez <guillaume@blackmilk.fr>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of G.Gimenez nor the names of its contributors may
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL G.Gimenez SA BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors:
 *     * Guillaume Gimenez <guillaume@blackmilk.fr>
 *
 */
#include "raii.H"

#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <ucontext.h>
#include <signal.h>
#include <execinfo.h>
#include <iostream>
namespace raii {

	bool dumpStackFlag=false;

	const char *fault_detail(int sig,const siginfo_t *siginfo)
	{
		if ( ! siginfo )
			return ("Unknown Fault, siginfo empty!");

		switch (sig)
		{
			case SIGILL:
				switch (siginfo->si_code)
				{
					case ILL_ILLOPC:
						return("Illegal Instruction: Illegal opcode.");
					case ILL_ILLOPN:
						return("Illegal Instruction: Illegal operand.");
					case ILL_ILLADR:
						return("Illegal Instruction: Illegal adressing mode.");
					case ILL_ILLTRP:
						return("Illegal Instruction: Illegal trap.");
					case ILL_PRVOPC:
						return("Illegal Instruction: Privileged opcode.");
					case ILL_PRVREG:
						return("Illegal Instruction: Privileged register.");
					case ILL_COPROC:
						return("Illegal Instruction: Coprocessor error.");
					case ILL_BADSTK:
						return("Illegal Instruction: Internal stack error.");
					default:
						return("Illegal Instruction: GNI?!");
				}
					case SIGFPE:
						switch (siginfo->si_code)
						{
							case FPE_INTDIV:
								return("Floating Point Exception: Integer divide by zero.");
							case FPE_INTOVF:
								return("Floating Point Exception: Integer overflow.");
							case FPE_FLTDIV:
								return("Floating Point Exception: Floating point divide by zero.");
							case FPE_FLTOVF:
								return("Floating Point Exception: Floating point overflow.");
							case FPE_FLTUND:
								return("Floating Point Exception: Floating point underflow.");
							case FPE_FLTRES:
								return("Floating Point Exception: Floating point inexact result.");
							case FPE_FLTINV:
								return("Floating Point Exception: Floating point invalid operation.");
							case FPE_FLTSUB:
								return("Floating Point Exception: Subscript out of range.");
							default:
								return("Floating Point Exception: GNI?!");
						}
							case SIGSEGV:
								switch (siginfo->si_code)
								{
									case  SEGV_MAPERR:
										return("Segmentation Violation: Adress not mapped to object.");
									case SEGV_ACCERR:
										return("Segmentation Violation: Invalid permissions for mapped object.");
									default:
										return("Segmentation Violation: GNI?!");
								}
									case SIGBUS:
										switch (siginfo->si_code)
										{
											case BUS_ADRALN:
												return("Bus Error: Invalid adress alignment.");
											case BUS_ADRERR:
												return("Bus Error: Non-existent physical adress.");
											case BUS_OBJERR:
												return("Bus Error: Object specific hardware error.");
											default:
												return("Bus Error: GNI?!");
										}
											case SIGABRT:
												return("Abort: fatal condition.");
											case SIGUSR2:
												return("User signal 2: stack dump requested.");
											default:
												return("Unknown Fault...");
		}
	}



	extern "C" void segfaulth(int sig,siginfo_t *siginfo, void *secret)
	{

		///c++ abi: nothing to play with
		/*
  ucontext_t *uc=(ucontext_t*)secret;
#ifdef __powerpc__
  signal.setFaultPosition((void *)uc->uc_mcontext.regs->nip);
#else
# ifdef __mips__
  signal.setFaultPosition((void *)( (int) ( uc->uc_mcontext.pc & (0xffffffff) ) ));
# else
  signal.setFaultPosition((void *)uc->uc_mcontext.gregs[REG_EIP]);
# endif
#endif
		 */
		//cerr <<  signal.getStackTrace();
		//cerr << signal.what();

		const char* detail_=fault_detail(sig,siginfo);
		char detail[1024];
		void *faultAddr=NULL;

		SegfaultBuffer emergencyBuffer;
		SegfaultBuffer *oldBuffer=NULL;
		bool stuckCandidate=false;

		if ( siginfo ) faultAddr=siginfo->si_addr;

		snprintf(detail,1024,"Signal (%d) : %s [%p]",sig,detail_,faultAddr);

		fprintf(stderr,"IN RAII SEGFAULT HANDLER, %s\n",detail);fflush(stderr);
               if(1){ //code à activer si demangle et throw merdouillent après segfault
                Exception e;
                std::cerr <<  e.toString() << "\n";
                std::cerr << e.getMessage() << "\n";
                e.poorManPrintStackTrace(false);
                }

		if ( !segfaultBuffer || sig == SIGUSR2 ) {
		        
			fprintf(stderr,
			        sig == SIGUSR2
			        ? "dumping stack\n"
			        : "segfaultBuffer is NULL (not in raii request)!\n");fflush(stderr);
			oldBuffer=segfaultBuffer;
			segfaultBuffer=&emergencyBuffer;
			stuckCandidate=true;
		}
 
		{ // segfaultBuffer is not null, in raii request
			switch (sig)
			{
				case SIGILL:
					segfaultBuffer->exception=new IllegalInstruction(detail);
					break;
				case SIGFPE:
					segfaultBuffer->exception=new FloatingPointException(detail);
					break;
				case SIGSEGV:
					segfaultBuffer->exception=new SegmentationFault(detail);
					break;
				case SIGBUS:
					segfaultBuffer->exception=new BusError(detail);
					break;
				case SIGABRT:
					segfaultBuffer->exception=new Abort(detail);
					break;
				case SIGUSR2:
					segfaultBuffer->exception=new DumpStack(detail);
				default:
					segfaultBuffer->exception=new Signal(detail);
			}
			if ( !segfaultBuffer->exception )
				fprintf(stderr,"UNABLE TO INSTANCIATE EXCEPTION\n");
			else {
				fprintf(stderr,"EXCEPTION INSTANCIATED, STACKTRACE SIZE IS %d\n",segfaultBuffer->exception->depth);
				fflush(stderr);


				segfaultBuffer->exception->depth=backtrace(segfaultBuffer->exception->stackTrace,1024);
				segfaultBuffer->exception->start=2;
				//int i;
/*
				//0 -> segfaulth
				//1 -> __restore_rt
				for ( i=2 ; i < depth ; i++ ) {
					segfaultBuffer->exception->stackTrace_.push_back(p[i]);
				}
				*/

			}
			fflush(stderr);

			segfaultBuffer->signal=sig;
			if ( stuckCandidate ) {
				segfaultBuffer=oldBuffer; //last state
				{
					static Mutex serialize_output;
					Lock l(serialize_output);
					Logger log("raii");
					log ("trying to dump stack");
					if ( emergencyBuffer.exception ) {
        					log.error(emergencyBuffer.exception->toString());
	        				std::cerr << emergencyBuffer.exception->getMessage() << "\n";
	        				if ( sig == SIGUSR2 )
        	        				emergencyBuffer.exception->printStackTrace();
        	        			else
        	        			        emergencyBuffer.exception->poorManPrintStackTrace();
	        		       }
	        		       else {
	        		                log("exception is null");
	        		       }
				}
				delete emergencyBuffer.exception;

				if ( sig == SIGUSR2 ) {
					dumpStackFlag=false;
					return;
				}

				//double fault héhé
				//apache redémarre pour le coup...
				//j'ai pas mieux à proposer. Sinon y'a toujours moyen de
				//passer le thread en stuck avec une contruction du type
				//do { sleep(5); fprintf(stderr,"bip\n"); } while(true);
				//disabling handler for current sig
				signal(sig,SIG_DFL);
				fprintf(stderr,"Commiting suicide with that weapon %s\n",
						fault_detail(sig,siginfo));fflush(stderr);
				raise(sig);
			}
			else {
				fprintf(stderr,"LONGJUMPING\n");fflush(stderr);
				longjmp(segfaultBuffer->env,sig);
			}
		}
	}
}
