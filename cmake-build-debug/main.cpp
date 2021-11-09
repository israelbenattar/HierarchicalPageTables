#include <iostream>
#include <math.h>
//
//int translateAddress(uint64_t virtualAddress)
//{
//    uint64_t add[4];
//    add[0] = virtualAddress & ((uint64_t)pow(2, 3)-1);
//    int i = 1;
//    while (virtualAddress != 0)
//    {
//        virtualAddress = virtualAddress >> 3LL;
//        add[i] = virtualAddress & ((uint64_t)pow(2, 3)-1);
//        i++;
//    }
//
//    return 1;
//}

//int translateAddress(uint64_t virtualAddress)
//{
//    uint64_t add[3];
//    int i = 0;
//    while (i != 2)
//    {
//        uint64_t x = virtualAddress >> (4LL - i*2LL);
//        add[i] = x & ((uint64_t)pow(2, 2)-1);
//        i++;
//    }
//    add[2] = virtualAddress & ((uint64_t)pow(2, 2)-1);
//    return 1;
//}
//
//int main() {
//    std::cout << "Hello, World!" << std::endl;
//    std::cout << translateAddress(43) << std::endl;
//    return 0;
//}
