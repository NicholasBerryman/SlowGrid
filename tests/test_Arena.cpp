//
// Created by nickberryman on 1/12/25.
//

#include "Logger.h"
#include "cstdint"
#define assert Logging::assert_except

import Logger;
import SG_Allocator;
void testArena(){
    SG_Allocator::Arena_ULL<5,3> arena;

    // Test alloc
    assert(arena.usedSpace() == 0);
    assert(arena.maxSize() == 5);
    auto alloc1 = arena.alloc<char>();
    *alloc1 = 'A';
    arena.sublifetime_open();
    assert(arena.usedSpace() == 1);
    assert(arena.maxSize() == 5);
    auto alloc2 = arena.alloc<uint32_t>();
    *alloc2 = 280;
    arena.sublifetime_open();
    assert(arena.usedSpace() == 5);
    assert(arena.maxSize() == 5);
    assert(*alloc1 == 'A');
    assert(*alloc2 == 280);

    // Test alloc beyond initial block
    auto alloc3 = arena.alloc<uint32_t>();
    *alloc3 = 0xFFFFFFFF;
    arena.sublifetime_open();
    assert(arena.usedSpace() == 9);
    assert(arena.maxSize() == 10);
    assert(*alloc3 == 0xFFFFFFFF);
    auto alloc4 = arena.alloc<char>();
    *alloc4 = 'B';
    assert(arena.usedSpace() == 10);
    assert(arena.maxSize() == 10);
    assert(*alloc3 == 0xFFFFFFFF);
    assert(*alloc4 == 'B');

    //Test fragment alloc
    auto alloc5 = arena.alloc<uint32_t>();
    *alloc5 = 0x10101010;
    assert(arena.usedSpace() == 14);
    assert(arena.maxSize() == 15);
    auto alloc6 = reinterpret_cast<uint32_t*>(arena.allocArray<uint8_t>(4));
    *alloc6 = 0x01010101;
    assert(arena.usedSpace() == 19);
    assert(arena.maxSize() == 20);
    assert(*alloc5 == 0x10101010);
    assert(*alloc6 == 0x01010101);

    // Test size limits
    LOGGER_ASSERT_ERROR( arena.alloc<uint64_t>(); )
    LOGGER_ASSERT_ERROR( arena.allocArray<char>(6); )

    // Test soft rollback
    arena.sublifetime_softRollback();
    assert(arena.maxSize() == 20);
    assert(arena.usedSpace() == 9);
    assert(*alloc3 == 0xFFFFFFFF);
    auto alloc7 = arena.alloc<char>();
    *alloc7 = 0x00;
    assert(*alloc7 == 0x00);
    assert(*alloc4 == 0x00);
    auto alloc8 = arena.alloc<uint16_t>();
    assert(alloc5 == reinterpret_cast<uint32_t*>(alloc8) && *alloc5 == 0x10101010);
    *alloc8 = 0xAA;
    assert(*alloc8 == 0xAA);
    assert(*alloc5 != 0x10101010);
    assert(arena.usedSpace() == 12);
    assert(arena.maxSize() == 20);
    assert(*alloc6 == 0x01010101);

    // Test hard rollback
    arena.sublifetime_rollback();
    assert(arena.maxSize() == 5);
    assert(arena.usedSpace() == 5);
    assert(*alloc1 == 'A');
    assert(*alloc2 == 280);

    auto alloc9 = arena.alloc<uint16_t>();
    *alloc9 = 0xCCC0;
    assert(*alloc9 == 0xCCC0);
    assert(arena.usedSpace() == 7);
    assert(arena.maxSize() == 10);

    // Test sublifetime limit error
    arena.sublifetime_softRollback();
    LOGGER_ASSERT_ERROR( arena.sublifetime_rollback(); )
    LOGGER_ASSERT_ERROR( arena.sublifetime_softRollback(); )
    arena.sublifetime_open();
    arena.sublifetime_open();
    arena.sublifetime_open();
    LOGGER_ASSERT_ERROR( arena.sublifetime_open(); )


    // Test allocConstruct
    struct consTest {
            char test = 'a';
            char test2 = 'b';
            consTest(char a, char b){test = a; test2 = b;}
    };
    consTest* construct = arena.allocConstruct<consTest>('A','B');
    assert(construct->test == 'A');
    assert(construct->test2 == 'B');

    consTest* construcArr = arena.allocConstructArray<consTest>(2, 'A','B');
    assert(construcArr[0].test  == 'A');
    assert(construcArr[0].test2 == 'B');
    assert(construcArr[1].test  == 'A');
    assert(construcArr[1].test2 == 'B');

    // Test fail on non-fundamental alloc (Manual)
    //arena.alloc<consTest>();
}

int main(int, char**) {
    testArena();
    return 0;
}

#undef assert

