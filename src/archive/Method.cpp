//===------ Method.cpp ------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#include "tuppence/Method.h"

#include "tuppence/Eval.h"
#include "tuppence/Logger.h"
#include "tuppence/RationalWord.h"
#include "tuppence/List.h"

#include "llvm/Support/Casting.h"

using namespace tuppence;

//const std::shared_ptr<Value> Method::call(std::shared_ptr<Value> Obj, std::vector<std::shared_ptr<Value>> Args) const {
//
//    auto Res = (*FunctionPointer)(Obj, Args);
//    if (!Res) {
//        return nullptr;
//    }
//
//    return Res;
//}

//const std::shared_ptr<Value> eval::size(std::shared_ptr<Value> Obj, std::vector<std::shared_ptr<Value>> Args) {
//
//    if (auto FiniteWordObj = llvm::dyn_cast<FiniteWord>(Obj.get())) {
//        return std::make_shared<RationalWord>(RationalWord::FactorySizeT(FiniteWordObj->size()));
//    }
//    else if (/*auto RationalWordObj = */llvm::dyn_cast<RationalWord>(Obj.get())) {
//        auto Infinity = eval::NamedValues["infinity"];
//        return Infinity;
//    }
//    else {
//        return LogError("Invalid value for size: " + Obj->string());
//    }
//}

//const std::shared_ptr<Value> eval::period(std::shared_ptr<Value> Obj, std::vector<std::shared_ptr<Value>> Args) {
//
//    if (auto RationalWordObj = llvm::dyn_cast<RationalWord>(Obj.get())) {
//        return std::make_shared<FiniteWord>(RationalWordObj->getPeriod());
//    } else {
//        return LogError("Invalid value for period: " + Obj->string());
//    }
//}

//const std::shared_ptr<Value> eval::transient(std::shared_ptr<Value> Obj, std::vector<std::shared_ptr<Value>> Args) {
//
//    if (auto RationalWordObj = llvm::dyn_cast<RationalWord>(Obj.get())) {
//        return std::make_shared<FiniteWord>(RationalWordObj->getTransient());
//    } else {
//        return LogError("Invalid value for transient: " + Obj->string());
//    }
//}

//const std::shared_ptr<Value> eval::numerator(std::shared_ptr<Value> Obj, std::vector<std::shared_ptr<Value>> Args) {
//
//    if (auto RationalWordObj = llvm::dyn_cast<RationalWord>(Obj.get())) {
//        auto Numerator = RationalWordObj->getNumerator();
//        return std::make_shared<RationalWord>(Numerator);
//    } else {
//        return LogError("Invalid value for numerator: " + Obj->string());
//    }
//}

//const std::shared_ptr<Value> eval::denominator(std::shared_ptr<Value> Obj, std::vector<std::shared_ptr<Value>> Args) {
//    
//    if (auto RationalWordObj = llvm::dyn_cast<RationalWord>(Obj.get())) {
//        auto Denominator = RationalWordObj->getDenominator();
//        return std::make_shared<RationalWord>(Denominator);
//    } else {
//        return LogError("Invalid value for denominator: " + Obj->string());
//    }
//}

