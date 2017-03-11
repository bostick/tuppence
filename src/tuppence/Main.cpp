#include "Eval.h"
#include "Parser.h"
#include "TuppenceConfig.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace tuppence;

class Interpreter {
	Parser Parse;

public:
	Interpreter(std::istream& in) :
		Parse(Parser(in)) {

		eval::NamedValues["infinity"] = std::make_shared<Symbol>(eval::SYMBOL_INFINITY);
		eval::NamedValues["print"] = std::make_shared<BuiltinFunction>(eval::BUILTIN_PRINT);
		eval::NamedValues["exit"] = std::make_shared<BuiltinFunction>(eval::BUILTIN_EXIT);
		eval::NamedValues["rationalize"] = std::make_shared<BuiltinFunction>(eval::BUILTIN_RATIONALIZE);

	};

	void HandleDefinition(bool interactive) {
		if (auto FnAST = Parse.ParseDefinition()) {
			auto FnIR = FnAST->eval();
			if (!FnIR) {
				return;
			}
		}
		else {
			Parse.throwAwayLine();
		}
	}

	void HandleTopLevelExpression(bool interactive) {
		if (auto EAST = Parse.ParseTopLevelExpression()) {
			auto Res = EAST->eval();
			if (!Res) {
				return;
			}

			if (interactive) {
				if (auto FiniteWordRes = llvm::dyn_cast<FiniteWord>(Res.get())) {
					if (FiniteWordRes->getSize() == 0) {
						// Do not print empty
					}
					else {
						std::cout << Res->string() << "\n";
					}
				}
				else {
					std::cout << Res->string() << "\n";
				}
			}
		}
		else {
			Parse.throwAwayLine();
		}
	}

	// top ::= definition | external | expression | ';'
	void MainLoop(bool interactive) {
		while (1) {
			if (interactive) {
				std::cout << ">>> ";
			}
			Parse.readNextToken();
			auto tok = Parse.getCurrentToken();
			switch (tok) {
			case tok_identifier:
			case tok_rationalword:
			case tok_finiteword:
			case tok_if:
			case tok_for:
			case tok_while:
			case tok_var:
			case '(':
			case '{':
			case '-':
			case tok_ddd:
			case '~':
				HandleTopLevelExpression(interactive);
				break;
			case tok_define:
				HandleDefinition(interactive);
				break;
			case tok_eof:
			case '\n':
				break;
			case tok_error:
				Parse.throwAwayLine();
				break;
			default:
				std::cerr << "Unhandled token: " << tok << "\n";
				Parse.throwAwayLine();
				break;
			}

			if (Parse.getCurrentToken() == tok_eof) {
				std::exit(EXIT_SUCCESS);
			}
		}
	}
};

int main(int argc, char *argv[]) {
	
	// set cout to be unbuffered
	std::cout.setf(std::ios_base::unitbuf);

	if (argc > 1) {
		auto fileName = argv[1];

		std::ifstream infile;
		infile.open(fileName);
		if (!infile.is_open()) {
			std::cerr << "file not good";
			std::exit(EXIT_FAILURE);
		}
		if (!infile.good()) {
			std::cerr << "file not good";
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

		auto Inter = Interpreter(iss);
		Inter.MainLoop(false);

		std::exit(EXIT_SUCCESS);
		return 0;
	}
	else {
		std::cout << "Tuppence " << Tuppence_VERSION_MAJOR << "." << Tuppence_VERSION_MINOR << "\n";

		auto Inter = Interpreter(std::cin);

		// Run the main "interpreter loop" now.
		Inter.MainLoop(true);
		std::exit(EXIT_SUCCESS);
		return 0;
	}
}
