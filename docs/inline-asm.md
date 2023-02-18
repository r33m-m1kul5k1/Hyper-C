# Inline assembly in C

The assembly flavor is AT&T

```C
int no = 100, val ;
    asm ("movl %1, %%ebx;"
         "movl %%ebx, %0;"
         : "=r" ( val )        /* output (write only) */
         : "r" ( no )         /* input */
         : "%ebx"         /* clobbered register */
     );
```

The register have two % to distinguish from a input / output operand.

## Common Constraints

```text
+---+--------------------+
| r |    Register(s)     |
+---+--------------------+
| a |   %eax, %ax, %al   |
| b |   %ebx, %bx, %bl   |
| c |   %ecx, %cx, %cl   |
| d |   %edx, %dx, %dl   |
| S |   %esi, %si        |
| D |   %edi, %di        |
+---+--------------------+
```

## Clobbered Registers

This tells gcc that the inline assembly uses this register and it should not be used as an IO operand.

## Symbolic Names

```C
uint32_t Mask = 1234;
uint32_t Index;

  asm ("bsfl %[aMask], %[aIndex]"
     : [aIndex] "=r" (Index)
     : [aMask] "r" (Mask)
     : "cc");
```

## Memory

The passed memory IO operand must be the value of that variable not it's address.

```C
int a = 0;
int *ptr = &a;
asm ("mov %1, %0": "=m" (*ptr) : "ir" (1))
```

## Optimization

Changing memory sometimes can hurt performance so if we specify or improve them.

```C
uint32_t c = 1;
uint32_t d;
uint32_t *c_ptr = &c;

asm ("mov %1, %0"
   : "=rm" (d)
   : "rm" (*c_ptr));
```

The `=rm` and `rm` constraint says that `d` and `c_ptr` can be a register or in memory.
Since the complier may already have the value of `c` inside a register it may just use this register.

## Resources

* [Constrains](https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html#ss6.1)
* [Tutorial](https://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C)
* [Official GNU Docs](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html)
