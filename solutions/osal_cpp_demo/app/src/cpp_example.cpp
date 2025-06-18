 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <iostream>

using namespace std;

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

    int i = 5;
    int j = 7;
    int x = std::max(i, j);

    cout << "max lenth:" << x << endl;
    return 0;
}

void example_main(void)
{
    printf("CPP DEMO Start!!!\r\n");
    cpp_main();
    printf("CPP DEMO End!!!\r\n");
}
