#pragma once

#include <algorithm>
#include <string>

namespace mpc::lcdgui {
    struct MColor {
        int A, R, G, B;
        MColor(int a = 255, int r = 0, int g = 0, int b = 0) : A(a), R(r), G(g), B(b) {}
        bool operator==(const MColor& rhs) { return (rhs.A == A && rhs.R == R && rhs.G == G && rhs.B == B); }
        bool operator!=(const MColor& rhs) { return !operator==(rhs); }
        bool Empty() const { return A == 0 && R == 0 && G == 0 && B == 0; }
        void Clamp()
        {
            A = std::min<int>(A, 255);
            R = std::min<int>(R, 255);
            G = std::min<int>(G, 255);
            B = std::min<int>(B, 255);
        }
    };

    struct MColorF {
        float A, R, G, B;
        MColorF(float a = 1.0f, float r = 0.0f, float g = 0.0f, float b = 0.0f) : A(a), R(r), G(g), B(b) {}
        bool operator==(const MColorF& rhs) { return (rhs.A == A && rhs.R == R && rhs.G == G && rhs.B == B); }
        bool operator!=(const MColorF& rhs) { return !operator==(rhs); }
        bool Empty() const { return A == 0 && R == 0 && G == 0 && B == 0; }
        void Clamp()
        {
            A = std::min<float>(A, 1.0f);
            R = std::min<float>(R, 1.0f);
            G = std::min<float>(G, 1.0f);
            B = std::min<float>(B, 1.0f);
        }
    };

    static void setPixel3(unsigned char* pixel, const MColor& color)
    {
        pixel[0] = color.R;
        pixel[1] = color.G;
        pixel[2] = color.B;
    }

    static void setPixel3F(unsigned char* pixel, const MColorF& color)
    {
        auto colorInt = MColor(color.A * 255.0f, color.R * 255.0f, color.G * 255.0f, color.B * 255.0f);
        setPixel3(pixel, colorInt);
    }

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

        inline MRECT Intersect(MRECT* pRHS)
        {
            if (Intersects(pRHS))
            {
                return MRECT(std::max<int>(L, pRHS->L), std::max<int>(T, pRHS->T), std::min<int>(R, pRHS->R), std::min<int>(B, pRHS->B));
            }
            return MRECT();
        }

        inline bool Intersects(MRECT* pRHS)
        {
            return (!Empty() && !pRHS->Empty() && R >= pRHS->L && L < pRHS->R && B >= pRHS->T && T < pRHS->B);
        }

        inline bool Contains(MRECT* pRHS)
        {
            return (!Empty() && !pRHS->Empty() && pRHS->L >= L && pRHS->R <= R && pRHS->T >= T && pRHS->B <= B);
        }

        inline bool Contains(int x, int y)
        {
            return (!Empty() && x >= L && x < R && y >= T && y < B);
        }

        inline void Constrain(int* x, int* y)
        {
            if (*x < L)
            {
                *x = L;
            }
            else if (*x > R)
            {
                *x = R;
            }

            if (*y < T)
            {
                *y = T;
            }
            else if (*y > B)
            {
                *y = B;
            }
        }

        inline MRECT SubRectVertical(int numSlices, int sliceIdx)
        {
            float heightOfSubRect = (float(H()) / numSlices);
            int t = (int) heightOfSubRect * sliceIdx;

            return MRECT(L, T + t, R, T + t + heightOfSubRect);
        }

        inline MRECT SubRectHorizontal(int numSlices, int sliceIdx)
        {
            float widthOfSubRect = (float(W()) / numSlices);
            int l = widthOfSubRect * sliceIdx;

            return MRECT(L + l, T, L + l + widthOfSubRect, B);
        }

        inline MRECT GetPadded(int padding)
        {
            return MRECT(L - padding, T - padding, R + padding, B + padding);
        }

        inline MRECT GetPadded(int padL, int padT, int padR, int padB)
        {
            return MRECT(L + padL, T + padT, R + padR, B + padB);
        }

        inline MRECT GetHPadded(int padding)
        {
            return MRECT(L - padding, T, R + padding, B);
        }

        inline MRECT GetVPadded(int padding)
        {
            return MRECT(L, T - padding, R, B + padding);
        }

        void Clank(MRECT* pRHS)
        {
            if (L < pRHS->L)
            {
                R = std::min<int>(pRHS->R - 1, R + pRHS->L - L);
                L = pRHS->L;
            }
            if (T < pRHS->T)
            {
                B = std::min<int>(pRHS->B - 1, B + pRHS->T - T);
                T = pRHS->T;
            }
            if (R >= pRHS->R)
            {
                L = std::max<int>(pRHS->L, L - (R - pRHS->R + 1));
                R = pRHS->R - 1;
            }
            if (B >= pRHS->B)
            {
                T = std::max<int>(pRHS->T, T - (B - pRHS->B + 1));
                B = pRHS->B - 1;
            }
        }
    };
}
