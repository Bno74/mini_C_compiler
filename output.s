.data
.L_fmt_d:
	.string "%d\n"
.text
.globl main
main:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$2, %rax
	pushq	%rax
	movq	$3, %rax
	popq	%rbx
	addq	%rbx, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	pushq	%rax
	movq	$3, %rax
	popq	%rbx
	addq	%rbx, %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	pushq	%rax
	movq	$4, %rax
	popq	%rbx
	addq	%rbx, %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	pushq	%rax
	movq	$7, %rax
	popq	%rbx
	addq	%rbx, %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, %rsi
	leaq	.L_fmt_d(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	subq	$32, %rsp
	movl	$0, %eax
	leave
	ret
