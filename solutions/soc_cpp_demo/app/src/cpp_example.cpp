/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

/* FIXME: <iostream> cost a lot of codesize if included. */
#if CONFIG_CPU_E906 || CONFIG_CPU_E906F || CONFIG_CPU_E906FD || CONFIG_CPU_E906P || CONFIG_CPU_E906FP || CONFIG_CPU_E906FDP \
    || CONFIG_CPU_E907 || CONFIG_CPU_E907F || CONFIG_CPU_E907FD || CONFIG_CPU_E907P || CONFIG_CPU_E907FP || CONFIG_CPU_E907FDP \
    || CONFIG_CPU_E902 || CONFIG_CPU_E902M || CONFIG_CPU_E902T || CONFIG_CPU_E902MT
/* resource of smartl platform is limited */
#define CODESIZE_SMALL
#endif

#include <stdio.h>
#ifndef CODESIZE_SMALL
#include <iostream>

using namespace std;
#endif

extern "C" void example_main(void);

class Box
{
public:
    double length;
    double breadth;
    double height;
    Box()
    {
        printf("Object is being created\r\n");
    }

    ~Box()
    {
        printf("Object is being deleted\r\n");
    }
};

static Box g_box;
static int cpp_main(void)
{
    Box box1;
    Box box2;
    Box box3;
    Box box4;
    Box box5;
    Box box6;

    double volume = 0.0;
    // g_box
    g_box.height = 4.0;
    g_box.length = 5.0;
    g_box.breadth = 6.0;

    // box1
    box1.height = 5.0;
    box1.length = 6.0;
    box1.breadth = 7.0;

    // box2
    box2.height = 10.0;
    box2.length = 12.0;
    box2.breadth = 13.0;

    // g_box
    volume = g_box.height * g_box.length * g_box.breadth;
    printf("g_box volume: %f\r\n", volume);

    // box1
    volume = box1.height * box1.length * box1.breadth;
    printf("box1 volume: %f\r\n", volume);

    // box2
    volume = box2.height * box2.length * box2.breadth;
    printf("box2 volume: %f\r\n", volume);

#ifndef CODESIZE_SMALL
    int i = 5;
    int j = 7;
    int x = std::max(i, j);

    cout << "max lenth:" << x << endl;
#endif
    return 0;
}

void example_main(void)
{
    printf("CPP DEMO Start!!!\r\n");
    cpp_main();
    printf("CPP DEMO End!!!\r\n");
}
