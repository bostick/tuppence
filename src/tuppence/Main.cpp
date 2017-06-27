
#include "TuppenceConfig.h"
#include "Interpreter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib> // for std::exit

using namespace tuppence;

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
