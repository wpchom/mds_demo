#include <stdio.h>

// 内部实现函数
void doPrintArray(const char *const arr, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// 宏包装：只允许 const int[]
#define printArray(arr, len) \
    _Generic((arr), const char*: doPrintArray)(arr, len)

int main() {
    const char *c_arr = "aer";
    char v_arr[] = "erg";

    doPrintArray("c_arr", 3);      // ✅ 正确
    printArray(v_arr, 3);      // ❌ 编译报错（取决于编译器是否严格）

    return 0;
}