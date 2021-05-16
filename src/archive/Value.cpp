//===------ Value.cpp ------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "tuppence/Value.h"

#include "tuppence/Eval.h"

#include "llvm/Support/ErrorHandling.h" // for llvm_unreachable

#include <cassert>
#include <cstddef> // for size_t
#include <string>

using namespace tuppence;

//llvm::Type *Value::CreateType() {
//
////    std::vector<llvm::Type *> members;
////    members.push_back( llvm::IntegerType::get(eval::TheContext, 8));
////
////    auto voidTy = llvm::StructType::create(eval::TheContext, "void");
////    auto voidPtrTy = llvm::PointerType::get(voidTy, 0);
////    members.push_back(voidPtrTy);
//
//    llvm::StructType *valueTy = llvm::StructType::create(eval::TheContext, "struct.Value");
////    valueTy->setBody(members);
//
//    return valueTy;
//}

//const std::string tuppence::stringFromValueKind(Value::ValueKind Kind) {
//    switch (Kind) {
//    case Value::ValueKind::VK_BuiltinFunction: return "BuiltinFunction";
//    case Value::ValueKind::VK_BuiltinSymbol: return "BuiltinSymbol";
//    case Value::ValueKind::VK_FiniteWord: return "FiniteWord";
//    case Value::ValueKind::VK_RationalWord: return "RationalWord";
//    case Value::ValueKind::VK_UserFunction: return "UserFunction";
//    case Value::ValueKind::VK_List: return "List";
//    default: {
//        llvm_unreachable("Add this kind");
////        return "ASSERT";
//    }
//    }
//}

//const std::string Value::string() const {
//    return "<Value>";
//}

//// Use string() if a more specific definition is not provided
//const std::string Value::typesetString() const {
//    return string();
//}

//const std::string BuiltinFunction::string() const {
//    return "<BuiltinFunction:" + Proto->string() + ">";
//}

//const std::string UserFunction::string() const {
//    return "<UserFunction:" + Proto->string() + " " + Body->string() + ">";
//}

//const std::string UserFunction::typesetString() const {
//    return "\\unicode{60}UserFunction:" + Proto->typesetString() + " " + Body->typesetString() + "\\unicode{62}";
//}

//const std::string BuiltinSymbol::string() const {
//    return Name;
//}

//const std::string InfinitySymbol::typesetString() const {
//    return "\\infty";
//}

