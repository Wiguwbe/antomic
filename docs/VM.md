# Intel on the antomic scripting VM

Operations shall be stack based with a backing dictionary,

_That is:_

For basic operations (arithmetic, functions, etc...), a stack shall be used;

But, variables will be stored/loaded from a backing dictionary.

To handle scopes, there should also be a _stack_ of dictionaries.
When a new scope is entered, a new dictionary is created and pushed into the
stack. Similarly, when the scope is left, the stack if pop'ed.
A `LOAD` from the dictionary will try successively (top to bottom)
until either the name is found or all the search exhausted.


## Remarks

_Some remarks regarding antomic's python_

- Function arguments have types/kinds, when calling a function, should passed types be checked?;
- Function arguments don't have default values;
- There should be a way to call native/engine functions (`getattr` and such, instead of defining more opcodes? or can a function be defined as native or bytecode?) (`CALLN`?);

---

## Classes and functions

functions' bodies are compiled into `code` objects and made into a function
when running the module,

classes are more or less the same: the initalization body is compiled into
`code` objects and run from the module

---

## File Format

### Header

_TODO_

should contain a reference/index to the `__main__` code object

### Constants/Literals

_Holds the names/values used directly in code_

It's a list of strings/objects,

_TODO figure out how to store this list_

#### Option 1

_Indexing before_

After the header, an array of `n` items appear to specify, for each item,
the type of the item, the length, and the offset (relative to init of file or end of header?) to where the object is located in the _pool_ (explained next);

The _pool_ contains all the objects.

>This option simplifies the lookup times but occupies a bit more space (and
integer, the offset, per item).

#### Option 2

_Array_

Here, each item on the list contains its type, size `size` and is immediately
followed by `size` bytes: its content.

> This option drops the initial list, but object lookup takes a _bit_ more time

### Types of constants/literals

- _Name_: a name, string, reference to something (e.g. `print`, `attribute`, etc...);
- _String_: an actual string. (where should the fstring go?);
- _Integer_: an integer constant (include float here?);
- _Float_: a float/double/decimal constant;
- _Code_: a code object, compiled bytecode.

---

## OP Codes

Opcodes take 2 bytes (16 bits). The first `n` bits are for the opcode,
the rest `16-n` bits are for indexing.

_If we only allocate 1 byte/8 bits for indexing, the literals can only be, at
most, 256. Allocating only the minimum necessary to the opcode leaves more
room to indexing (e.g. 16 instructions, 4 bits to opcode,
12 bits to indexing, 4096 literals)_

### Dictionary operations

Here, index refers to the literals list (built into the bytecode)

- `LOAD <index>`: load a variable (from backing dict, name at index)
	and push into the stack;
- `LOAD_CONST <index>`: load a value from the literals (may be a name)
	and push into stack;
- `STORE <index>`: pop variable name from stack, pop value from stack, store into backing dict;
- `DEL <index>`: delete a variable from dict, name at index;

**TODO**: include dictionary push operation? Or make that implicit?

### Stack operations

- `POP`: remove from top of stack;
- `DUP`: duplicate top of stack;

**TODO**: rotate and more?

### Flow control

- `CALL <args_count>`: pops a _callable_ from the top of stack and calls it,
	taking `args_count` arguments with it;
- `RETURN`: returns from functions/code;
- `JMP <instructions>` jump number of instructions
- `JT <instructions>`: jump if top of stack evaluates to True (pops stack)
- `JNT <instructions>`: jump if top of stack is not True (pops stack)

### Object oriented

- `GETATTR <index>`: get attr (from `<index>`) of value `<stack>` and push into stack;
- `SETATTR <index>`: set attribute (from `<index>`) of `<stack>` to `<stack-1>`;
- `DELATTR <index>`: delete attribute (from `<index`>) of `<stack>`;
- `GETSUB`: get subscript (index `<stack>`) from object at `<stack-1>`;
- `SETSUB`: set subscsript (index `<stack>`) of object `<stack-1>` to `<stack-2>`;
- `DELSUB`: delete item at index `<stack>` from object `<stack-1>`;
- `IMP <index>`: get module name from `<index>`, import it, push module into stack; (import as: store as name);
- `RAISE`: raise exception at `<stack>`;
- `MAKE_FUNC <arg_count>`: create function from code object and argument types. Will take code object from `<stack>` and the rest of the arguments are the names (as strings?) of the arguments; _TODO include types?_
- `MAKE_CLASS <base_count>`: use current namespace to create a class. Name of class should be at <stack> (as string/name?) and the bases follow;

### Containers

- `TUPLE <count>`: make a tuple taking `<count>` items from stack;
- `LIST <count>`: make a list taking `<count>` items from stack; _should it just turn a tuple into list instead?_
- `DICT`: make a dict tacking a keys tuple from `<stack>` and values tuple from `<stack-1>`;

### Iterators

- `ITER`: create an iterator given an object at `<stack>` (pops stack);
- `NEXT <else>`: get next value from iterator on `<stack>` (doesn't pop). If the iterator is exhausted, jumps to instruction at `<else>`;
- `SLICE <arg_count>`: create an iterator based on a slice (parameters follow the classic python implementation);

### Exceptions

- `TRY <exc_ip>`: declares exception handlers starting at `<exc_ip>`;
- `HANDLE`: mark exception as _handled_;
- `END_TRY`: mark try block as ended. Useful when no exception handlers are declared (or none handle the exception) and there may be a `finally` statement;

### Arithmetic

Most of there operations will pop 2 (sometimes just 1) operands from the
stack and push the result back into the stack.

For binary operations (ones that require 2 operands), the form is:

`<stack-1> <op> <stack>`

> **Note:**
>
> In order to save opcodes (less opcodes = more indexing space),
> These operations can use the index to specify which operation to run,
> Aggregating all these operations inside a single `OP`/`ARIT` opcode.

- `OP_ADD`: addition;
- `OP_SUB`: subtraction;
- `OP_NEG`: unary, negate;
- `OP_MUL`: multiplication;
- `OP_MMUL`: matrix multiplication;
- `OP_DIV`: division;
- `OP_FDIV`: floor division;
- `OP_EXP`: exponentiaion (`<stack-1> ** <stack>`);
- `OP_MOD`: modulo;
- `OP_OR`: binary OR;
- `OP_AND`: binary AND;
- `OP_XOR`: binary XOR;
- `OP_INV`: unary, binary inversion;
- `OP_SHL`: shift left;
- `OP_SHR`: shift right;
- `OP_EQ`: test for equality;
- `OP_NEQ`: test for equality (not);
- `OP_GEQ`: equal/greater;
- `OP_LEQ`: equal/lesser;
- `OP_GT`: greater than;
- `OP_LT`: less then;
- `OP_IS`: `is` keyword, `is not` simply NOT later
- `OP_IN`: `in` keyword, `not in` simply NOT later
- `OP_BOOL_AND`: boolean AND;
- `OP_BOOL_OR`: boolean OR;
- `OP_BOOL_NOT`: boolean NOT

> **TODO**
>
> Shall the VM contemplate OPCodes to instantiate dict/tuple/list ?
>
> In case of dictionaries and lists, one can call the correspondent constructor
> and passing the arguments _but_,
> tuples could take advantage of bytecode creation.
>
> For reference, CPython has these opcodes.