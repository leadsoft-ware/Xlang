# Basic Command

You can read example program yourself,there's nothing I will say.<br>
**It is too easy.**

# Call And Return

call statement marco no longer included in test.xasm.Users must do it by themselves.

```go
    // call stmt
    push(reg16,8);
    push(reg17,8);
    mov(reg0,reg18);
    add(reg0,448); // add offset, 22*16 + arguments_count * 3 * 16
    push(reg0,8);
    // arguments process
    push(114514,8);
    push(233333,8);

    add(reg16,reg17);
    sub(reg16,16); // arguments total size: 8*2 (114514,233333)
    mov(reg17,0);
    add(reg17,16); // let arguments can be pop, 16 means total size same as line 12
    jmp_nofx(main); // put your function name here.
```

return statement is a marco<br>
just type`ret([value,register or address]);`