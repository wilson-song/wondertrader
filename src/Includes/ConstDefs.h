//
// Created by wilsonsong on 2023/7/29.
//
#pragma once

enum POSITION {
    POSITION_LONG = 1,
    POSITION_SHORT = 1<<1,
    POSITION_LONG_SHORT = POSITION_LONG | POSITION_SHORT,
};
