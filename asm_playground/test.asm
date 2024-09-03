.data
    format: .string "%d\n"

.text
    .globl main

main:
    /* prologue */
    pushl %ebp
    movl %esp, %ebp

    /* block-allocate 6*4B variables */
    sub $24, %esp

    /* a <- 1 */
    movl $1, %eax
    movl %eax, -0(%ebp)

    /* t1 < mul $2, a*/
    movl -0(%ebp), %eax
    imull $2, %eax
    movl %eax, -4(%ebp)

    /* b <- t1 */
    movl -4(%ebp), %eax
    movl %eax, -8(%ebp)

    /* t2 <- mul $4, %a */
    movl -0(%ebp), %eax
    imull $4, %eax
    movl %eax, -12(%ebp)

    /* t3 <- mul %b, %b */
    movl -4(%ebp), %eax
    imull -4(%ebp), %eax
    movl %eax, -16(%ebp)

    /* t4 <- div t2, $2 */
    movl -12(%ebp), %eax
    cdq
    movl $2, %ecx
    idivl %ecx
    movl %eax, -20(%ebp)

    /* t5 <- add t1, t2 */
    movl -4(%ebp), %eax
    addl -12(%ebp), %eax
    movl %eax, -24(%ebp)

    /* print(t5) */
    push -24(%ebp)
    push $format
    call printf
    addl $8, %esp

    /* epilogue */
    movl $0, %eax
    movl %ebp, %esp
    pop %ebp
    ret
