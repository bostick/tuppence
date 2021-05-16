
#include "TuppenceConfig.h"
#include "../lib/Interpreter.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

#include <fstream>
#include <sstream>
//#include <cstdlib> // for std::exit
#include <iostream>
#include <unistd.h>

using namespace tuppence;

llvm::cl::opt<bool> Jupyter("jupyter", llvm::cl::desc("Enable running Tuppence inside of Jupyter Notebook"));
//llvm::cl::opt<bool> Codegen("codegen", llvm::cl::desc("Enable code generation"));
llvm::cl::opt<std::string> InputFilename(llvm::cl::Positional, llvm::cl::desc("<input file>"), llvm::cl::init("-"));

void PrintVersion(llvm::raw_ostream &s) {
    s << "Tuppence " << Tuppence_VERSION_MAJOR << "." << Tuppence_VERSION_MINOR << "\n";
}

int main(int argc, char *argv[]) {
    llvm::cl::SetVersionPrinter(&PrintVersion);
    llvm::cl::ParseCommandLineOptions(argc, argv);
    
	// set cout to be unbuffered
	std::cout.setf(std::ios_base::unitbuf);

	if (InputFilename != "-") {

		std::ifstream infile;
		infile.open(InputFilename);
		if (!infile.is_open()) {
			std::cerr << "file not good\n";
			std::exit(EXIT_FAILURE);
		}
		if (!infile.good()) {
			std::cerr << "file not good\n";
			std::exit(EXIT_FAILURE);
		}

		std::string entireFile;
		std::string line;
		while (std::getline(infile, line))
		{
			entireFile += line;
			entireFile += "\n";
		}
		entireFile += "\n";
		std::istringstream iss(entireFile);

		auto Inter = Interpreter(iss, 0);
		Inter.MainLoop();

		std::exit(EXIT_SUCCESS);
		return 0;
	}
	else {
        llvm::raw_fd_ostream o(STDOUT_FILENO, false, false);
        
        PrintVersion(o);
        
        int flags = COMMANDLINE_INTERACTIVEBIT;
        if (Jupyter) {
            flags |= COMMANDLINE_JUPYTERBIT;
        }
//        if (Codegen) {
//            flags |= COMMANDLINE_CODEGENBIT;
//        }
        
		auto Interp = Interpreter(std::cin, flags);
		Interp.MainLoop();
        
		std::exit(EXIT_SUCCESS);
		return 0;
	}
}
