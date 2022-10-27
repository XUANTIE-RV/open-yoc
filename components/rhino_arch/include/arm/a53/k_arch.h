/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef K_ARCH_H
#define K_ARCH_H

#ifndef __ASSEMBLY__
typedef struct {
    long Q30;
    long Q31;
    long Q28;
    long Q29;
    long Q26;
    long Q27;
    long Q24;
    long Q25;
    long Q22;
    long Q23;
    long Q20;
    long Q21;
    long Q18;
    long Q19;
    long Q16;
    long Q17;
    long Q14;
    long Q15;
    long Q12;
    long Q13;
    long Q10;
    long Q11;
    long Q8;
    long Q9;
    long Q6;
    long Q7;
    long Q4;
    long Q5;
    long Q2;
    long Q3;
    long Q0;
    long Q1;
    long ELR;
    long SPSR;

    long X30;
    long XZR;
    long X28;
    long X29;
    long X26;
    long X27;
    long X24;
    long X25;
    long X22;
    long X23;
    long X20;
    long X21;
    long X18;
    long X19;
    long X16;
    long X17;
    long X14;
    long X15;
    long X12;
    long X13;
    long X10;
    long X11;
    long X8;
    long X9;
    long X6;
    long X7;
    long X4;
    long X5;
    long X2;
    long X3;
    long X0;
    long X1;
} context_t;

#endif

#endif /*  K_ARCH_H */

