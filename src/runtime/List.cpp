//===------ List.cpp -------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

// #include "tuppence/List.h"
#include "../common/List.h"

#include "../common/FiniteWord.h"

// #include "tuppence/Logger.h"
#include "../common/RationalWord.h"

#include "llvm/Support/Casting.h"

#include <sstream>
#include <string>
#include <cassert>

// namespace tuppence {

	// namespace list {

//        const FiniteWord EMPTY = FiniteWord::FactoryEmpty();
//        const FiniteWord ZERO_1BIT = FiniteWord::FactoryBool((bool)0);
//        const FiniteWord ONE_1BIT = FiniteWord::FactoryBool((bool)1);
	// }
// }

// using namespace tuppence;


//
struct List {
    TuppenceValue **Data;
    size_t Size;

    List(TuppenceValue **Data, size_t Size) :
        Data(Data),
        Size(Size) {}
    
//    bool operator==(List) const;
//    bool operator!=(List) const;

    
};



List *List_EMPTY;

void List_initialize() {
    List_EMPTY = List_createEmpty();
}



List *List_createEmpty() {
    auto l = new List(nullptr, 0);
    return l;
}

List *List_createFromVals(TuppenceValue **Vals, size_t Count) {
    
    std::vector<TuppenceValue *> *newVals = new std::vector<TuppenceValue *>;
    for (size_t i = 0; i < Count; i++) {
        auto oldVal = Vals[i];
        auto copiedVal = Value_createFromValue(oldVal);
        newVals->push_back(copiedVal);
    }
    
    auto l = new List(&(*newVals)[0], Count);
    return l;
}

List *List_createFromList(List *list) {
    auto l = new List(list->Data, list->Size);
    return l;
}

size_t List_size(List *list) {
    return list->Size;
}

TuppenceValue **List_data(List *list) {
    return list->Data;
}

int32_t List_newString(List *list, char **str) {
    
    std::ostringstream strs;
    std::vector<TuppenceValue *> Vals(list->Data, list->Data + list->Size);
    
    // iterate reversed for printing
    strs << "(";
    for (auto iter = Vals.rbegin(); iter != Vals.rend(); ++iter) {
        char *valStr;
        Value_CreateString(*iter, &valStr);
        strs << valStr;
        if (std::next(iter) != Vals.rend()) {
            strs << ", ";
        }
    }
    strs << ")";
    std::string resStr = strs.str();
    
    
    auto cStr = new char[resStr.size() + 1];
    resStr.copy(cStr, resStr.size());
    cStr[resStr.size()] = '\0';
    
    *str = cStr;
    
    return 0;
}

//const std::string List::string() const {
//    std::ostringstream strs;
//    // iterate reversed for printing
//    strs << "(";
//    for (auto iter = Vals.rbegin(); iter != Vals.rend(); ++iter) {
//        strs << (*iter)->string();
//        if (std::next(iter) != Vals.rend()) {
//            strs << ", ";
//        }
//    }
//    strs << ")";
//    std::string str = strs.str();
//    return str;
//}

//const std::string List::typesetString() const {
//    std::ostringstream strs;
//    // iterate reversed for printing
//    strs << "(";
//    for (auto iter = Vals.rbegin(); iter != Vals.rend(); ++iter) {
//        strs << (*iter)->typesetString();
//        if (std::next(iter) != Vals.rend()) {
//            strs << ", ";
//        }
//    }
//    strs << ")";
//    std::string str = strs.str();
//    return str;
//}

//
// Structural Operations
//

TuppenceValue *List_get(List *list, size_t idx) {
    return list->Data[idx];
}

bool List_isComparable(List *list) {
    for (decltype(list->Size) i = 0; i < list->Size; i++) {
        auto Val = list->Data[i];
        if (Val->tag == FiniteWordTag) {

        }
        else if (Val->tag == RationalWordTag) {

        }
        else if (Val->tag == ListTag) {
            auto ListVal = Val->list;
            if (!List_isComparable(ListVal)) {
                return false;
            }
        }
        else {
            // some other, non-comparable element
            return false;
        }
    }
    return true;
}

FiniteWord *List_equal(List *list, List *other) {
    auto size = list->Size;
    assert(size == other->Size && "Sizes not equal");

    for (decltype(size) i = 0; i < size; i++) {
        auto Val = list->Data[i];
        auto OtherVal = other->Data[i];
        if (Val->tag == FiniteWordTag) {
            auto FiniteWordVal = Val->finite;
            if (OtherVal->tag == FiniteWordTag) {
                auto OtherFiniteWordVal = OtherVal->finite;
                if (FiniteWord_notEqual(FiniteWordVal, OtherFiniteWordVal)) {
                    return FiniteWord_ZERO_1BIT;
                }
            }
            else {
                assert(false && "do better type checking");
                return FiniteWord_ZERO_1BIT;
            }
        }
        else if (Val->tag == RationalWordTag) {
            auto RationalWordVal = Val->rational;
            if (OtherVal->tag == RationalWordTag) {
                auto OtherRationalWordVal = OtherVal->rational;
                if (RationalWord_notEqual(RationalWordVal, OtherRationalWordVal)) {
                    return FiniteWord_ZERO_1BIT;
                }
            }
            else {
                assert(false && "do better type checking");
                return FiniteWord_ZERO_1BIT;
            }
        }
        else if (Val->tag == ListTag) {
            auto ListVal = Val->list;
            if (OtherVal->tag == ListTag) {
                auto OtherListVal = OtherVal->list;
                if (List_notEqual(ListVal, OtherListVal)) {
                    return FiniteWord_ZERO_1BIT;
                }
            }
            else {
                assert(false && "do better type checking");
                return FiniteWord_ZERO_1BIT;
            }
        }
    }

    return FiniteWord_ONE_1BIT;
}

FiniteWord *List_notEqual(List *list, List *other) {
    auto equal = List_equal(list, other);
    return FiniteWord_not(equal);
}

FiniteWord *List_arrayEqual(List **Vals, size_t count) {
    assert(count > 1 && "Vals does not contain more than one element");
    auto First = Vals[0];
    for (decltype(count) i = 1; i < count; i++) {
        // maintain intuitive order
        if (List_notEqual(Vals[i], First)) {
            return FiniteWord_ZERO_1BIT;
        }
    }
    return FiniteWord_ONE_1BIT;
}

FiniteWord *List_arrayNotEqual(List **Vals, size_t count) {
    assert(count > 1 && "Vals does not contain more than one element");
    auto First = Vals[0];
    for (decltype(count) i = 1; i < count; i++) {
        // maintain intuitive order
        if (List_equal(Vals[i], First)) {
            return FiniteWord_ZERO_1BIT;
        }
    }
    return FiniteWord_ONE_1BIT;
}

TuppenceValue *List_concatenateArgs(TuppenceValue **ToCat, size_t Count) {
    if (Count == 0) {
        const char *msg = "Cannot concatenate 0 elements";
        return Value_createFromError(msg);
    }
    
//    auto ArgsCasted = std::vector<TuppenceValue *>(ToCat, ToCat + Count);
    
    auto AccumulateFinite = FiniteWord_EMPTY;
    for (size_t i = 0; i < Count-1; i++) {
        auto V = ToCat[i];
        if (V->tag == FiniteWordTag) {
            auto FiniteWordIter = V->finite;
            AccumulateFinite = FiniteWord_concatenate(FiniteWordIter, AccumulateFinite);
        }
        else {
            char *str;
            Value_CreateString(V, &str);
            return Value_createFromError((std::string("Expected FiniteWord: ") + str).c_str());
        }
    }
    
    auto Last = ToCat[Count-1];
    if (Last->tag == FiniteWordTag) {
        auto FiniteWordLast = Last->finite;
        AccumulateFinite = FiniteWord_concatenate(FiniteWordLast, AccumulateFinite);
        return Value_createFromFiniteWord(AccumulateFinite);
    }
    else if (Last->tag == RationalWordTag) {
        auto RationalWordLast = Last->rational;
        auto AccumulateRational = RationalWord_concatenate(RationalWordLast, AccumulateFinite);
        return Value_createFromRationalWord(AccumulateRational);
    }
    else {
        char *str;
        Value_CreateString(Last, &str);
        std::string s = std::string(str);
        //      Value_ReleaseString(Last, str);
        return Value_createFromError((std::string("Expected FiniteWord or RationalWord: ") + s).c_str());
    }

}

List *List_concatenateLists(TuppenceValue **ToCat, size_t Count) {
    
//    auto ArgsCasted = std::vector<TuppenceValue *>(ToCat, ToCat + Count);
    
    std::vector<TuppenceValue *> Vals;
    for (size_t i = 0; i < Count; i++) {
        auto V = ToCat[i];
        auto L = V->list;
        auto LSize = List_size(L);
        for (size_t j = 0; j < LSize; j++) {
            
            auto I = List_get(L, j);
            
            Vals.push_back(I);
        }
    }
    
    auto AccumulateList = List_createFromVals(&Vals[0], Vals.size());
    
    return AccumulateList;
}


