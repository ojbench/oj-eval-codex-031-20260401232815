# Problem 031 - python列表

**ACMOJ Problem ID**: 1427

## Table of Contents

- [Problem 031 - python列表](#problem-031-python列表)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
    - [Background](#background)
  - [Assignment Description](#assignment-description)
    - [Grade Composition](#grade-composition)
  - [Assignment Requirements](#assignment-requirements)
    - [Input Format](#input-format)
    - [Output Format](#output-format)
    - [Samples](#samples)
    - [Data Constraints](#data-constraints)
  - [Per-Testcase Resource Limits](#per-testcase-resource-limits)
  - [Test Data](#test-data)
  - [Submission Requirements](#submission-requirements)
    - [Submission Format](#submission-format)
    - [Submission Guidelines](#submission-guidelines)
    - [Evaluation Notes](#evaluation-notes)
    - [Academic Integrity](#academic-integrity)

## Introduction

### Background

Implement Python-like list in C++: supports append, insert, delete, slice, print.

## Assignment Description

### Grade Composition

| Grading Component | Percentage |
| :--: | :--: |
| Pass **1427. python列表** | 80% |
| Code Review | 20% |

Here are several points that need clarification:

- In the Code Review, we will **strictly examine your code style and repository organization structure, etc.**

- This assignment provides some sample data for testing, stored in the `/workspace/data/031/data_test/` directory. Note that these are not the test cases on the Online Judge. Passing all local test cases does not guarantee that you will pass the OJ tests.

- Besides the provided sample data, we also encourage you to design your own test data based on your program logic to assist debugging.

## Assignment Requirements

### Problem Description

本题中你可以自由地调用库。

本题中，你提交的代码将被作为名为"src.hpp"的头文件而被提交，评测主程序将会include该头文件并检验该头文件的正确性。


python解释器一直为大家所津津乐道，但stella发现里面却没有列表的实现要求。stella觉得没有列表的python怎么能用呢？于是趁这次机会ta决定让大家用c++实现一个python列表。

目的：阅读测试样例，完成一个极简的 python 中的列表，int 版本，类名为pylist。

这个列表类似一个vector，区别在于: 

1. 列表可以互相嵌套，嵌套深度无限制，如`[0, 1, [2, []]]`。
2. 列表在赋值与拷贝构造的时候都是传引用。如，执行语句`ls2 = ls`后，`ls2`与`ls`实际指向了同一个列表，对两者中的任何一个的操作将同时反映在另一个列表当中。直到`ls = ls3`以后，`ls`与`ls3`指向了同一个列表，而`ls2`不变。

需要支持功能：

- 成员函数append：在列表末尾增加一个int或pylist，类似vector中的push_back。

- 成员函数pop：弹出列表末尾的int或pylist，并返回这个被弹出的值，类似的vector中的pop_back（带返回值）。

- 下标访问运算符[]：返回指定下标处的int或pylist，返回值也可当做左值被赋值为int或pylist。
  (以这种方法赋值时，int 可被赋值为 pylist，pylist 也可被赋值为 int ) 。

- 重载输出流运算符<<：打印该列表，格式见样例。注意，为了防止列表自引用产生的无限输出，你需要在发生第一次递归自引用的时候将列表的深层打印值替换为[...]。如:

  ```
  pylist ls;
  ls.append(1);
  ls.append(ls);
  cout << ls << endl; // ls == [1, [...]]
  cout << ls.pop() << endl; // ls == [1]
  ```
  
只要能AC，你可以使用任意方法完成此题。

- 细节说明：你的列表中的int会被pop出而返回或被[]访问，保证样例中对这样返回的int的使用方式不超过一下方式：用<<进行输出，或被用在二元运算符`+ - * / % & | ^ << >>`，一元运算符`+ -`，比较运算符 `< > <= >= == !=`中，或重新被append入一个pylist。但我们不会对其类型做判断。例如，我们不会用一个int*指针指向它，不会用decltype或typeid获取类型，也不会对其调用`*=2`等操作。

### 代码模板

```
#ifndef PYLIST_H
#define PYLIST_H

#include<iostream>

class pylist {

private:

public:

    pylist() {
        
    }
    
    void append(const pylist &x) {
        
    }

    pylist pop() {
        return pylist(); //meaningless return to prevent compile error.
    }

    pylist &operator[](size_t i) {
        return *this; //meaningless return to prevent compile error.
    }

    friend std::ostream &operator<<(std::ostream &os, const pylist &ls) {
        return os;
    }

};


#endif //PYLIST_H
```

### 样例测试程序

```cpp
#include "src.hpp"
#include<iostream>

using namespace std;

int main() {
    puts("First, have some simple operation just like std::vector.");
    pylist ls;
    ls.append(1);
    cout << ls[0] << endl; // 1
    ls.append(2);
    cout << ls[1] << endl; // 2
    ls.pop();
    ls.append(3);
    cout << ls[1] << endl; // 3 (Now, ls == [1, 3])
    puts("After warming up exercise, your assignment operator will be tested.");
    puts("Now we will test whether your assignment operator will pass by reference.");
    pylist ls2 = ls;
    ls.append(4);
    cout << ls2[2] << endl; // 4 (Now, ls == ls2 == [1, 3, 4]) (Because assignment operator pass by reference. We will not mention this afterward.)
    ls.pop();
    ls2.append(5);
    cout << ls[2] << endl; // 5 (Now, ls == ls2 == [1, 3, 5])
    puts("Now we will test whether your pylist enable nesting.");
    pylist ls3;
    ls3.append(6);
    ls[0] = ls3;
    cout << ls2[0][0] << endl;// 6 (Now, ls == ls2 == [[6], 3, 5])
    ls2.append(ls3);
    ls2[3].append(7);
    ls[1] = pylist();
    cout << ls[0][1] << endl; // 7 (Now, ls == ls2 == [[6, 7], [], 5, [6, 7]])
    ls2[2] = ls[1];
    ls2[1].append(8);
    cout << ls2[2][0] << endl; // 8 (Now, ls == ls2 == [[6, 7], [8], [8], [6, 7]], ls3 = [6, 7])
    puts("Robust test now.");
    ls[0] = pylist();
    ls2 = ls;
    ls3 = ls3; //(Now, ls == ls2 == [[], [8], [8], [6, 7]], ls3 = [6, 7])
    ls2 = ls3; //(Now, ls == [[], [8], [8], [6, 7]], ls2 == ls3 = [6, 7])
    ls3[0] = ls3; // (ls3 is being referenced by itself)
    ls2.append(3*3); //(Now, ls == [[], [8], [8], [[...], 7, 9]], ls2 == ls3 = [[...], 7, 9])
    cout << ls[3][0][0][0][0][0][0][0][0][0][0][0][0][2] << endl; // 9
    puts("Finally!");
    cout << 2 * ls[3][0][2] + (ls2[1] << 1 >> 1) - (1 ^ ls2[2] ^ 1) - (ls2[1] / 1) - (ls[1][0] % 8) + 1 << endl; // 10
    cout << ls << endl; // [[], [8], [8], [[...], 7, 9]]
    cout << ls.pop() << endl; // [[...], 7, 9]
    cout << 0 + ls[2].pop() << endl; // 8
    cout << ls << endl; // [[], [], []]
}
```

### Input Format

无

### Output Format

无

### Samples

#### Sample 1

**Output:**
```
First, have some simple operation just like std::vector.
1
2
3
After warming up exercise, your assignment operator will be tested.
Now we will test whether your assignment operator will pass by reference.
4
5
Now we will test whether your pylist enable nesting.
6
7
8
Robust test now.
9
Finally!
10
[[], [8], [8], [[...], 7, 9]]
[[...], 7, 9]
8
[[], [], []]
```

### Data Constraints

要求`append`，`pop`，下标访问运算符`[]`的时间复杂度均为O(1)。

测试点说明：

| 测试序号  | 范围说明 |
| ---- | ------------------------------------------------------------ |
| 1 2  | 以如同c++ vector的方式使用vector，需完成`append`，`pop`，下标访问运算符`[]`。其中测试点2为压力测试 |
| 3 4  | 在1 2基础上增加了对无省略号的列表输出、赋值或拷贝构造传引用的考察 |
| 5 6  | 在1 2基础上增加了对无省略号的列表输出、列表嵌套的考察        |
| 7    | 3 4 与 5 6 的综合                                            |
| 8    | 在7基础上增加了对输出省略号的考察                                   |
| 9 10 | 综合测试点                                                   |

## Per-Testcase Resource Limits

- **Time Limit (per test case)**: 2000 ms
- **Memory Limit (per test case)**: 512 MiB
- **Disk Usage**: No disk usage is permitted.

## Test Data

The test data for this problem is located at `/workspace/data/031/data_test/`.

## Submission Requirements

### Submission Format

Submit a single C++ source file to the Online Judge. The OJ provides its own `main.cpp` which `#include "src.hpp"`. Your submission will be written to `src.hpp` and compiled together with the OJ's driver code.

### Submission Guidelines

- The submitted code must be able to compile successfully through the above compilation process
- The program needs to be able to read data from standard input and write results to standard output
- Please ensure the code runs correctly within the given time and space limits
- **You must use C++ or C language** to implement this assignment

### Evaluation Notes

- The evaluation system will test your program using the provided test data
- The program output must exactly match the expected output (including format)
- Exceeding time or memory limits will be judged as the corresponding error type
- Please pay attention to the overall time performance of your code and the time complexity of each part of your algorithm.

### Academic Integrity

If any violations are found during evaluation or code review (including but not limited to using unconventional methods to pass test cases), your final score may be significantly reduced or become **0 points**.
