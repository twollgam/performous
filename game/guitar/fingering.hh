#pragma once

// 0 = empty string / Leersaite.
// -1 = not played / nicht angespielt
struct Fingering {
    int e_low = -1;
    int a = -1;
    int d = -1;
    int g = -1;
    int b = -1;
    int e_high = -1;

    int operator[](int n) const;
};
