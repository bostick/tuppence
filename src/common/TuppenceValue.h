//===------ Value.h -------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

// #include "FiniteWordImpl.h"
// #include "List.h"
// #include "RationalWord.h"
//#include "../common/AST.h"

#include <cstdint>
#include <cstddef>

#ifdef _WIN32
#    if RUNTIME_DLL
#    define RUNTIME_API __declspec(dllexport)
#    else
#    define RUNTIME_API __declspec(dllimport)
#endif
#else
#define RUNTIME_API
#endif


enum TuppenceValueTag : uint8_t {
    BuiltinFunctionTag,
    BuiltinSymbolTag,
    FiniteWordTag,
    RationalWordTag,
    UserFunctionTag,
    ListTag,
    ErrorTag
};

struct BuiltinFunction;

struct BuiltinSymbol;

struct FiniteWord;

struct RationalWord;

struct UserFunction;

struct List;

struct TuppenceValue {
    TuppenceValueTag tag;
    union {
        BuiltinFunction *builtinFunction;
        BuiltinSymbol *builtinSymbol;
        FiniteWord *finite;
        RationalWord *rational;
        UserFunction *userFunction;
        List *list;
        char *error;
    };
};


extern "C" RUNTIME_API {

    void Value_initialize();
    
//    TuppenceValue *Value_CreateBuiltinFunction(tuppence::PrototypeAST *Proto);

    TuppenceValue *Value_createFromFiniteWord(FiniteWord *fin);

    TuppenceValue *Value_createFromRationalWord(RationalWord *rat);

    TuppenceValue *Value_createFromList(List *list);

    TuppenceValue *Value_createFromError(const char *);
    
    TuppenceValue *Value_createFromBuiltinSymbol(BuiltinSymbol *);
    
    TuppenceValue *Value_createFromBuiltinFunction(BuiltinFunction *);
    
    
    
    
    TuppenceValue *Value_createFromValue(TuppenceValue *);
    
    uint8_t Value_tag(TuppenceValue *);

//    void Value_dump(TuppenceValue *);
    

//    void Value_ReleaseValue(TuppenceValue *);
    
    
    
    
    
    
    int Value_CreateString(TuppenceValue *, char**);
    
//    void Value_ReleaseString(TuppenceValue *, char*);
    

    
    
    
    
    
    TuppenceValue *Value_Unary(char Op, TuppenceValue *Arg1);
    
    TuppenceValue *Value_Binary(char Op, TuppenceValue *Arg1, TuppenceValue *Arg2);
    
    TuppenceValue *Value_Infix(char Op, TuppenceValue **args, size_t Count);


    extern TuppenceValue *TuppenceValue_EMPTYWORD;
    extern TuppenceValue *TuppenceValue_ZERO_1BIT;
    extern TuppenceValue *TuppenceValue_ONE_1BIT;
    extern TuppenceValue *TuppenceValue_ONE;
    extern TuppenceValue *TuppenceValue_ZERO;
    extern TuppenceValue *TuppenceValue_MINUS_ONE;
    extern TuppenceValue *TuppenceValue_EMPTYLIST;
    
    extern TuppenceValue *TuppenceValue_INFINITY;
    
    extern TuppenceValue *TuppenceValue_PRINT;
    extern TuppenceValue *TuppenceValue_EXIT;
    extern TuppenceValue *TuppenceValue_RATIONALIZE;
    extern TuppenceValue *TuppenceValue_SIZE;
    extern TuppenceValue *TuppenceValue_TRANSIENT;
    extern TuppenceValue *TuppenceValue_PERIOD;
    extern TuppenceValue *TuppenceValue_NUMERATOR;
    extern TuppenceValue *TuppenceValue_DENOMINATOR;
}











#if 0

#pragma once

#include "tuppence/Parser.h"

#include "llvm/IR/DerivedTypes.h"

#include <memory>
#include <string>
#include <vector>

namespace tuppence {
    
    // Forward declare
//    class PrototypeAST;
//    class ExprAST;
    
//    struct Value;
    
	/// Value - Base class for all values
    class NamedValue {
    public:
        /// Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
        enum ValueKind {
            VK_BuiltinFunction,
//            VK_BuiltinSymbol,
//            VK_FiniteWord,
//            VK_RationalWord,
            VK_UserFunction
//            VK_List
        };
    private:
        const ValueKind Kind;
    public:
        NamedValue(ValueKind K) : Kind(K) {}

        virtual ~NamedValue() {}
        
//        static llvm::Type *CreateType();

//        virtual const std::string string() const;
        
//        virtual const std::string typesetString() const;
        
        ValueKind getKind() const { return Kind; }
    };

//    const std::string stringFromValueKind(Value::ValueKind);
    
    
    class BuiltinFunction : public NamedValue {
        const std::shared_ptr<PrototypeAST> Proto;
        //        llvm::Function *FunctionPointer;

    public:
        BuiltinFunction(const std::shared_ptr<PrototypeAST> Proto) :
        NamedValue(VK_BuiltinFunction), Proto(Proto) {}

        const std::shared_ptr<PrototypeAST> getProto() const { return Proto; }

        //        llvm::Function *getFunctionPointer() const { return FunctionPointer; }

        //        const std::shared_ptr<Value> call(std::vector<std::shared_ptr<Value>> Args) const;

//        const std::string string() const override;

        //
        // LLVM RTTI
        //
        static bool classof(const NamedValue *S) {
            return S->getKind() == VK_BuiltinFunction;
        }
    };
    
//    class BuiltinSymbol : public Value {
//        const std::string Name;
//
//    public:
//        BuiltinSymbol(std::string Name) :
//        Value(VK_BuiltinSymbol), Name(Name) {}
//
//        virtual const std::string string() const override;
//
//        const std::string getName() const { return Name; }
//
//        //
//        // LLVM RTTI
//        //
//
//        static bool classof(const Value *S) {
//            return S->getKind() == VK_BuiltinSymbol;
//        }
//    };
    
//    class InfinitySymbol : public BuiltinSymbol {
//
//    public:
//        InfinitySymbol() :
//        BuiltinSymbol("infinity") {}
//
//        virtual const std::string typesetString() const override;
//
//    };
    
    class UserFunction : public NamedValue {
//        const std::string Name;
        const std::shared_ptr<PrototypeAST> Proto;
        const std::shared_ptr<ExprAST> Body;
//        llvm::Function *FunctionPointer;
        
    public:
        UserFunction(const std::shared_ptr<PrototypeAST> Proto, const std::shared_ptr<ExprAST> Body) :
            NamedValue(VK_UserFunction), Proto(Proto), Body(Body) {}
        
        const std::shared_ptr<PrototypeAST> getProto() const { return Proto; }
        
//        llvm::Function *getFunctionPointer() const { return FunctionPointer; }
        
//        const std::shared_ptr<Value> call(std::vector<std::shared_ptr<Value>> Args) const;

//        const std::string string() const override;
        
//        virtual const std::string typesetString() const override;
        
        //
        // LLVM RTTI
        //
        static bool classof(const NamedValue *S) {
            return S->getKind() == VK_UserFunction;
        }
    };
    
} // end namespace tuppence

#endif

