//===------ Value.h -------------------------------------------------------===//
//
// The Tuppence Programming Language
//
// This file is distributed under the MIT Open Source License.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace tuppence {

	class ExprAST;
	class PrototypeAST;

	/// Value - Base class for all values
	class Value {
	public:
		/// Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
		enum ValueKind {
			VK_BuiltinFunction,
			VK_FiniteWord,
			VK_RationalWord,
			VK_Symbol,
			VK_UserFunction,
			VK_ValueList
		};
	private:
		const ValueKind Kind;
	public:
		Value(ValueKind K) : Kind(K) {}

		virtual ~Value() {}

		virtual const std::string string() const = 0;

		const ValueKind getKind() const { return Kind; }
	};

	const std::string stringFromValueKind(Value::ValueKind);

	class UserFunction : public Value {
        const std::string Name;
		const std::shared_ptr<PrototypeAST> Proto;
		const std::shared_ptr<ExprAST> Body;

	public:
		UserFunction(std::string Name, const std::shared_ptr<PrototypeAST> Proto, const std::shared_ptr<ExprAST> Body) :
			Value(VK_UserFunction), Name(Name), Proto(Proto), Body(Body) {}

		const std::shared_ptr<Value> call(std::vector<std::shared_ptr<Value>> Args) const;

		const std::string string() const override;

		//
		// LLVM RTTI
		//
		static bool classof(const Value *S) {
			return S->getKind() == VK_UserFunction;
		}
	};

	class BuiltinFunction : public Value {
        const std::string Name;
		const std::shared_ptr<Value>(*FunctionPointer)(std::vector<std::shared_ptr<Value>>);

	public:
		BuiltinFunction(std::string Name, const std::shared_ptr<Value>(*FunctionPointer)(std::vector<std::shared_ptr<Value>>)) :
			Value(VK_BuiltinFunction), Name(Name), FunctionPointer(FunctionPointer) {}

		const std::shared_ptr<Value> call(std::vector<std::shared_ptr<Value>> Args) const;

		const std::string string() const override;

		//
		// LLVM RTTI
		//
		static bool classof(const Value *S) {
			return S->getKind() == VK_BuiltinFunction;
		}
	};

	class Symbol : public Value {
		const std::string Name;

	public:
		Symbol(std::string Name) :
			Value(VK_Symbol), Name(Name) {}

		const std::string string() const override;

		const std::string getName() const { return Name; }

		const bool operator==(Symbol) const;

		//
		// LLVM RTTI
		//

		static bool classof(const Value *S) {
			return S->getKind() == VK_Symbol;
		}
	};

	// 
	// math
	//
	const size_t gcd(size_t a, size_t b);

	const size_t lcm(size_t a, size_t b);

	const size_t bitLength(size_t);
}
