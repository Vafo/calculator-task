# calculator-task

## Brief
This project implements basic infix calculator, with extendability in mind.
Unary and binary operators and functions are supported.

## Contents
* Infix to postfix conversion implementation
* Token object, which represents numbers, operators etc. Attempted to be extendable
* basic implementation of vector, stack and smart pointers residing in util directory. It reappears in next projects and may not be synced

## Usage
<dl>
  <dt>
    postfix_converter_t Class
  </dt>
  <dd>
    convert(const std::string& in_str) - converts infix arithmetic expression to evaluable postfix_expr_t<br>
    Throws, if there is syntax error (e.g. misplaced operators, brackets etc) or unknown token is present
  </dd>
  <dt>
    postfix_expr_t
  </dt>
  <dd>
    evaluate() - evaluates expression
  </dd>
</dl>

## Future Work
A refactoring is urgently needed in
* General files structure
* Moving definition and renaming of classes and functions
* Rework extendability, to make it as smooth as possible
