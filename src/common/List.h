//===------ List.h - List class definition ----------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "TuppenceValue.h"
//#include "tuppence/FiniteWord.h"
// #include "tuppence/Value.h"

#include <memory>
#include <vector>

#ifdef _WIN32
#    if RUNTIME_DLL
#    define RUNTIME_API __declspec(dllexport)
#    else
#    define RUNTIME_API __declspec(dllimport)
#endif
#else
#define RUNTIME_API
#endif





extern "C" RUNTIME_API {

    void List_initialize();
    
    List *List_createEmpty();
    List *List_createFromVals(TuppenceValue **Vals, size_t Count);
    List *List_createFromList(List *list);
    
    
    int32_t List_newString(List *word, char **str);
    

    TuppenceValue *List_get(List *list, size_t idx);

    size_t List_size(List *list);

    TuppenceValue **List_data(List *list);
    
    bool List_isComparable(List *list);

    FiniteWord *List_equal(List *, List *);
    FiniteWord *List_notEqual(List *, List *);
    
    FiniteWord *List_arrayEqual(List **ToTest, size_t Count);
    FiniteWord *List_arrayNotEqual(List **ToTest, size_t Count);

    TuppenceValue *List_concatenateArgs(TuppenceValue **ToCat, size_t Count);
    
    List *List_concatenateLists(TuppenceValue **ToCat, size_t Count);
    
    // TuppenceValue List_get(List list, size_t idx);

    // size_t List_size(List list);

    extern List *List_EMPTY;
    
}


// namespace tuppence {

//     class List {
        
//     public:
//         size_t size() const;
        
//         TuppenceValue operator[](size_t i) const;
//     };
    
//    class List : public Value {
//        const std::vector<std::shared_ptr<Value>> Vals;
//
//    public:
//        List(std::vector<std::shared_ptr<Value>> Vals) :
//            Value(VK_List), Vals(Vals) {}
//
//        const std::string string() const override;
//
//        const std::string typesetString() const override;
//
//        typedef std::vector<std::shared_ptr<Value>>::const_iterator const_iterator;
//        typedef std::vector<std::shared_ptr<Value>>::const_reverse_iterator const_reverse_iterator;
//        
//        const_iterator begin() const { return Vals.begin(); }
//        const_iterator end() const { return Vals.end(); }
//
//        const_reverse_iterator rbegin() const { return Vals.rbegin(); }
//        const_reverse_iterator rend() const { return Vals.rend(); }
//        
//        const std::shared_ptr<Value> operator[](size_t i) const {
//            return Vals[i];
//        }
//
//        size_t size() const {
//            return Vals.size();
//        }
//
//        //
//        // structural operations
//        //
//
//        /// This list only contains comparable elements
//        bool isComparable() const;
//
//        bool operator==(List) const;
//        bool operator!=(List) const;
//
//        static const FiniteWord equal(std::vector<List>);
//        static const FiniteWord notequal(std::vector<List>);
//
//        const std::shared_ptr<Value> concatenate() const;
//
//        //
//        // LLVM RTTI
//        //
//
//        static bool classof(const Value *S) {
//            return S->getKind() == VK_List;
//        }
//    };
    
// } // end namespace tuppence
