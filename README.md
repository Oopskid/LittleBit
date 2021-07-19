LittleBit - a C++ bytecode interpreter
-
Definitions
-

**bytecode** - compiled code for an interpreter to execute

**LittleBit** - a C++ bytecode interpreter

**small** - a byte of information

**medium** - a short of information

**large** - an integer of information

**SM** - static memory. LittleBit reserves an (implementation dependant) amount of data addresses which are primarily used to point to instances which LittleBit has access to.
These instances are referred to by a static index. Instances can be changed by external code for different reasons; most notably the desire for the bytecode to be context specific!
An address does *not* have to point to an instance, the address can instead be used to convey a value. The reason for this is so that LittleBit can perform logical operations using
the same space. It is also so that a function with access to the LittleBit instance can return a value by setting to a predetermined address

**PS** - parameter selection. LittleBit calls functions by packing SM addresses into this array and passing it as an argument

General Usage
-
oh noes! incomplete!

Instruction Set
-
[0] `END`
> Indicates the end of the program. i.e. `return`

[1] `HOPFORWARD(small offset)`
> Moves the current program position forward by `offset` number of bytes

[2] `HOPBACK(small offset)`
> Moves the current program position backward by `offset` number of bytes

[3] `JUMP(large pos)`
> Sets the program position to the byte indicated by `pos`

[4] `BNZ(small id, large pos)`
> Checks the SM value (of the address) at `id` and sets the program position to `pos` *if and only if* the address is *not* zero

[5] `BOZ(small id, large pos)`
> Checks the SM value (of the address) at `id` and sets the program position to `pos` *if and only if* the address *is* zero

[6] `BNE(small id, large pos)`
> Checks the SM value (of the address) at `id` and sets the program position to `pos` *if and only if* the address is -ve under twos complement

[7] `BPO(small id, large pos)`
> Checks the SM value (of the address) at `id` and sets the program position to `pos` *if and only if* the address is +ve under twos complement

[8] `SFUNC(small id, small[] params)`

[9] `MFUNC(medium id, medium[] params)`

[10] `LFUNC(large id, large[] params)`
> Calls the registered function at `id` with the SM parameters referenced by IDs `params`. The amount listed in `params` *must* match the expected number of parameters as registered with the function