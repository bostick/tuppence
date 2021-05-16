
#include "runtime/FiniteWordImpl.h"

#include "llvm/Support/raw_ostream.h"

//#include <string>

int main(int argc, char *argv[]) {
    
    FiniteWordImpl *a = FiniteWordImpl_createFromVal(4, 5);
    FiniteWordImpl *b = FiniteWordImpl_createFromVal(4, 1);
    
    FiniteWordImpl *c = FiniteWordImpl_add(a, b);
    
    (void)c;
    
//    std::string str = FiniteWordImpl_toString(c, 10, false);
//
//    llvm::outs() << "start\n";
//    llvm::outs() << str << "\n";
//    llvm::outs() << "end\n";
}
