//===-- Parser.h ------------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_EXPR_PARSER_H
#define KLEE_EXPR_PARSER_H

#include "klee/Expr.h"

#include <vector>
#include <string>

namespace llvm {
  class MemoryBuffer;
}

namespace klee {
namespace expr {
  // These are the language types we manipulate.
  typedef ref<Expr> ExprHandle;
  typedef UpdateList VersionHandle;
  
  /// Identifier - Wrapper for a uniqued string.
  struct Identifier {
    const std::string Name;

  public:
    Identifier(const std::string _Name) : Name(_Name) {}
  };

  // FIXME: Do we have a use for tracking source locations?

  /// Decl - Base class for top level declarations.
  class Decl {
  public:
    Decl();
    virtual ~Decl() {}

    /// dump - Dump the AST node to stderr.
    virtual void dump() = 0;
  };

  /// ArrayDecl - Array declarations.
  ///
  /// For example:
  ///   array obj : 32 -> 8 = symbolic
  ///   array obj[32] : 32 -> 8 = { ... }
  class ArrayDecl : public Decl {
  public:
    /// Name - The name of this array.
    const Identifier *Name;

    /// Size - The maximum array size (or 0 if unspecified). Concrete
    /// arrays always are specified with a size.
    const unsigned Size;

    /// Domain - The width of indices.
    const unsigned Domain;

    /// Range - The width of array contents.
    const unsigned Range;
    
    /// Contents - The initial contents of the array. The array is
    /// symbolic if no contents are specified. The contained
    /// expressions are guaranteed to be constants.
    const std::vector<ExprHandle> Contents;

  public:
    template<typename InputIterator>
    ArrayDecl(const Identifier *_Name, unsigned _Size, 
              unsigned _Domain, unsigned _Range,
              InputIterator ContentsBegin=InputIterator(), 
              InputIterator ContentsEnd=InputIterator()) 
      : Name(_Name), Size(_Size), Domain(_Domain), Range(_Range),
        Contents(ContentsBegin, ContentsEnd) {}
  };

  /// VarDecl - Variable declarations, used to associate names to
  /// expressions or array versions outside of expressions.
  /// 
  /// For example:
  // FIXME: What syntax are we going to use for this? We need it.
  class VarDecl : public Decl {
  public:
    const Identifier *Name;    
  };

  /// ExprVarDecl - Expression variable declarations.
  class ExprVarDecl : public VarDecl {
  public:
    ExprHandle Value;
  };

  /// VersionVarDecl - Array version variable declarations.
  class VersionVarDecl : public VarDecl {
  public:
    VersionHandle Value;
  };

  /// CommandDecl - Base class for language commands.
  class CommandDecl : public Decl {
  public:
    const Identifier *Name;
  };

  /// QueryCommand - Query commands.
  ///
  /// (query [ ... constraints ... ] expression)
  /// (query [ ... constraints ... ] expression values)
  /// (query [ ... constraints ... ] expression values objects)
  class QueryCommand : public CommandDecl {
  public:
    // FIXME: One issue with STP... should we require the FE to
    // guarantee that these are consistent? That is a cornerstone of
    // being able to do independence. We may want this as a flag, if
    // we are to interface with things like SMT.

    /// Constraints - The list of constraints to assume for this
    /// expression.
    const std::vector<ExprHandle> Constraints;
    
    /// Query - The expression being queried.
    ExprHandle Query;

    /// Values - The expressions for which counterexamples should be
    /// given if the query is invalid.
    const std::vector<ExprHandle> Values;

    /// Objects - Symbolic arrays whose initial contents should be
    /// given if the query is invalid.
    const std::vector<ArrayDecl> Objects;

  public:
    template<typename InputIterator>
    QueryCommand(InputIterator ConstraintsBegin, 
                 InputIterator ConstraintsEnd,
                 ExprHandle _Query)
      : Constraints(ConstraintsBegin, ConstraintsEnd),
        Query(_Query) {}

    virtual void dump();
  };
  
  /// Parser - Public interface for parsing a .pc language file.
  class Parser {
  protected:
    Parser();
  public:
    virtual ~Parser();

    /// SetMaxErrors - Suppress anything beyond the first N errors.
    virtual void SetMaxErrors(unsigned N) = 0;
    
    /// GetNumErrors - Return the number of encountered errors.
    virtual unsigned GetNumErrors() const = 0;

    /// ParseTopLevelDecl - Parse and return a top level declaration,
    /// which the caller assumes ownership of.
    ///
    /// \return NULL indicates the end of the file has been reached.
    virtual Decl *ParseTopLevelDecl() = 0;

    /// CreateParser - Create a parser implementation for the given
    /// MemoryBuffer.
    ///
    /// \arg Name - The name to use in diagnostic messages.
    static Parser *Create(const std::string Name,
                          const llvm::MemoryBuffer *MB);
  };
}
}

#endif