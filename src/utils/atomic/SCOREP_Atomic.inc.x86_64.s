	.file	"SCOREP_Atomic.inc.c"
	.text
	.p2align 4,,15
	.globl	SCOREP_Atomic_TestAndSet
	.type	SCOREP_Atomic_TestAndSet, @function
SCOREP_Atomic_TestAndSet:
.LFB0:
	.cfi_startproc
	movl	$1, %eax
	xchgb	(%rdi), %al
	ret
	.cfi_endproc
.LFE0:
	.size	SCOREP_Atomic_TestAndSet, .-SCOREP_Atomic_TestAndSet
	.p2align 4,,15
	.globl	SCOREP_Atomic_clear
	.type	SCOREP_Atomic_clear, @function
SCOREP_Atomic_clear:
.LFB1:
	.cfi_startproc
	movb	$0, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE1:
	.size	SCOREP_Atomic_clear, .-SCOREP_Atomic_clear
	.p2align 4,,15
	.globl	SCOREP_Atomic_ThreadFence
	.type	SCOREP_Atomic_ThreadFence, @function
SCOREP_Atomic_ThreadFence:
.LFB2:
	.cfi_startproc
	mfence
	ret
	.cfi_endproc
.LFE2:
	.size	SCOREP_Atomic_ThreadFence, .-SCOREP_Atomic_ThreadFence
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_void_ptr
	.type	SCOREP_Atomic_LoadN_void_ptr, @function
SCOREP_Atomic_LoadN_void_ptr:
.LFB3:
	.cfi_startproc
	movq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE3:
	.size	SCOREP_Atomic_LoadN_void_ptr, .-SCOREP_Atomic_LoadN_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_void_ptr
	.type	SCOREP_Atomic_StoreN_void_ptr, @function
SCOREP_Atomic_StoreN_void_ptr:
.LFB4:
	.cfi_startproc
	movq	%rsi, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE4:
	.size	SCOREP_Atomic_StoreN_void_ptr, .-SCOREP_Atomic_StoreN_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_void_ptr
	.type	SCOREP_Atomic_ExchangeN_void_ptr, @function
SCOREP_Atomic_ExchangeN_void_ptr:
.LFB5:
	.cfi_startproc
	movq	%rsi, %rax
	xchgq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE5:
	.size	SCOREP_Atomic_ExchangeN_void_ptr, .-SCOREP_Atomic_ExchangeN_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_void_ptr
	.type	SCOREP_Atomic_CompareExchangeN_void_ptr, @function
SCOREP_Atomic_CompareExchangeN_void_ptr:
.LFB6:
	.cfi_startproc
	movq	(%rsi), %rax
	lock cmpxchgq	%rdx, (%rdi)
	movq	%rax, %rdx
	sete	%al
	je	.L8
	movq	%rdx, (%rsi)
.L8:
	rep ret
	.cfi_endproc
.LFE6:
	.size	SCOREP_Atomic_CompareExchangeN_void_ptr, .-SCOREP_Atomic_CompareExchangeN_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_void_ptr
	.type	SCOREP_Atomic_AddFetch_void_ptr, @function
SCOREP_Atomic_AddFetch_void_ptr:
.LFB7:
	.cfi_startproc
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	addq	%rsi, %rax
	ret
	.cfi_endproc
.LFE7:
	.size	SCOREP_Atomic_AddFetch_void_ptr, .-SCOREP_Atomic_AddFetch_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_void_ptr
	.type	SCOREP_Atomic_SubFetch_void_ptr, @function
SCOREP_Atomic_SubFetch_void_ptr:
.LFB8:
	.cfi_startproc
	negq	%rsi
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	addq	%rsi, %rax
	ret
	.cfi_endproc
.LFE8:
	.size	SCOREP_Atomic_SubFetch_void_ptr, .-SCOREP_Atomic_SubFetch_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_void_ptr
	.type	SCOREP_Atomic_AndFetch_void_ptr, @function
SCOREP_Atomic_AndFetch_void_ptr:
.LFB9:
	.cfi_startproc
	movq	(%rdi), %rax
.L13:
	movq	%rsi, %rdx
	andq	%rax, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L13
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE9:
	.size	SCOREP_Atomic_AndFetch_void_ptr, .-SCOREP_Atomic_AndFetch_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_void_ptr
	.type	SCOREP_Atomic_XorFetch_void_ptr, @function
SCOREP_Atomic_XorFetch_void_ptr:
.LFB10:
	.cfi_startproc
	movq	(%rdi), %rax
.L17:
	movq	%rsi, %rdx
	xorq	%rax, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L17
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE10:
	.size	SCOREP_Atomic_XorFetch_void_ptr, .-SCOREP_Atomic_XorFetch_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_void_ptr
	.type	SCOREP_Atomic_OrFetch_void_ptr, @function
SCOREP_Atomic_OrFetch_void_ptr:
.LFB11:
	.cfi_startproc
	movq	(%rdi), %rax
.L21:
	movq	%rsi, %rdx
	orq	%rax, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L21
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE11:
	.size	SCOREP_Atomic_OrFetch_void_ptr, .-SCOREP_Atomic_OrFetch_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_void_ptr
	.type	SCOREP_Atomic_NandFetch_void_ptr, @function
SCOREP_Atomic_NandFetch_void_ptr:
.LFB12:
	.cfi_startproc
	movq	(%rdi), %rax
.L25:
	movq	%rsi, %rdx
	andq	%rax, %rdx
	notq	%rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L25
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE12:
	.size	SCOREP_Atomic_NandFetch_void_ptr, .-SCOREP_Atomic_NandFetch_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_void_ptr
	.type	SCOREP_Atomic_FetchAdd_void_ptr, @function
SCOREP_Atomic_FetchAdd_void_ptr:
.LFB13:
	.cfi_startproc
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE13:
	.size	SCOREP_Atomic_FetchAdd_void_ptr, .-SCOREP_Atomic_FetchAdd_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_void_ptr
	.type	SCOREP_Atomic_FetchSub_void_ptr, @function
SCOREP_Atomic_FetchSub_void_ptr:
.LFB14:
	.cfi_startproc
	movq	%rsi, %rax
	negq	%rax
	lock xaddq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE14:
	.size	SCOREP_Atomic_FetchSub_void_ptr, .-SCOREP_Atomic_FetchSub_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_void_ptr
	.type	SCOREP_Atomic_FetchAnd_void_ptr, @function
SCOREP_Atomic_FetchAnd_void_ptr:
.LFB15:
	.cfi_startproc
	movq	(%rdi), %rax
.L31:
	movq	%rsi, %rcx
	movq	%rax, %rdx
	andq	%rax, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L31
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE15:
	.size	SCOREP_Atomic_FetchAnd_void_ptr, .-SCOREP_Atomic_FetchAnd_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_void_ptr
	.type	SCOREP_Atomic_FetchXor_void_ptr, @function
SCOREP_Atomic_FetchXor_void_ptr:
.LFB16:
	.cfi_startproc
	movq	(%rdi), %rax
.L35:
	movq	%rsi, %rcx
	movq	%rax, %rdx
	xorq	%rax, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L35
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE16:
	.size	SCOREP_Atomic_FetchXor_void_ptr, .-SCOREP_Atomic_FetchXor_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_void_ptr
	.type	SCOREP_Atomic_FetchOr_void_ptr, @function
SCOREP_Atomic_FetchOr_void_ptr:
.LFB17:
	.cfi_startproc
	movq	(%rdi), %rax
.L39:
	movq	%rsi, %rcx
	movq	%rax, %rdx
	orq	%rax, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L39
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE17:
	.size	SCOREP_Atomic_FetchOr_void_ptr, .-SCOREP_Atomic_FetchOr_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_void_ptr
	.type	SCOREP_Atomic_FetchNand_void_ptr, @function
SCOREP_Atomic_FetchNand_void_ptr:
.LFB18:
	.cfi_startproc
	movq	(%rdi), %rax
.L43:
	movq	%rsi, %rdx
	movq	%rax, %rcx
	andq	%rax, %rdx
	notq	%rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L43
	movq	%rcx, %rax
	ret
	.cfi_endproc
.LFE18:
	.size	SCOREP_Atomic_FetchNand_void_ptr, .-SCOREP_Atomic_FetchNand_void_ptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_bool
	.type	SCOREP_Atomic_LoadN_bool, @function
SCOREP_Atomic_LoadN_bool:
.LFB19:
	.cfi_startproc
	movzbl	(%rdi), %eax
	testb	%al, %al
	setne	%al
	ret
	.cfi_endproc
.LFE19:
	.size	SCOREP_Atomic_LoadN_bool, .-SCOREP_Atomic_LoadN_bool
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_bool
	.type	SCOREP_Atomic_StoreN_bool, @function
SCOREP_Atomic_StoreN_bool:
.LFB20:
	.cfi_startproc
	movb	%sil, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE20:
	.size	SCOREP_Atomic_StoreN_bool, .-SCOREP_Atomic_StoreN_bool
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_bool
	.type	SCOREP_Atomic_ExchangeN_bool, @function
SCOREP_Atomic_ExchangeN_bool:
.LFB21:
	.cfi_startproc
	movzbl	%sil, %esi
	xchgb	(%rdi), %sil
	testb	%sil, %sil
	setne	%al
	ret
	.cfi_endproc
.LFE21:
	.size	SCOREP_Atomic_ExchangeN_bool, .-SCOREP_Atomic_ExchangeN_bool
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_bool
	.type	SCOREP_Atomic_CompareExchangeN_bool, @function
SCOREP_Atomic_CompareExchangeN_bool:
.LFB22:
	.cfi_startproc
	movzbl	(%rsi), %eax
	movzbl	%dl, %edx
	lock cmpxchgb	%dl, (%rdi)
	movl	%eax, %edx
	sete	%al
	je	.L49
	movb	%dl, (%rsi)
.L49:
	rep ret
	.cfi_endproc
.LFE22:
	.size	SCOREP_Atomic_CompareExchangeN_bool, .-SCOREP_Atomic_CompareExchangeN_bool
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_char
	.type	SCOREP_Atomic_LoadN_char, @function
SCOREP_Atomic_LoadN_char:
.LFB23:
	.cfi_startproc
	movzbl	(%rdi), %eax
	ret
	.cfi_endproc
.LFE23:
	.size	SCOREP_Atomic_LoadN_char, .-SCOREP_Atomic_LoadN_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_char
	.type	SCOREP_Atomic_StoreN_char, @function
SCOREP_Atomic_StoreN_char:
.LFB24:
	.cfi_startproc
	movb	%sil, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE24:
	.size	SCOREP_Atomic_StoreN_char, .-SCOREP_Atomic_StoreN_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_char
	.type	SCOREP_Atomic_ExchangeN_char, @function
SCOREP_Atomic_ExchangeN_char:
.LFB25:
	.cfi_startproc
	movzbl	%sil, %eax
	xchgb	(%rdi), %al
	ret
	.cfi_endproc
.LFE25:
	.size	SCOREP_Atomic_ExchangeN_char, .-SCOREP_Atomic_ExchangeN_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_char
	.type	SCOREP_Atomic_CompareExchangeN_char, @function
SCOREP_Atomic_CompareExchangeN_char:
.LFB26:
	.cfi_startproc
	movzbl	(%rsi), %eax
	movzbl	%dl, %edx
	lock cmpxchgb	%dl, (%rdi)
	movl	%eax, %edx
	sete	%al
	je	.L54
	movb	%dl, (%rsi)
.L54:
	rep ret
	.cfi_endproc
.LFE26:
	.size	SCOREP_Atomic_CompareExchangeN_char, .-SCOREP_Atomic_CompareExchangeN_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_char
	.type	SCOREP_Atomic_AddFetch_char, @function
SCOREP_Atomic_AddFetch_char:
.LFB27:
	.cfi_startproc
	movzbl	%sil, %esi
	movl	%esi, %eax
	lock xaddb	%al, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE27:
	.size	SCOREP_Atomic_AddFetch_char, .-SCOREP_Atomic_AddFetch_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_char
	.type	SCOREP_Atomic_SubFetch_char, @function
SCOREP_Atomic_SubFetch_char:
.LFB28:
	.cfi_startproc
	negl	%esi
	movl	%esi, %eax
	lock xaddb	%al, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE28:
	.size	SCOREP_Atomic_SubFetch_char, .-SCOREP_Atomic_SubFetch_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_char
	.type	SCOREP_Atomic_AndFetch_char, @function
SCOREP_Atomic_AndFetch_char:
.LFB29:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L59:
	movl	%eax, %edx
	andl	%esi, %edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L59
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE29:
	.size	SCOREP_Atomic_AndFetch_char, .-SCOREP_Atomic_AndFetch_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_char
	.type	SCOREP_Atomic_XorFetch_char, @function
SCOREP_Atomic_XorFetch_char:
.LFB30:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L63:
	movl	%eax, %edx
	xorl	%esi, %edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L63
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE30:
	.size	SCOREP_Atomic_XorFetch_char, .-SCOREP_Atomic_XorFetch_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_char
	.type	SCOREP_Atomic_OrFetch_char, @function
SCOREP_Atomic_OrFetch_char:
.LFB31:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L67:
	movl	%eax, %edx
	orl	%esi, %edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L67
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE31:
	.size	SCOREP_Atomic_OrFetch_char, .-SCOREP_Atomic_OrFetch_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_char
	.type	SCOREP_Atomic_NandFetch_char, @function
SCOREP_Atomic_NandFetch_char:
.LFB32:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L71:
	movl	%eax, %edx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L71
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE32:
	.size	SCOREP_Atomic_NandFetch_char, .-SCOREP_Atomic_NandFetch_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_char
	.type	SCOREP_Atomic_FetchAdd_char, @function
SCOREP_Atomic_FetchAdd_char:
.LFB33:
	.cfi_startproc
	movzbl	%sil, %eax
	lock xaddb	%al, (%rdi)
	ret
	.cfi_endproc
.LFE33:
	.size	SCOREP_Atomic_FetchAdd_char, .-SCOREP_Atomic_FetchAdd_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_char
	.type	SCOREP_Atomic_FetchSub_char, @function
SCOREP_Atomic_FetchSub_char:
.LFB34:
	.cfi_startproc
	movl	%esi, %eax
	negl	%eax
	lock xaddb	%al, (%rdi)
	ret
	.cfi_endproc
.LFE34:
	.size	SCOREP_Atomic_FetchSub_char, .-SCOREP_Atomic_FetchSub_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_char
	.type	SCOREP_Atomic_FetchAnd_char, @function
SCOREP_Atomic_FetchAnd_char:
.LFB35:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L77:
	movl	%eax, %ecx
	movl	%eax, %edx
	andl	%esi, %ecx
	lock cmpxchgb	%cl, (%rdi)
	jne	.L77
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE35:
	.size	SCOREP_Atomic_FetchAnd_char, .-SCOREP_Atomic_FetchAnd_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_char
	.type	SCOREP_Atomic_FetchXor_char, @function
SCOREP_Atomic_FetchXor_char:
.LFB36:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L81:
	movl	%eax, %ecx
	movl	%eax, %edx
	xorl	%esi, %ecx
	lock cmpxchgb	%cl, (%rdi)
	jne	.L81
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE36:
	.size	SCOREP_Atomic_FetchXor_char, .-SCOREP_Atomic_FetchXor_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_char
	.type	SCOREP_Atomic_FetchOr_char, @function
SCOREP_Atomic_FetchOr_char:
.LFB37:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L85:
	movl	%eax, %ecx
	movl	%eax, %edx
	orl	%esi, %ecx
	lock cmpxchgb	%cl, (%rdi)
	jne	.L85
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE37:
	.size	SCOREP_Atomic_FetchOr_char, .-SCOREP_Atomic_FetchOr_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_char
	.type	SCOREP_Atomic_FetchNand_char, @function
SCOREP_Atomic_FetchNand_char:
.LFB38:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L89:
	movl	%eax, %edx
	movl	%eax, %ecx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L89
	movl	%ecx, %eax
	ret
	.cfi_endproc
.LFE38:
	.size	SCOREP_Atomic_FetchNand_char, .-SCOREP_Atomic_FetchNand_char
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_uint8
	.type	SCOREP_Atomic_LoadN_uint8, @function
SCOREP_Atomic_LoadN_uint8:
.LFB39:
	.cfi_startproc
	movzbl	(%rdi), %eax
	ret
	.cfi_endproc
.LFE39:
	.size	SCOREP_Atomic_LoadN_uint8, .-SCOREP_Atomic_LoadN_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_uint8
	.type	SCOREP_Atomic_StoreN_uint8, @function
SCOREP_Atomic_StoreN_uint8:
.LFB40:
	.cfi_startproc
	movb	%sil, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE40:
	.size	SCOREP_Atomic_StoreN_uint8, .-SCOREP_Atomic_StoreN_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_uint8
	.type	SCOREP_Atomic_ExchangeN_uint8, @function
SCOREP_Atomic_ExchangeN_uint8:
.LFB41:
	.cfi_startproc
	movzbl	%sil, %eax
	xchgb	(%rdi), %al
	ret
	.cfi_endproc
.LFE41:
	.size	SCOREP_Atomic_ExchangeN_uint8, .-SCOREP_Atomic_ExchangeN_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_uint8
	.type	SCOREP_Atomic_CompareExchangeN_uint8, @function
SCOREP_Atomic_CompareExchangeN_uint8:
.LFB42:
	.cfi_startproc
	movzbl	(%rsi), %eax
	movzbl	%dl, %edx
	lock cmpxchgb	%dl, (%rdi)
	movl	%eax, %edx
	sete	%al
	je	.L95
	movb	%dl, (%rsi)
.L95:
	rep ret
	.cfi_endproc
.LFE42:
	.size	SCOREP_Atomic_CompareExchangeN_uint8, .-SCOREP_Atomic_CompareExchangeN_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_uint8
	.type	SCOREP_Atomic_AddFetch_uint8, @function
SCOREP_Atomic_AddFetch_uint8:
.LFB43:
	.cfi_startproc
	movzbl	%sil, %esi
	movl	%esi, %eax
	lock xaddb	%al, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE43:
	.size	SCOREP_Atomic_AddFetch_uint8, .-SCOREP_Atomic_AddFetch_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_uint8
	.type	SCOREP_Atomic_SubFetch_uint8, @function
SCOREP_Atomic_SubFetch_uint8:
.LFB44:
	.cfi_startproc
	negl	%esi
	movl	%esi, %eax
	lock xaddb	%al, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE44:
	.size	SCOREP_Atomic_SubFetch_uint8, .-SCOREP_Atomic_SubFetch_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_uint8
	.type	SCOREP_Atomic_AndFetch_uint8, @function
SCOREP_Atomic_AndFetch_uint8:
.LFB45:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L100:
	movl	%eax, %edx
	andl	%esi, %edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L100
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE45:
	.size	SCOREP_Atomic_AndFetch_uint8, .-SCOREP_Atomic_AndFetch_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_uint8
	.type	SCOREP_Atomic_XorFetch_uint8, @function
SCOREP_Atomic_XorFetch_uint8:
.LFB46:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L104:
	movl	%eax, %edx
	xorl	%esi, %edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L104
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE46:
	.size	SCOREP_Atomic_XorFetch_uint8, .-SCOREP_Atomic_XorFetch_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_uint8
	.type	SCOREP_Atomic_OrFetch_uint8, @function
SCOREP_Atomic_OrFetch_uint8:
.LFB47:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L108:
	movl	%eax, %edx
	orl	%esi, %edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L108
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE47:
	.size	SCOREP_Atomic_OrFetch_uint8, .-SCOREP_Atomic_OrFetch_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_uint8
	.type	SCOREP_Atomic_NandFetch_uint8, @function
SCOREP_Atomic_NandFetch_uint8:
.LFB48:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L112:
	movl	%eax, %edx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L112
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE48:
	.size	SCOREP_Atomic_NandFetch_uint8, .-SCOREP_Atomic_NandFetch_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_uint8
	.type	SCOREP_Atomic_FetchAdd_uint8, @function
SCOREP_Atomic_FetchAdd_uint8:
.LFB49:
	.cfi_startproc
	movzbl	%sil, %eax
	lock xaddb	%al, (%rdi)
	ret
	.cfi_endproc
.LFE49:
	.size	SCOREP_Atomic_FetchAdd_uint8, .-SCOREP_Atomic_FetchAdd_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_uint8
	.type	SCOREP_Atomic_FetchSub_uint8, @function
SCOREP_Atomic_FetchSub_uint8:
.LFB50:
	.cfi_startproc
	movl	%esi, %eax
	negl	%eax
	lock xaddb	%al, (%rdi)
	ret
	.cfi_endproc
.LFE50:
	.size	SCOREP_Atomic_FetchSub_uint8, .-SCOREP_Atomic_FetchSub_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_uint8
	.type	SCOREP_Atomic_FetchAnd_uint8, @function
SCOREP_Atomic_FetchAnd_uint8:
.LFB51:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L118:
	movl	%eax, %ecx
	movl	%eax, %edx
	andl	%esi, %ecx
	lock cmpxchgb	%cl, (%rdi)
	jne	.L118
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE51:
	.size	SCOREP_Atomic_FetchAnd_uint8, .-SCOREP_Atomic_FetchAnd_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_uint8
	.type	SCOREP_Atomic_FetchXor_uint8, @function
SCOREP_Atomic_FetchXor_uint8:
.LFB52:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L122:
	movl	%eax, %ecx
	movl	%eax, %edx
	xorl	%esi, %ecx
	lock cmpxchgb	%cl, (%rdi)
	jne	.L122
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE52:
	.size	SCOREP_Atomic_FetchXor_uint8, .-SCOREP_Atomic_FetchXor_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_uint8
	.type	SCOREP_Atomic_FetchOr_uint8, @function
SCOREP_Atomic_FetchOr_uint8:
.LFB53:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L126:
	movl	%eax, %ecx
	movl	%eax, %edx
	orl	%esi, %ecx
	lock cmpxchgb	%cl, (%rdi)
	jne	.L126
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE53:
	.size	SCOREP_Atomic_FetchOr_uint8, .-SCOREP_Atomic_FetchOr_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_uint8
	.type	SCOREP_Atomic_FetchNand_uint8, @function
SCOREP_Atomic_FetchNand_uint8:
.LFB54:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L130:
	movl	%eax, %edx
	movl	%eax, %ecx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L130
	movl	%ecx, %eax
	ret
	.cfi_endproc
.LFE54:
	.size	SCOREP_Atomic_FetchNand_uint8, .-SCOREP_Atomic_FetchNand_uint8
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_uint16
	.type	SCOREP_Atomic_LoadN_uint16, @function
SCOREP_Atomic_LoadN_uint16:
.LFB55:
	.cfi_startproc
	movzwl	(%rdi), %eax
	ret
	.cfi_endproc
.LFE55:
	.size	SCOREP_Atomic_LoadN_uint16, .-SCOREP_Atomic_LoadN_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_uint16
	.type	SCOREP_Atomic_StoreN_uint16, @function
SCOREP_Atomic_StoreN_uint16:
.LFB56:
	.cfi_startproc
	movw	%si, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE56:
	.size	SCOREP_Atomic_StoreN_uint16, .-SCOREP_Atomic_StoreN_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_uint16
	.type	SCOREP_Atomic_ExchangeN_uint16, @function
SCOREP_Atomic_ExchangeN_uint16:
.LFB57:
	.cfi_startproc
	movzwl	%si, %eax
	xchgw	(%rdi), %ax
	ret
	.cfi_endproc
.LFE57:
	.size	SCOREP_Atomic_ExchangeN_uint16, .-SCOREP_Atomic_ExchangeN_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_uint16
	.type	SCOREP_Atomic_CompareExchangeN_uint16, @function
SCOREP_Atomic_CompareExchangeN_uint16:
.LFB58:
	.cfi_startproc
	movzwl	(%rsi), %eax
	movzwl	%dx, %edx
	lock cmpxchgw	%dx, (%rdi)
	movl	%eax, %edx
	sete	%al
	je	.L136
	movw	%dx, (%rsi)
.L136:
	rep ret
	.cfi_endproc
.LFE58:
	.size	SCOREP_Atomic_CompareExchangeN_uint16, .-SCOREP_Atomic_CompareExchangeN_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_uint16
	.type	SCOREP_Atomic_AddFetch_uint16, @function
SCOREP_Atomic_AddFetch_uint16:
.LFB59:
	.cfi_startproc
	movzwl	%si, %esi
	movl	%esi, %eax
	lock xaddw	%ax, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE59:
	.size	SCOREP_Atomic_AddFetch_uint16, .-SCOREP_Atomic_AddFetch_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_uint16
	.type	SCOREP_Atomic_SubFetch_uint16, @function
SCOREP_Atomic_SubFetch_uint16:
.LFB60:
	.cfi_startproc
	negl	%esi
	movl	%esi, %eax
	lock xaddw	%ax, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE60:
	.size	SCOREP_Atomic_SubFetch_uint16, .-SCOREP_Atomic_SubFetch_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_uint16
	.type	SCOREP_Atomic_AndFetch_uint16, @function
SCOREP_Atomic_AndFetch_uint16:
.LFB61:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L141:
	movl	%eax, %edx
	andl	%esi, %edx
	lock cmpxchgw	%dx, (%rdi)
	jne	.L141
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE61:
	.size	SCOREP_Atomic_AndFetch_uint16, .-SCOREP_Atomic_AndFetch_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_uint16
	.type	SCOREP_Atomic_XorFetch_uint16, @function
SCOREP_Atomic_XorFetch_uint16:
.LFB62:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L145:
	movl	%eax, %edx
	xorl	%esi, %edx
	lock cmpxchgw	%dx, (%rdi)
	jne	.L145
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE62:
	.size	SCOREP_Atomic_XorFetch_uint16, .-SCOREP_Atomic_XorFetch_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_uint16
	.type	SCOREP_Atomic_OrFetch_uint16, @function
SCOREP_Atomic_OrFetch_uint16:
.LFB63:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L149:
	movl	%eax, %edx
	orl	%esi, %edx
	lock cmpxchgw	%dx, (%rdi)
	jne	.L149
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE63:
	.size	SCOREP_Atomic_OrFetch_uint16, .-SCOREP_Atomic_OrFetch_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_uint16
	.type	SCOREP_Atomic_NandFetch_uint16, @function
SCOREP_Atomic_NandFetch_uint16:
.LFB64:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L153:
	movl	%eax, %edx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgw	%dx, (%rdi)
	jne	.L153
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE64:
	.size	SCOREP_Atomic_NandFetch_uint16, .-SCOREP_Atomic_NandFetch_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_uint16
	.type	SCOREP_Atomic_FetchAdd_uint16, @function
SCOREP_Atomic_FetchAdd_uint16:
.LFB65:
	.cfi_startproc
	movzwl	%si, %eax
	lock xaddw	%ax, (%rdi)
	ret
	.cfi_endproc
.LFE65:
	.size	SCOREP_Atomic_FetchAdd_uint16, .-SCOREP_Atomic_FetchAdd_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_uint16
	.type	SCOREP_Atomic_FetchSub_uint16, @function
SCOREP_Atomic_FetchSub_uint16:
.LFB66:
	.cfi_startproc
	movl	%esi, %eax
	negl	%eax
	lock xaddw	%ax, (%rdi)
	ret
	.cfi_endproc
.LFE66:
	.size	SCOREP_Atomic_FetchSub_uint16, .-SCOREP_Atomic_FetchSub_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_uint16
	.type	SCOREP_Atomic_FetchAnd_uint16, @function
SCOREP_Atomic_FetchAnd_uint16:
.LFB67:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L159:
	movl	%eax, %ecx
	movl	%eax, %edx
	andl	%esi, %ecx
	lock cmpxchgw	%cx, (%rdi)
	jne	.L159
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE67:
	.size	SCOREP_Atomic_FetchAnd_uint16, .-SCOREP_Atomic_FetchAnd_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_uint16
	.type	SCOREP_Atomic_FetchXor_uint16, @function
SCOREP_Atomic_FetchXor_uint16:
.LFB68:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L163:
	movl	%eax, %ecx
	movl	%eax, %edx
	xorl	%esi, %ecx
	lock cmpxchgw	%cx, (%rdi)
	jne	.L163
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE68:
	.size	SCOREP_Atomic_FetchXor_uint16, .-SCOREP_Atomic_FetchXor_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_uint16
	.type	SCOREP_Atomic_FetchOr_uint16, @function
SCOREP_Atomic_FetchOr_uint16:
.LFB69:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L167:
	movl	%eax, %ecx
	movl	%eax, %edx
	orl	%esi, %ecx
	lock cmpxchgw	%cx, (%rdi)
	jne	.L167
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE69:
	.size	SCOREP_Atomic_FetchOr_uint16, .-SCOREP_Atomic_FetchOr_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_uint16
	.type	SCOREP_Atomic_FetchNand_uint16, @function
SCOREP_Atomic_FetchNand_uint16:
.LFB70:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L171:
	movl	%eax, %edx
	movl	%eax, %ecx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgw	%dx, (%rdi)
	jne	.L171
	movl	%ecx, %eax
	ret
	.cfi_endproc
.LFE70:
	.size	SCOREP_Atomic_FetchNand_uint16, .-SCOREP_Atomic_FetchNand_uint16
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_uint32
	.type	SCOREP_Atomic_LoadN_uint32, @function
SCOREP_Atomic_LoadN_uint32:
.LFB71:
	.cfi_startproc
	movl	(%rdi), %eax
	ret
	.cfi_endproc
.LFE71:
	.size	SCOREP_Atomic_LoadN_uint32, .-SCOREP_Atomic_LoadN_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_uint32
	.type	SCOREP_Atomic_StoreN_uint32, @function
SCOREP_Atomic_StoreN_uint32:
.LFB72:
	.cfi_startproc
	movl	%esi, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE72:
	.size	SCOREP_Atomic_StoreN_uint32, .-SCOREP_Atomic_StoreN_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_uint32
	.type	SCOREP_Atomic_ExchangeN_uint32, @function
SCOREP_Atomic_ExchangeN_uint32:
.LFB73:
	.cfi_startproc
	movl	%esi, %eax
	xchgl	(%rdi), %eax
	ret
	.cfi_endproc
.LFE73:
	.size	SCOREP_Atomic_ExchangeN_uint32, .-SCOREP_Atomic_ExchangeN_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_uint32
	.type	SCOREP_Atomic_CompareExchangeN_uint32, @function
SCOREP_Atomic_CompareExchangeN_uint32:
.LFB74:
	.cfi_startproc
	movl	(%rsi), %eax
	lock cmpxchgl	%edx, (%rdi)
	movl	%eax, %edx
	sete	%al
	je	.L177
	movl	%edx, (%rsi)
.L177:
	rep ret
	.cfi_endproc
.LFE74:
	.size	SCOREP_Atomic_CompareExchangeN_uint32, .-SCOREP_Atomic_CompareExchangeN_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_uint32
	.type	SCOREP_Atomic_AddFetch_uint32, @function
SCOREP_Atomic_AddFetch_uint32:
.LFB75:
	.cfi_startproc
	movl	%esi, %eax
	lock xaddl	%eax, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE75:
	.size	SCOREP_Atomic_AddFetch_uint32, .-SCOREP_Atomic_AddFetch_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_uint32
	.type	SCOREP_Atomic_SubFetch_uint32, @function
SCOREP_Atomic_SubFetch_uint32:
.LFB76:
	.cfi_startproc
	negl	%esi
	movl	%esi, %eax
	lock xaddl	%eax, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE76:
	.size	SCOREP_Atomic_SubFetch_uint32, .-SCOREP_Atomic_SubFetch_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_uint32
	.type	SCOREP_Atomic_AndFetch_uint32, @function
SCOREP_Atomic_AndFetch_uint32:
.LFB77:
	.cfi_startproc
	movl	(%rdi), %eax
.L182:
	movl	%eax, %edx
	andl	%esi, %edx
	lock cmpxchgl	%edx, (%rdi)
	jne	.L182
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE77:
	.size	SCOREP_Atomic_AndFetch_uint32, .-SCOREP_Atomic_AndFetch_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_uint32
	.type	SCOREP_Atomic_XorFetch_uint32, @function
SCOREP_Atomic_XorFetch_uint32:
.LFB78:
	.cfi_startproc
	movl	(%rdi), %eax
.L186:
	movl	%eax, %edx
	xorl	%esi, %edx
	lock cmpxchgl	%edx, (%rdi)
	jne	.L186
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE78:
	.size	SCOREP_Atomic_XorFetch_uint32, .-SCOREP_Atomic_XorFetch_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_uint32
	.type	SCOREP_Atomic_OrFetch_uint32, @function
SCOREP_Atomic_OrFetch_uint32:
.LFB79:
	.cfi_startproc
	movl	(%rdi), %eax
.L190:
	movl	%eax, %edx
	orl	%esi, %edx
	lock cmpxchgl	%edx, (%rdi)
	jne	.L190
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE79:
	.size	SCOREP_Atomic_OrFetch_uint32, .-SCOREP_Atomic_OrFetch_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_uint32
	.type	SCOREP_Atomic_NandFetch_uint32, @function
SCOREP_Atomic_NandFetch_uint32:
.LFB80:
	.cfi_startproc
	movl	(%rdi), %eax
.L194:
	movl	%eax, %edx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgl	%edx, (%rdi)
	jne	.L194
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE80:
	.size	SCOREP_Atomic_NandFetch_uint32, .-SCOREP_Atomic_NandFetch_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_uint32
	.type	SCOREP_Atomic_FetchAdd_uint32, @function
SCOREP_Atomic_FetchAdd_uint32:
.LFB81:
	.cfi_startproc
	movl	%esi, %eax
	lock xaddl	%eax, (%rdi)
	ret
	.cfi_endproc
.LFE81:
	.size	SCOREP_Atomic_FetchAdd_uint32, .-SCOREP_Atomic_FetchAdd_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_uint32
	.type	SCOREP_Atomic_FetchSub_uint32, @function
SCOREP_Atomic_FetchSub_uint32:
.LFB82:
	.cfi_startproc
	movl	%esi, %eax
	negl	%eax
	lock xaddl	%eax, (%rdi)
	ret
	.cfi_endproc
.LFE82:
	.size	SCOREP_Atomic_FetchSub_uint32, .-SCOREP_Atomic_FetchSub_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_uint32
	.type	SCOREP_Atomic_FetchAnd_uint32, @function
SCOREP_Atomic_FetchAnd_uint32:
.LFB83:
	.cfi_startproc
	movl	(%rdi), %eax
.L200:
	movl	%eax, %ecx
	movl	%eax, %edx
	andl	%esi, %ecx
	lock cmpxchgl	%ecx, (%rdi)
	jne	.L200
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE83:
	.size	SCOREP_Atomic_FetchAnd_uint32, .-SCOREP_Atomic_FetchAnd_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_uint32
	.type	SCOREP_Atomic_FetchXor_uint32, @function
SCOREP_Atomic_FetchXor_uint32:
.LFB84:
	.cfi_startproc
	movl	(%rdi), %eax
.L204:
	movl	%eax, %ecx
	movl	%eax, %edx
	xorl	%esi, %ecx
	lock cmpxchgl	%ecx, (%rdi)
	jne	.L204
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE84:
	.size	SCOREP_Atomic_FetchXor_uint32, .-SCOREP_Atomic_FetchXor_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_uint32
	.type	SCOREP_Atomic_FetchOr_uint32, @function
SCOREP_Atomic_FetchOr_uint32:
.LFB85:
	.cfi_startproc
	movl	(%rdi), %eax
.L208:
	movl	%eax, %ecx
	movl	%eax, %edx
	orl	%esi, %ecx
	lock cmpxchgl	%ecx, (%rdi)
	jne	.L208
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE85:
	.size	SCOREP_Atomic_FetchOr_uint32, .-SCOREP_Atomic_FetchOr_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_uint32
	.type	SCOREP_Atomic_FetchNand_uint32, @function
SCOREP_Atomic_FetchNand_uint32:
.LFB86:
	.cfi_startproc
	movl	(%rdi), %eax
.L212:
	movl	%eax, %edx
	movl	%eax, %ecx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgl	%edx, (%rdi)
	jne	.L212
	movl	%ecx, %eax
	ret
	.cfi_endproc
.LFE86:
	.size	SCOREP_Atomic_FetchNand_uint32, .-SCOREP_Atomic_FetchNand_uint32
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_uint64
	.type	SCOREP_Atomic_LoadN_uint64, @function
SCOREP_Atomic_LoadN_uint64:
.LFB87:
	.cfi_startproc
	movq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE87:
	.size	SCOREP_Atomic_LoadN_uint64, .-SCOREP_Atomic_LoadN_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_uint64
	.type	SCOREP_Atomic_StoreN_uint64, @function
SCOREP_Atomic_StoreN_uint64:
.LFB88:
	.cfi_startproc
	movq	%rsi, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE88:
	.size	SCOREP_Atomic_StoreN_uint64, .-SCOREP_Atomic_StoreN_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_uint64
	.type	SCOREP_Atomic_ExchangeN_uint64, @function
SCOREP_Atomic_ExchangeN_uint64:
.LFB89:
	.cfi_startproc
	movq	%rsi, %rax
	xchgq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE89:
	.size	SCOREP_Atomic_ExchangeN_uint64, .-SCOREP_Atomic_ExchangeN_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_uint64
	.type	SCOREP_Atomic_CompareExchangeN_uint64, @function
SCOREP_Atomic_CompareExchangeN_uint64:
.LFB90:
	.cfi_startproc
	movq	(%rsi), %rax
	lock cmpxchgq	%rdx, (%rdi)
	movq	%rax, %rdx
	sete	%al
	je	.L218
	movq	%rdx, (%rsi)
.L218:
	rep ret
	.cfi_endproc
.LFE90:
	.size	SCOREP_Atomic_CompareExchangeN_uint64, .-SCOREP_Atomic_CompareExchangeN_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_uint64
	.type	SCOREP_Atomic_AddFetch_uint64, @function
SCOREP_Atomic_AddFetch_uint64:
.LFB91:
	.cfi_startproc
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	addq	%rsi, %rax
	ret
	.cfi_endproc
.LFE91:
	.size	SCOREP_Atomic_AddFetch_uint64, .-SCOREP_Atomic_AddFetch_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_uint64
	.type	SCOREP_Atomic_SubFetch_uint64, @function
SCOREP_Atomic_SubFetch_uint64:
.LFB92:
	.cfi_startproc
	negq	%rsi
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	addq	%rsi, %rax
	ret
	.cfi_endproc
.LFE92:
	.size	SCOREP_Atomic_SubFetch_uint64, .-SCOREP_Atomic_SubFetch_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_uint64
	.type	SCOREP_Atomic_AndFetch_uint64, @function
SCOREP_Atomic_AndFetch_uint64:
.LFB93:
	.cfi_startproc
	movq	(%rdi), %rax
.L223:
	movq	%rax, %rdx
	andq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L223
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE93:
	.size	SCOREP_Atomic_AndFetch_uint64, .-SCOREP_Atomic_AndFetch_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_uint64
	.type	SCOREP_Atomic_XorFetch_uint64, @function
SCOREP_Atomic_XorFetch_uint64:
.LFB94:
	.cfi_startproc
	movq	(%rdi), %rax
.L227:
	movq	%rax, %rdx
	xorq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L227
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE94:
	.size	SCOREP_Atomic_XorFetch_uint64, .-SCOREP_Atomic_XorFetch_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_uint64
	.type	SCOREP_Atomic_OrFetch_uint64, @function
SCOREP_Atomic_OrFetch_uint64:
.LFB95:
	.cfi_startproc
	movq	(%rdi), %rax
.L231:
	movq	%rax, %rdx
	orq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L231
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE95:
	.size	SCOREP_Atomic_OrFetch_uint64, .-SCOREP_Atomic_OrFetch_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_uint64
	.type	SCOREP_Atomic_NandFetch_uint64, @function
SCOREP_Atomic_NandFetch_uint64:
.LFB96:
	.cfi_startproc
	movq	(%rdi), %rax
.L235:
	movq	%rax, %rdx
	andq	%rsi, %rdx
	notq	%rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L235
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE96:
	.size	SCOREP_Atomic_NandFetch_uint64, .-SCOREP_Atomic_NandFetch_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_uint64
	.type	SCOREP_Atomic_FetchAdd_uint64, @function
SCOREP_Atomic_FetchAdd_uint64:
.LFB97:
	.cfi_startproc
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE97:
	.size	SCOREP_Atomic_FetchAdd_uint64, .-SCOREP_Atomic_FetchAdd_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_uint64
	.type	SCOREP_Atomic_FetchSub_uint64, @function
SCOREP_Atomic_FetchSub_uint64:
.LFB98:
	.cfi_startproc
	movq	%rsi, %rax
	negq	%rax
	lock xaddq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE98:
	.size	SCOREP_Atomic_FetchSub_uint64, .-SCOREP_Atomic_FetchSub_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_uint64
	.type	SCOREP_Atomic_FetchAnd_uint64, @function
SCOREP_Atomic_FetchAnd_uint64:
.LFB99:
	.cfi_startproc
	movq	(%rdi), %rax
.L241:
	movq	%rax, %rcx
	movq	%rax, %rdx
	andq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L241
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE99:
	.size	SCOREP_Atomic_FetchAnd_uint64, .-SCOREP_Atomic_FetchAnd_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_uint64
	.type	SCOREP_Atomic_FetchXor_uint64, @function
SCOREP_Atomic_FetchXor_uint64:
.LFB100:
	.cfi_startproc
	movq	(%rdi), %rax
.L245:
	movq	%rax, %rcx
	movq	%rax, %rdx
	xorq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L245
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE100:
	.size	SCOREP_Atomic_FetchXor_uint64, .-SCOREP_Atomic_FetchXor_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_uint64
	.type	SCOREP_Atomic_FetchOr_uint64, @function
SCOREP_Atomic_FetchOr_uint64:
.LFB101:
	.cfi_startproc
	movq	(%rdi), %rax
.L249:
	movq	%rax, %rcx
	movq	%rax, %rdx
	orq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L249
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE101:
	.size	SCOREP_Atomic_FetchOr_uint64, .-SCOREP_Atomic_FetchOr_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_uint64
	.type	SCOREP_Atomic_FetchNand_uint64, @function
SCOREP_Atomic_FetchNand_uint64:
.LFB102:
	.cfi_startproc
	movq	(%rdi), %rax
.L253:
	movq	%rax, %rdx
	movq	%rax, %rcx
	andq	%rsi, %rdx
	notq	%rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L253
	movq	%rcx, %rax
	ret
	.cfi_endproc
.LFE102:
	.size	SCOREP_Atomic_FetchNand_uint64, .-SCOREP_Atomic_FetchNand_uint64
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_uintptr
	.type	SCOREP_Atomic_LoadN_uintptr, @function
SCOREP_Atomic_LoadN_uintptr:
.LFB200:
	.cfi_startproc
	movq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE200:
	.size	SCOREP_Atomic_LoadN_uintptr, .-SCOREP_Atomic_LoadN_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_uintptr
	.type	SCOREP_Atomic_StoreN_uintptr, @function
SCOREP_Atomic_StoreN_uintptr:
.LFB202:
	.cfi_startproc
	movq	%rsi, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE202:
	.size	SCOREP_Atomic_StoreN_uintptr, .-SCOREP_Atomic_StoreN_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_uintptr
	.type	SCOREP_Atomic_ExchangeN_uintptr, @function
SCOREP_Atomic_ExchangeN_uintptr:
.LFB204:
	.cfi_startproc
	movq	%rsi, %rax
	xchgq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE204:
	.size	SCOREP_Atomic_ExchangeN_uintptr, .-SCOREP_Atomic_ExchangeN_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_uintptr
	.type	SCOREP_Atomic_CompareExchangeN_uintptr, @function
SCOREP_Atomic_CompareExchangeN_uintptr:
.LFB230:
	.cfi_startproc
	movq	(%rsi), %rax
	lock cmpxchgq	%rdx, (%rdi)
	movq	%rax, %rdx
	sete	%al
	je	.L259
	movq	%rdx, (%rsi)
.L259:
	rep ret
	.cfi_endproc
.LFE230:
	.size	SCOREP_Atomic_CompareExchangeN_uintptr, .-SCOREP_Atomic_CompareExchangeN_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_uintptr
	.type	SCOREP_Atomic_AddFetch_uintptr, @function
SCOREP_Atomic_AddFetch_uintptr:
.LFB206:
	.cfi_startproc
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	addq	%rsi, %rax
	ret
	.cfi_endproc
.LFE206:
	.size	SCOREP_Atomic_AddFetch_uintptr, .-SCOREP_Atomic_AddFetch_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_uintptr
	.type	SCOREP_Atomic_SubFetch_uintptr, @function
SCOREP_Atomic_SubFetch_uintptr:
.LFB208:
	.cfi_startproc
	negq	%rsi
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	addq	%rsi, %rax
	ret
	.cfi_endproc
.LFE208:
	.size	SCOREP_Atomic_SubFetch_uintptr, .-SCOREP_Atomic_SubFetch_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_uintptr
	.type	SCOREP_Atomic_AndFetch_uintptr, @function
SCOREP_Atomic_AndFetch_uintptr:
.LFB210:
	.cfi_startproc
	movq	(%rdi), %rax
.L264:
	movq	%rax, %rdx
	andq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L264
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE210:
	.size	SCOREP_Atomic_AndFetch_uintptr, .-SCOREP_Atomic_AndFetch_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_uintptr
	.type	SCOREP_Atomic_XorFetch_uintptr, @function
SCOREP_Atomic_XorFetch_uintptr:
.LFB212:
	.cfi_startproc
	movq	(%rdi), %rax
.L268:
	movq	%rax, %rdx
	xorq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L268
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE212:
	.size	SCOREP_Atomic_XorFetch_uintptr, .-SCOREP_Atomic_XorFetch_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_uintptr
	.type	SCOREP_Atomic_OrFetch_uintptr, @function
SCOREP_Atomic_OrFetch_uintptr:
.LFB214:
	.cfi_startproc
	movq	(%rdi), %rax
.L272:
	movq	%rax, %rdx
	orq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L272
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE214:
	.size	SCOREP_Atomic_OrFetch_uintptr, .-SCOREP_Atomic_OrFetch_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_uintptr
	.type	SCOREP_Atomic_NandFetch_uintptr, @function
SCOREP_Atomic_NandFetch_uintptr:
.LFB216:
	.cfi_startproc
	movq	(%rdi), %rax
.L276:
	movq	%rax, %rdx
	andq	%rsi, %rdx
	notq	%rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L276
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE216:
	.size	SCOREP_Atomic_NandFetch_uintptr, .-SCOREP_Atomic_NandFetch_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_uintptr
	.type	SCOREP_Atomic_FetchAdd_uintptr, @function
SCOREP_Atomic_FetchAdd_uintptr:
.LFB218:
	.cfi_startproc
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE218:
	.size	SCOREP_Atomic_FetchAdd_uintptr, .-SCOREP_Atomic_FetchAdd_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_uintptr
	.type	SCOREP_Atomic_FetchSub_uintptr, @function
SCOREP_Atomic_FetchSub_uintptr:
.LFB220:
	.cfi_startproc
	movq	%rsi, %rax
	negq	%rax
	lock xaddq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE220:
	.size	SCOREP_Atomic_FetchSub_uintptr, .-SCOREP_Atomic_FetchSub_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_uintptr
	.type	SCOREP_Atomic_FetchAnd_uintptr, @function
SCOREP_Atomic_FetchAnd_uintptr:
.LFB222:
	.cfi_startproc
	movq	(%rdi), %rax
.L282:
	movq	%rax, %rcx
	movq	%rax, %rdx
	andq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L282
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE222:
	.size	SCOREP_Atomic_FetchAnd_uintptr, .-SCOREP_Atomic_FetchAnd_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_uintptr
	.type	SCOREP_Atomic_FetchXor_uintptr, @function
SCOREP_Atomic_FetchXor_uintptr:
.LFB224:
	.cfi_startproc
	movq	(%rdi), %rax
.L286:
	movq	%rax, %rcx
	movq	%rax, %rdx
	xorq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L286
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE224:
	.size	SCOREP_Atomic_FetchXor_uintptr, .-SCOREP_Atomic_FetchXor_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_uintptr
	.type	SCOREP_Atomic_FetchOr_uintptr, @function
SCOREP_Atomic_FetchOr_uintptr:
.LFB226:
	.cfi_startproc
	movq	(%rdi), %rax
.L290:
	movq	%rax, %rcx
	movq	%rax, %rdx
	orq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L290
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE226:
	.size	SCOREP_Atomic_FetchOr_uintptr, .-SCOREP_Atomic_FetchOr_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_uintptr
	.type	SCOREP_Atomic_FetchNand_uintptr, @function
SCOREP_Atomic_FetchNand_uintptr:
.LFB228:
	.cfi_startproc
	movq	(%rdi), %rax
.L294:
	movq	%rax, %rdx
	movq	%rax, %rcx
	andq	%rsi, %rdx
	notq	%rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L294
	movq	%rcx, %rax
	ret
	.cfi_endproc
.LFE228:
	.size	SCOREP_Atomic_FetchNand_uintptr, .-SCOREP_Atomic_FetchNand_uintptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_int8
	.type	SCOREP_Atomic_LoadN_int8, @function
SCOREP_Atomic_LoadN_int8:
.LFB119:
	.cfi_startproc
	movzbl	(%rdi), %eax
	ret
	.cfi_endproc
.LFE119:
	.size	SCOREP_Atomic_LoadN_int8, .-SCOREP_Atomic_LoadN_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_int8
	.type	SCOREP_Atomic_StoreN_int8, @function
SCOREP_Atomic_StoreN_int8:
.LFB120:
	.cfi_startproc
	movb	%sil, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE120:
	.size	SCOREP_Atomic_StoreN_int8, .-SCOREP_Atomic_StoreN_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_int8
	.type	SCOREP_Atomic_ExchangeN_int8, @function
SCOREP_Atomic_ExchangeN_int8:
.LFB121:
	.cfi_startproc
	movzbl	%sil, %eax
	xchgb	(%rdi), %al
	ret
	.cfi_endproc
.LFE121:
	.size	SCOREP_Atomic_ExchangeN_int8, .-SCOREP_Atomic_ExchangeN_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_int8
	.type	SCOREP_Atomic_CompareExchangeN_int8, @function
SCOREP_Atomic_CompareExchangeN_int8:
.LFB122:
	.cfi_startproc
	movzbl	(%rsi), %eax
	movzbl	%dl, %edx
	lock cmpxchgb	%dl, (%rdi)
	movl	%eax, %edx
	sete	%al
	je	.L300
	movb	%dl, (%rsi)
.L300:
	rep ret
	.cfi_endproc
.LFE122:
	.size	SCOREP_Atomic_CompareExchangeN_int8, .-SCOREP_Atomic_CompareExchangeN_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_int8
	.type	SCOREP_Atomic_AddFetch_int8, @function
SCOREP_Atomic_AddFetch_int8:
.LFB123:
	.cfi_startproc
	movzbl	%sil, %esi
	movl	%esi, %eax
	lock xaddb	%al, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE123:
	.size	SCOREP_Atomic_AddFetch_int8, .-SCOREP_Atomic_AddFetch_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_int8
	.type	SCOREP_Atomic_SubFetch_int8, @function
SCOREP_Atomic_SubFetch_int8:
.LFB124:
	.cfi_startproc
	negl	%esi
	movl	%esi, %eax
	lock xaddb	%al, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE124:
	.size	SCOREP_Atomic_SubFetch_int8, .-SCOREP_Atomic_SubFetch_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_int8
	.type	SCOREP_Atomic_AndFetch_int8, @function
SCOREP_Atomic_AndFetch_int8:
.LFB125:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L305:
	movl	%eax, %edx
	andl	%esi, %edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L305
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE125:
	.size	SCOREP_Atomic_AndFetch_int8, .-SCOREP_Atomic_AndFetch_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_int8
	.type	SCOREP_Atomic_XorFetch_int8, @function
SCOREP_Atomic_XorFetch_int8:
.LFB126:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L309:
	movl	%eax, %edx
	xorl	%esi, %edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L309
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE126:
	.size	SCOREP_Atomic_XorFetch_int8, .-SCOREP_Atomic_XorFetch_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_int8
	.type	SCOREP_Atomic_OrFetch_int8, @function
SCOREP_Atomic_OrFetch_int8:
.LFB127:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L313:
	movl	%eax, %edx
	orl	%esi, %edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L313
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE127:
	.size	SCOREP_Atomic_OrFetch_int8, .-SCOREP_Atomic_OrFetch_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_int8
	.type	SCOREP_Atomic_NandFetch_int8, @function
SCOREP_Atomic_NandFetch_int8:
.LFB128:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L317:
	movl	%eax, %edx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L317
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE128:
	.size	SCOREP_Atomic_NandFetch_int8, .-SCOREP_Atomic_NandFetch_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_int8
	.type	SCOREP_Atomic_FetchAdd_int8, @function
SCOREP_Atomic_FetchAdd_int8:
.LFB129:
	.cfi_startproc
	movzbl	%sil, %eax
	lock xaddb	%al, (%rdi)
	ret
	.cfi_endproc
.LFE129:
	.size	SCOREP_Atomic_FetchAdd_int8, .-SCOREP_Atomic_FetchAdd_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_int8
	.type	SCOREP_Atomic_FetchSub_int8, @function
SCOREP_Atomic_FetchSub_int8:
.LFB130:
	.cfi_startproc
	movl	%esi, %eax
	negl	%eax
	lock xaddb	%al, (%rdi)
	ret
	.cfi_endproc
.LFE130:
	.size	SCOREP_Atomic_FetchSub_int8, .-SCOREP_Atomic_FetchSub_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_int8
	.type	SCOREP_Atomic_FetchAnd_int8, @function
SCOREP_Atomic_FetchAnd_int8:
.LFB131:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L323:
	movl	%eax, %ecx
	movl	%eax, %edx
	andl	%esi, %ecx
	lock cmpxchgb	%cl, (%rdi)
	jne	.L323
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE131:
	.size	SCOREP_Atomic_FetchAnd_int8, .-SCOREP_Atomic_FetchAnd_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_int8
	.type	SCOREP_Atomic_FetchXor_int8, @function
SCOREP_Atomic_FetchXor_int8:
.LFB132:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L327:
	movl	%eax, %ecx
	movl	%eax, %edx
	xorl	%esi, %ecx
	lock cmpxchgb	%cl, (%rdi)
	jne	.L327
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE132:
	.size	SCOREP_Atomic_FetchXor_int8, .-SCOREP_Atomic_FetchXor_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_int8
	.type	SCOREP_Atomic_FetchOr_int8, @function
SCOREP_Atomic_FetchOr_int8:
.LFB133:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L331:
	movl	%eax, %ecx
	movl	%eax, %edx
	orl	%esi, %ecx
	lock cmpxchgb	%cl, (%rdi)
	jne	.L331
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE133:
	.size	SCOREP_Atomic_FetchOr_int8, .-SCOREP_Atomic_FetchOr_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_int8
	.type	SCOREP_Atomic_FetchNand_int8, @function
SCOREP_Atomic_FetchNand_int8:
.LFB134:
	.cfi_startproc
	movzbl	(%rdi), %eax
	movzbl	%sil, %esi
.L335:
	movl	%eax, %edx
	movl	%eax, %ecx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgb	%dl, (%rdi)
	jne	.L335
	movl	%ecx, %eax
	ret
	.cfi_endproc
.LFE134:
	.size	SCOREP_Atomic_FetchNand_int8, .-SCOREP_Atomic_FetchNand_int8
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_int16
	.type	SCOREP_Atomic_LoadN_int16, @function
SCOREP_Atomic_LoadN_int16:
.LFB135:
	.cfi_startproc
	movzwl	(%rdi), %eax
	ret
	.cfi_endproc
.LFE135:
	.size	SCOREP_Atomic_LoadN_int16, .-SCOREP_Atomic_LoadN_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_int16
	.type	SCOREP_Atomic_StoreN_int16, @function
SCOREP_Atomic_StoreN_int16:
.LFB136:
	.cfi_startproc
	movw	%si, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE136:
	.size	SCOREP_Atomic_StoreN_int16, .-SCOREP_Atomic_StoreN_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_int16
	.type	SCOREP_Atomic_ExchangeN_int16, @function
SCOREP_Atomic_ExchangeN_int16:
.LFB137:
	.cfi_startproc
	movzwl	%si, %eax
	xchgw	(%rdi), %ax
	ret
	.cfi_endproc
.LFE137:
	.size	SCOREP_Atomic_ExchangeN_int16, .-SCOREP_Atomic_ExchangeN_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_int16
	.type	SCOREP_Atomic_CompareExchangeN_int16, @function
SCOREP_Atomic_CompareExchangeN_int16:
.LFB138:
	.cfi_startproc
	movzwl	(%rsi), %eax
	movzwl	%dx, %edx
	lock cmpxchgw	%dx, (%rdi)
	movl	%eax, %edx
	sete	%al
	je	.L341
	movw	%dx, (%rsi)
.L341:
	rep ret
	.cfi_endproc
.LFE138:
	.size	SCOREP_Atomic_CompareExchangeN_int16, .-SCOREP_Atomic_CompareExchangeN_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_int16
	.type	SCOREP_Atomic_AddFetch_int16, @function
SCOREP_Atomic_AddFetch_int16:
.LFB139:
	.cfi_startproc
	movzwl	%si, %esi
	movl	%esi, %eax
	lock xaddw	%ax, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE139:
	.size	SCOREP_Atomic_AddFetch_int16, .-SCOREP_Atomic_AddFetch_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_int16
	.type	SCOREP_Atomic_SubFetch_int16, @function
SCOREP_Atomic_SubFetch_int16:
.LFB140:
	.cfi_startproc
	negl	%esi
	movl	%esi, %eax
	lock xaddw	%ax, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE140:
	.size	SCOREP_Atomic_SubFetch_int16, .-SCOREP_Atomic_SubFetch_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_int16
	.type	SCOREP_Atomic_AndFetch_int16, @function
SCOREP_Atomic_AndFetch_int16:
.LFB141:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L346:
	movl	%eax, %edx
	andl	%esi, %edx
	lock cmpxchgw	%dx, (%rdi)
	jne	.L346
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE141:
	.size	SCOREP_Atomic_AndFetch_int16, .-SCOREP_Atomic_AndFetch_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_int16
	.type	SCOREP_Atomic_XorFetch_int16, @function
SCOREP_Atomic_XorFetch_int16:
.LFB142:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L350:
	movl	%eax, %edx
	xorl	%esi, %edx
	lock cmpxchgw	%dx, (%rdi)
	jne	.L350
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE142:
	.size	SCOREP_Atomic_XorFetch_int16, .-SCOREP_Atomic_XorFetch_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_int16
	.type	SCOREP_Atomic_OrFetch_int16, @function
SCOREP_Atomic_OrFetch_int16:
.LFB143:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L354:
	movl	%eax, %edx
	orl	%esi, %edx
	lock cmpxchgw	%dx, (%rdi)
	jne	.L354
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE143:
	.size	SCOREP_Atomic_OrFetch_int16, .-SCOREP_Atomic_OrFetch_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_int16
	.type	SCOREP_Atomic_NandFetch_int16, @function
SCOREP_Atomic_NandFetch_int16:
.LFB144:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L358:
	movl	%eax, %edx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgw	%dx, (%rdi)
	jne	.L358
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE144:
	.size	SCOREP_Atomic_NandFetch_int16, .-SCOREP_Atomic_NandFetch_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_int16
	.type	SCOREP_Atomic_FetchAdd_int16, @function
SCOREP_Atomic_FetchAdd_int16:
.LFB145:
	.cfi_startproc
	movzwl	%si, %eax
	lock xaddw	%ax, (%rdi)
	ret
	.cfi_endproc
.LFE145:
	.size	SCOREP_Atomic_FetchAdd_int16, .-SCOREP_Atomic_FetchAdd_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_int16
	.type	SCOREP_Atomic_FetchSub_int16, @function
SCOREP_Atomic_FetchSub_int16:
.LFB146:
	.cfi_startproc
	movl	%esi, %eax
	negl	%eax
	lock xaddw	%ax, (%rdi)
	ret
	.cfi_endproc
.LFE146:
	.size	SCOREP_Atomic_FetchSub_int16, .-SCOREP_Atomic_FetchSub_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_int16
	.type	SCOREP_Atomic_FetchAnd_int16, @function
SCOREP_Atomic_FetchAnd_int16:
.LFB147:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L364:
	movl	%eax, %ecx
	movl	%eax, %edx
	andl	%esi, %ecx
	lock cmpxchgw	%cx, (%rdi)
	jne	.L364
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE147:
	.size	SCOREP_Atomic_FetchAnd_int16, .-SCOREP_Atomic_FetchAnd_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_int16
	.type	SCOREP_Atomic_FetchXor_int16, @function
SCOREP_Atomic_FetchXor_int16:
.LFB148:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L368:
	movl	%eax, %ecx
	movl	%eax, %edx
	xorl	%esi, %ecx
	lock cmpxchgw	%cx, (%rdi)
	jne	.L368
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE148:
	.size	SCOREP_Atomic_FetchXor_int16, .-SCOREP_Atomic_FetchXor_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_int16
	.type	SCOREP_Atomic_FetchOr_int16, @function
SCOREP_Atomic_FetchOr_int16:
.LFB149:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L372:
	movl	%eax, %ecx
	movl	%eax, %edx
	orl	%esi, %ecx
	lock cmpxchgw	%cx, (%rdi)
	jne	.L372
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE149:
	.size	SCOREP_Atomic_FetchOr_int16, .-SCOREP_Atomic_FetchOr_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_int16
	.type	SCOREP_Atomic_FetchNand_int16, @function
SCOREP_Atomic_FetchNand_int16:
.LFB150:
	.cfi_startproc
	movzwl	(%rdi), %eax
	movzwl	%si, %esi
.L376:
	movl	%eax, %edx
	movl	%eax, %ecx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgw	%dx, (%rdi)
	jne	.L376
	movl	%ecx, %eax
	ret
	.cfi_endproc
.LFE150:
	.size	SCOREP_Atomic_FetchNand_int16, .-SCOREP_Atomic_FetchNand_int16
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_int32
	.type	SCOREP_Atomic_LoadN_int32, @function
SCOREP_Atomic_LoadN_int32:
.LFB151:
	.cfi_startproc
	movl	(%rdi), %eax
	ret
	.cfi_endproc
.LFE151:
	.size	SCOREP_Atomic_LoadN_int32, .-SCOREP_Atomic_LoadN_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_int32
	.type	SCOREP_Atomic_StoreN_int32, @function
SCOREP_Atomic_StoreN_int32:
.LFB152:
	.cfi_startproc
	movl	%esi, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE152:
	.size	SCOREP_Atomic_StoreN_int32, .-SCOREP_Atomic_StoreN_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_int32
	.type	SCOREP_Atomic_ExchangeN_int32, @function
SCOREP_Atomic_ExchangeN_int32:
.LFB153:
	.cfi_startproc
	movl	%esi, %eax
	xchgl	(%rdi), %eax
	ret
	.cfi_endproc
.LFE153:
	.size	SCOREP_Atomic_ExchangeN_int32, .-SCOREP_Atomic_ExchangeN_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_int32
	.type	SCOREP_Atomic_CompareExchangeN_int32, @function
SCOREP_Atomic_CompareExchangeN_int32:
.LFB154:
	.cfi_startproc
	movl	(%rsi), %eax
	lock cmpxchgl	%edx, (%rdi)
	movl	%eax, %edx
	sete	%al
	je	.L382
	movl	%edx, (%rsi)
.L382:
	rep ret
	.cfi_endproc
.LFE154:
	.size	SCOREP_Atomic_CompareExchangeN_int32, .-SCOREP_Atomic_CompareExchangeN_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_int32
	.type	SCOREP_Atomic_AddFetch_int32, @function
SCOREP_Atomic_AddFetch_int32:
.LFB155:
	.cfi_startproc
	movl	%esi, %eax
	lock xaddl	%eax, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE155:
	.size	SCOREP_Atomic_AddFetch_int32, .-SCOREP_Atomic_AddFetch_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_int32
	.type	SCOREP_Atomic_SubFetch_int32, @function
SCOREP_Atomic_SubFetch_int32:
.LFB156:
	.cfi_startproc
	negl	%esi
	movl	%esi, %eax
	lock xaddl	%eax, (%rdi)
	addl	%esi, %eax
	ret
	.cfi_endproc
.LFE156:
	.size	SCOREP_Atomic_SubFetch_int32, .-SCOREP_Atomic_SubFetch_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_int32
	.type	SCOREP_Atomic_AndFetch_int32, @function
SCOREP_Atomic_AndFetch_int32:
.LFB157:
	.cfi_startproc
	movl	(%rdi), %eax
.L387:
	movl	%eax, %edx
	andl	%esi, %edx
	lock cmpxchgl	%edx, (%rdi)
	jne	.L387
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE157:
	.size	SCOREP_Atomic_AndFetch_int32, .-SCOREP_Atomic_AndFetch_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_int32
	.type	SCOREP_Atomic_XorFetch_int32, @function
SCOREP_Atomic_XorFetch_int32:
.LFB158:
	.cfi_startproc
	movl	(%rdi), %eax
.L391:
	movl	%eax, %edx
	xorl	%esi, %edx
	lock cmpxchgl	%edx, (%rdi)
	jne	.L391
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE158:
	.size	SCOREP_Atomic_XorFetch_int32, .-SCOREP_Atomic_XorFetch_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_int32
	.type	SCOREP_Atomic_OrFetch_int32, @function
SCOREP_Atomic_OrFetch_int32:
.LFB159:
	.cfi_startproc
	movl	(%rdi), %eax
.L395:
	movl	%eax, %edx
	orl	%esi, %edx
	lock cmpxchgl	%edx, (%rdi)
	jne	.L395
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE159:
	.size	SCOREP_Atomic_OrFetch_int32, .-SCOREP_Atomic_OrFetch_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_int32
	.type	SCOREP_Atomic_NandFetch_int32, @function
SCOREP_Atomic_NandFetch_int32:
.LFB160:
	.cfi_startproc
	movl	(%rdi), %eax
.L399:
	movl	%eax, %edx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgl	%edx, (%rdi)
	jne	.L399
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE160:
	.size	SCOREP_Atomic_NandFetch_int32, .-SCOREP_Atomic_NandFetch_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_int32
	.type	SCOREP_Atomic_FetchAdd_int32, @function
SCOREP_Atomic_FetchAdd_int32:
.LFB161:
	.cfi_startproc
	movl	%esi, %eax
	lock xaddl	%eax, (%rdi)
	ret
	.cfi_endproc
.LFE161:
	.size	SCOREP_Atomic_FetchAdd_int32, .-SCOREP_Atomic_FetchAdd_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_int32
	.type	SCOREP_Atomic_FetchSub_int32, @function
SCOREP_Atomic_FetchSub_int32:
.LFB162:
	.cfi_startproc
	movl	%esi, %eax
	negl	%eax
	lock xaddl	%eax, (%rdi)
	ret
	.cfi_endproc
.LFE162:
	.size	SCOREP_Atomic_FetchSub_int32, .-SCOREP_Atomic_FetchSub_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_int32
	.type	SCOREP_Atomic_FetchAnd_int32, @function
SCOREP_Atomic_FetchAnd_int32:
.LFB163:
	.cfi_startproc
	movl	(%rdi), %eax
.L405:
	movl	%eax, %ecx
	movl	%eax, %edx
	andl	%esi, %ecx
	lock cmpxchgl	%ecx, (%rdi)
	jne	.L405
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE163:
	.size	SCOREP_Atomic_FetchAnd_int32, .-SCOREP_Atomic_FetchAnd_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_int32
	.type	SCOREP_Atomic_FetchXor_int32, @function
SCOREP_Atomic_FetchXor_int32:
.LFB164:
	.cfi_startproc
	movl	(%rdi), %eax
.L409:
	movl	%eax, %ecx
	movl	%eax, %edx
	xorl	%esi, %ecx
	lock cmpxchgl	%ecx, (%rdi)
	jne	.L409
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE164:
	.size	SCOREP_Atomic_FetchXor_int32, .-SCOREP_Atomic_FetchXor_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_int32
	.type	SCOREP_Atomic_FetchOr_int32, @function
SCOREP_Atomic_FetchOr_int32:
.LFB165:
	.cfi_startproc
	movl	(%rdi), %eax
.L413:
	movl	%eax, %ecx
	movl	%eax, %edx
	orl	%esi, %ecx
	lock cmpxchgl	%ecx, (%rdi)
	jne	.L413
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE165:
	.size	SCOREP_Atomic_FetchOr_int32, .-SCOREP_Atomic_FetchOr_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_int32
	.type	SCOREP_Atomic_FetchNand_int32, @function
SCOREP_Atomic_FetchNand_int32:
.LFB166:
	.cfi_startproc
	movl	(%rdi), %eax
.L417:
	movl	%eax, %edx
	movl	%eax, %ecx
	andl	%esi, %edx
	notl	%edx
	lock cmpxchgl	%edx, (%rdi)
	jne	.L417
	movl	%ecx, %eax
	ret
	.cfi_endproc
.LFE166:
	.size	SCOREP_Atomic_FetchNand_int32, .-SCOREP_Atomic_FetchNand_int32
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_int64
	.type	SCOREP_Atomic_LoadN_int64, @function
SCOREP_Atomic_LoadN_int64:
.LFB167:
	.cfi_startproc
	movq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE167:
	.size	SCOREP_Atomic_LoadN_int64, .-SCOREP_Atomic_LoadN_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_int64
	.type	SCOREP_Atomic_StoreN_int64, @function
SCOREP_Atomic_StoreN_int64:
.LFB168:
	.cfi_startproc
	movq	%rsi, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE168:
	.size	SCOREP_Atomic_StoreN_int64, .-SCOREP_Atomic_StoreN_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_int64
	.type	SCOREP_Atomic_ExchangeN_int64, @function
SCOREP_Atomic_ExchangeN_int64:
.LFB169:
	.cfi_startproc
	movq	%rsi, %rax
	xchgq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE169:
	.size	SCOREP_Atomic_ExchangeN_int64, .-SCOREP_Atomic_ExchangeN_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_int64
	.type	SCOREP_Atomic_CompareExchangeN_int64, @function
SCOREP_Atomic_CompareExchangeN_int64:
.LFB170:
	.cfi_startproc
	movq	(%rsi), %rax
	lock cmpxchgq	%rdx, (%rdi)
	movq	%rax, %rdx
	sete	%al
	je	.L423
	movq	%rdx, (%rsi)
.L423:
	rep ret
	.cfi_endproc
.LFE170:
	.size	SCOREP_Atomic_CompareExchangeN_int64, .-SCOREP_Atomic_CompareExchangeN_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_int64
	.type	SCOREP_Atomic_AddFetch_int64, @function
SCOREP_Atomic_AddFetch_int64:
.LFB171:
	.cfi_startproc
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	addq	%rsi, %rax
	ret
	.cfi_endproc
.LFE171:
	.size	SCOREP_Atomic_AddFetch_int64, .-SCOREP_Atomic_AddFetch_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_int64
	.type	SCOREP_Atomic_SubFetch_int64, @function
SCOREP_Atomic_SubFetch_int64:
.LFB172:
	.cfi_startproc
	negq	%rsi
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	addq	%rsi, %rax
	ret
	.cfi_endproc
.LFE172:
	.size	SCOREP_Atomic_SubFetch_int64, .-SCOREP_Atomic_SubFetch_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_int64
	.type	SCOREP_Atomic_AndFetch_int64, @function
SCOREP_Atomic_AndFetch_int64:
.LFB173:
	.cfi_startproc
	movq	(%rdi), %rax
.L428:
	movq	%rax, %rdx
	andq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L428
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE173:
	.size	SCOREP_Atomic_AndFetch_int64, .-SCOREP_Atomic_AndFetch_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_int64
	.type	SCOREP_Atomic_XorFetch_int64, @function
SCOREP_Atomic_XorFetch_int64:
.LFB174:
	.cfi_startproc
	movq	(%rdi), %rax
.L432:
	movq	%rax, %rdx
	xorq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L432
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE174:
	.size	SCOREP_Atomic_XorFetch_int64, .-SCOREP_Atomic_XorFetch_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_int64
	.type	SCOREP_Atomic_OrFetch_int64, @function
SCOREP_Atomic_OrFetch_int64:
.LFB175:
	.cfi_startproc
	movq	(%rdi), %rax
.L436:
	movq	%rax, %rdx
	orq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L436
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE175:
	.size	SCOREP_Atomic_OrFetch_int64, .-SCOREP_Atomic_OrFetch_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_int64
	.type	SCOREP_Atomic_NandFetch_int64, @function
SCOREP_Atomic_NandFetch_int64:
.LFB176:
	.cfi_startproc
	movq	(%rdi), %rax
.L440:
	movq	%rax, %rdx
	andq	%rsi, %rdx
	notq	%rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L440
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE176:
	.size	SCOREP_Atomic_NandFetch_int64, .-SCOREP_Atomic_NandFetch_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_int64
	.type	SCOREP_Atomic_FetchAdd_int64, @function
SCOREP_Atomic_FetchAdd_int64:
.LFB177:
	.cfi_startproc
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE177:
	.size	SCOREP_Atomic_FetchAdd_int64, .-SCOREP_Atomic_FetchAdd_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_int64
	.type	SCOREP_Atomic_FetchSub_int64, @function
SCOREP_Atomic_FetchSub_int64:
.LFB178:
	.cfi_startproc
	movq	%rsi, %rax
	negq	%rax
	lock xaddq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE178:
	.size	SCOREP_Atomic_FetchSub_int64, .-SCOREP_Atomic_FetchSub_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_int64
	.type	SCOREP_Atomic_FetchAnd_int64, @function
SCOREP_Atomic_FetchAnd_int64:
.LFB179:
	.cfi_startproc
	movq	(%rdi), %rax
.L446:
	movq	%rax, %rcx
	movq	%rax, %rdx
	andq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L446
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE179:
	.size	SCOREP_Atomic_FetchAnd_int64, .-SCOREP_Atomic_FetchAnd_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_int64
	.type	SCOREP_Atomic_FetchXor_int64, @function
SCOREP_Atomic_FetchXor_int64:
.LFB180:
	.cfi_startproc
	movq	(%rdi), %rax
.L450:
	movq	%rax, %rcx
	movq	%rax, %rdx
	xorq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L450
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE180:
	.size	SCOREP_Atomic_FetchXor_int64, .-SCOREP_Atomic_FetchXor_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_int64
	.type	SCOREP_Atomic_FetchOr_int64, @function
SCOREP_Atomic_FetchOr_int64:
.LFB181:
	.cfi_startproc
	movq	(%rdi), %rax
.L454:
	movq	%rax, %rcx
	movq	%rax, %rdx
	orq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L454
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE181:
	.size	SCOREP_Atomic_FetchOr_int64, .-SCOREP_Atomic_FetchOr_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_int64
	.type	SCOREP_Atomic_FetchNand_int64, @function
SCOREP_Atomic_FetchNand_int64:
.LFB182:
	.cfi_startproc
	movq	(%rdi), %rax
.L458:
	movq	%rax, %rdx
	movq	%rax, %rcx
	andq	%rsi, %rdx
	notq	%rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L458
	movq	%rcx, %rax
	ret
	.cfi_endproc
.LFE182:
	.size	SCOREP_Atomic_FetchNand_int64, .-SCOREP_Atomic_FetchNand_int64
	.p2align 4,,15
	.globl	SCOREP_Atomic_LoadN_intptr
	.type	SCOREP_Atomic_LoadN_intptr, @function
SCOREP_Atomic_LoadN_intptr:
.LFB232:
	.cfi_startproc
	movq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE232:
	.size	SCOREP_Atomic_LoadN_intptr, .-SCOREP_Atomic_LoadN_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_StoreN_intptr
	.type	SCOREP_Atomic_StoreN_intptr, @function
SCOREP_Atomic_StoreN_intptr:
.LFB234:
	.cfi_startproc
	movq	%rsi, (%rdi)
	mfence
	ret
	.cfi_endproc
.LFE234:
	.size	SCOREP_Atomic_StoreN_intptr, .-SCOREP_Atomic_StoreN_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_ExchangeN_intptr
	.type	SCOREP_Atomic_ExchangeN_intptr, @function
SCOREP_Atomic_ExchangeN_intptr:
.LFB236:
	.cfi_startproc
	movq	%rsi, %rax
	xchgq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE236:
	.size	SCOREP_Atomic_ExchangeN_intptr, .-SCOREP_Atomic_ExchangeN_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_CompareExchangeN_intptr
	.type	SCOREP_Atomic_CompareExchangeN_intptr, @function
SCOREP_Atomic_CompareExchangeN_intptr:
.LFB262:
	.cfi_startproc
	movq	(%rsi), %rax
	lock cmpxchgq	%rdx, (%rdi)
	movq	%rax, %rdx
	sete	%al
	je	.L464
	movq	%rdx, (%rsi)
.L464:
	rep ret
	.cfi_endproc
.LFE262:
	.size	SCOREP_Atomic_CompareExchangeN_intptr, .-SCOREP_Atomic_CompareExchangeN_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_AddFetch_intptr
	.type	SCOREP_Atomic_AddFetch_intptr, @function
SCOREP_Atomic_AddFetch_intptr:
.LFB238:
	.cfi_startproc
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	addq	%rsi, %rax
	ret
	.cfi_endproc
.LFE238:
	.size	SCOREP_Atomic_AddFetch_intptr, .-SCOREP_Atomic_AddFetch_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_SubFetch_intptr
	.type	SCOREP_Atomic_SubFetch_intptr, @function
SCOREP_Atomic_SubFetch_intptr:
.LFB240:
	.cfi_startproc
	negq	%rsi
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	addq	%rsi, %rax
	ret
	.cfi_endproc
.LFE240:
	.size	SCOREP_Atomic_SubFetch_intptr, .-SCOREP_Atomic_SubFetch_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_AndFetch_intptr
	.type	SCOREP_Atomic_AndFetch_intptr, @function
SCOREP_Atomic_AndFetch_intptr:
.LFB242:
	.cfi_startproc
	movq	(%rdi), %rax
.L469:
	movq	%rax, %rdx
	andq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L469
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE242:
	.size	SCOREP_Atomic_AndFetch_intptr, .-SCOREP_Atomic_AndFetch_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_XorFetch_intptr
	.type	SCOREP_Atomic_XorFetch_intptr, @function
SCOREP_Atomic_XorFetch_intptr:
.LFB244:
	.cfi_startproc
	movq	(%rdi), %rax
.L473:
	movq	%rax, %rdx
	xorq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L473
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE244:
	.size	SCOREP_Atomic_XorFetch_intptr, .-SCOREP_Atomic_XorFetch_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_OrFetch_intptr
	.type	SCOREP_Atomic_OrFetch_intptr, @function
SCOREP_Atomic_OrFetch_intptr:
.LFB246:
	.cfi_startproc
	movq	(%rdi), %rax
.L477:
	movq	%rax, %rdx
	orq	%rsi, %rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L477
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE246:
	.size	SCOREP_Atomic_OrFetch_intptr, .-SCOREP_Atomic_OrFetch_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_NandFetch_intptr
	.type	SCOREP_Atomic_NandFetch_intptr, @function
SCOREP_Atomic_NandFetch_intptr:
.LFB248:
	.cfi_startproc
	movq	(%rdi), %rax
.L481:
	movq	%rax, %rdx
	andq	%rsi, %rdx
	notq	%rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L481
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE248:
	.size	SCOREP_Atomic_NandFetch_intptr, .-SCOREP_Atomic_NandFetch_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAdd_intptr
	.type	SCOREP_Atomic_FetchAdd_intptr, @function
SCOREP_Atomic_FetchAdd_intptr:
.LFB250:
	.cfi_startproc
	movq	%rsi, %rax
	lock xaddq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE250:
	.size	SCOREP_Atomic_FetchAdd_intptr, .-SCOREP_Atomic_FetchAdd_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchSub_intptr
	.type	SCOREP_Atomic_FetchSub_intptr, @function
SCOREP_Atomic_FetchSub_intptr:
.LFB252:
	.cfi_startproc
	movq	%rsi, %rax
	negq	%rax
	lock xaddq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE252:
	.size	SCOREP_Atomic_FetchSub_intptr, .-SCOREP_Atomic_FetchSub_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchAnd_intptr
	.type	SCOREP_Atomic_FetchAnd_intptr, @function
SCOREP_Atomic_FetchAnd_intptr:
.LFB254:
	.cfi_startproc
	movq	(%rdi), %rax
.L487:
	movq	%rax, %rcx
	movq	%rax, %rdx
	andq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L487
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE254:
	.size	SCOREP_Atomic_FetchAnd_intptr, .-SCOREP_Atomic_FetchAnd_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchXor_intptr
	.type	SCOREP_Atomic_FetchXor_intptr, @function
SCOREP_Atomic_FetchXor_intptr:
.LFB256:
	.cfi_startproc
	movq	(%rdi), %rax
.L491:
	movq	%rax, %rcx
	movq	%rax, %rdx
	xorq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L491
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE256:
	.size	SCOREP_Atomic_FetchXor_intptr, .-SCOREP_Atomic_FetchXor_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchOr_intptr
	.type	SCOREP_Atomic_FetchOr_intptr, @function
SCOREP_Atomic_FetchOr_intptr:
.LFB258:
	.cfi_startproc
	movq	(%rdi), %rax
.L495:
	movq	%rax, %rcx
	movq	%rax, %rdx
	orq	%rsi, %rcx
	lock cmpxchgq	%rcx, (%rdi)
	jne	.L495
	movq	%rdx, %rax
	ret
	.cfi_endproc
.LFE258:
	.size	SCOREP_Atomic_FetchOr_intptr, .-SCOREP_Atomic_FetchOr_intptr
	.p2align 4,,15
	.globl	SCOREP_Atomic_FetchNand_intptr
	.type	SCOREP_Atomic_FetchNand_intptr, @function
SCOREP_Atomic_FetchNand_intptr:
.LFB260:
	.cfi_startproc
	movq	(%rdi), %rax
.L499:
	movq	%rax, %rdx
	movq	%rax, %rcx
	andq	%rsi, %rdx
	notq	%rdx
	lock cmpxchgq	%rdx, (%rdi)
	jne	.L499
	movq	%rcx, %rax
	ret
	.cfi_endproc
.LFE260:
	.size	SCOREP_Atomic_FetchNand_intptr, .-SCOREP_Atomic_FetchNand_intptr
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
