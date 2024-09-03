# Code Generation

## Operand Abstraction

Operands can be one of the following:
- Immediate value: `$<value>`
- Stack offset: `-<offset>(%ebp)`
- Register: `%<register>`

## Conversions

### 1. Declarations

```asm
movl <operand_a>, %eax
movl %eax, <operand_result>
```

### 2. Addition / Subtraction / Multiplication

```asm
movl <operand_a>, %eax
addl <operand_b>, %eax
movl %eax, <operand_result>
```

### 3. Division

```asm
movl <operand_a>, %eax
cdq
idivl <operand_b>
movl %eax, <operand_result>
```

### 4. Print

```asm
pushl <operand_a>
pushl $integer_format
call printf
addl $8, %esp
```

## Template

```
.data
    integer_format: .string "%d\n"

.text
    .globl main

main:
    /* prologue */
    pushl %ebp
    movl %esp, %ebp

    <codegen>

    /* epilogue */
    movl $0, %eax
    movl %ebp, %esp
    pop %ebp
    ret
```
