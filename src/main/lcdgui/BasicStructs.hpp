#pragma once

#include <algorithm>
#include <string>

namespace mpc::lcdgui {
    struct MRECT {
        int L, T, R, B;

        MRECT() { L = T = R = B = 0; }
        MRECT(int l, int t, int r, int b) : L(l), T(t), R(r), B(b) {}

        bool Empty() const
        {
            return (L == 0 && T == 0 && R == 0 && B == 0);
        }

        inline std::string getInfo() const
        {
            return "x " + std::to_string(L) + ", y " + std::to_string(T) + ", w " + std::to_string(W()) + ", h " + std::to_string(H());
        }

        void Clear()
        {
            L = T = R = B = 0;
        }

        bool operator==(const MRECT& rhs) const
        {
            return (L == rhs.L && T == rhs.T && R == rhs.R && B == rhs.B);
        }

        bool operator!=(const MRECT& rhs) const
        {
            return !(*this == rhs);
        }

        inline int W() const { return R - L; }
        inline int H() const { return B - T; }
        inline float MW() const { return 0.5f * (float) (L + R); }
        inline float MH() const { return 0.5f * (float) (T + B); }

        inline MRECT Union(MRECT* pRHS)
        {
            if (Empty()) { return *pRHS; }
            if (pRHS->Empty()) { return *this; }
            return MRECT(std::min<int>(L, pRHS->L), std::min<int>(T, pRHS->T), std::max<int>(R, pRHS->R), std::max<int>(B, pRHS->B));
        }
    };
}
