#pragma once


class Randumb
{
private:
    unsigned long long s;
public:
    Randumb(unsigned long long seed) {
        s = seed;
    };
    inline unsigned long long Next() {
        s = s * 12345 + 1; // modulus this by an odd number, an even number mod leads to alternating between odd and even numbers, therefore you also never get the same number twice in a row
        return s;
    }
    inline int NextRanged(const int min, const int max) {
        const unsigned int width = max - min;
        const int offset = max - width;
        return (Next() % width) + offset;
    }
    inline float NextFloat0To1() {
        const int resolution = 1000000001;
        if (s < 111111111) { // to prevent several low numbers in a row everytime the seed is below the resolution / 9
            s *= 12345;
        }
        return (float)(Next() % resolution) / resolution;
    }
    inline float NextRangedFloat(const float min, const float max) {
        const float r = NextFloat0To1();
        const float width = max - min;
        const float offset = max - width;
        return (width * r) + offset;
    }
    inline bool CoinFlip() {
        return (Next() % 100) >= 50;
    }
};


namespace trt {

    const unsigned int uCap = ~0;
    const int sCap = uCap >> 1;
    const int sCapMin = -sCap - 1;

    const unsigned short uCap16 = ~0;
    const short sCap16 = uCap16 >> 1;
    const short sCapMin16 = -sCap16 - 1;

    const unsigned char uCap8 = ~0;
    const char sCap8 = uCap8 >> 1;
    const char sCapMin8 = -sCap8 - 1;

    const double pi = 3.141592653589793238;
    const double sqrRt2 = 1.414213562373095048;
    const double sqrRt3 = 1.732050807568877293;
    const double goldenRatio = 1.618033988749894848;

    const float piF = 3.141592653f;
    const float sqrRt2F = 1.414213562f;
    const float sqrRt3F = 1.732050807f;
    const float goldenRatioF = 1.618033988f;


    struct Transform {
    public:
        float position[3];
        float right[3];
        float up[3];
        float forward[3];
    };
    struct Plane {
    public:
        float a[3];
        float b[3];
        float c[3];
        float n[3];
    };

    struct Int3 {
        int x = 0;
        int y = 0;
        int z = 0;
        inline bool operator==(const Int3& other) const {
            return x == other.x && y == other.y && z == other.z;
        }
    };

    /*
    // example of hash function for Int3
    template<> struct hash<Int3> {
        size_t operator()(Int3 const& int3) const {
            return ((int3.x ^ (int3.y << 1)) >> 1) ^ (int3.z << 1);
        }
    };
    */

    inline unsigned int randumb32(void) {
        static unsigned int z1 = 3, z2 = 7, z3 = 13, z4 = 19;
        z1 = z1 * 9 + 1; // equation gets all 4,294,967,296 numbers
        if (z1 == 3) {
            z2 = z2 * 9 + 1;
            if (z2 == 7) {
                z3 = z3 * 9 + 1;
                if (z3 == 13) {
                    z4 = z4 * 9 + 1;
                }
            }
        }
        return (z1 ^ z2 ^ z3 ^ z4);
        // 2^128 long pattern or 3.4 * 10^38
        // at 500 million a second, it would take you 21,580,566,141,612,028,377,941 years to compute all numbers
        // even China's 10 million processor supercomputer would still take 2 quadrillion years to compute
        // it is about 20X faster than rand() and about 5X faster than lfsr113_Bits()
    }
    inline unsigned short randumb16(void) {
        static unsigned short z1 = 3, z2 = 7, z3 = 13, z4 = 19;
        z1 = z1 * 9 + 1;
        if (z1 == 3) {
            z2 = z2 * 9 + 1;
            if (z2 == 7) {
                z3 = z3 * 9 + 1;
                if (z3 == 13) {
                    z4 = z4 * 9 + 1;
                }
            }
        }
        return (z1 ^ z2 ^ z3 ^ z4);
    }
    inline unsigned int lfsr113_Bits(void)
    {
        static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
        unsigned int b;
        b = ((z1 << 6) ^ z1) >> 13;
        z1 = ((z1 & 4294967294U) << 18) ^ b;
        b = ((z2 << 2) ^ z2) >> 27;
        z2 = ((z2 & 4294967288U) << 2) ^ b;
        b = ((z3 << 13) ^ z3) >> 21;
        z3 = ((z3 & 4294967280U) << 7) ^ b;
        b = ((z4 << 3) ^ z4) >> 12;
        z4 = ((z4 & 4294967168U) << 13) ^ b;
        return (z1 ^ z2 ^ z3 ^ z4);
    }

    inline void setBit(int& bytes, const char bitIndex, const bool value) {
        if (value) {
            bytes |= 1 << bitIndex;
        }
        else
        {
            bytes &= ~(1 << bitIndex);
        }
    }
    inline bool getBit(const int bytes, const char bitIndex) {
        return (bytes & (1 << bitIndex)) > 0;
    }
    inline void setByte(int& bytes, const char byte, const char byteIndex) {
        char shift = 8 * byteIndex;
        bytes &= ~(15 << shift); // clearing the byte
        bytes |= byte << shift; // setting the byte
    }
    inline char getByte(const int bytes, const char byteIndex) {
        return (bytes >> (byteIndex * 8)) & 255;
    }
    inline int getBits(const int bytes, const char startIndex, const char stopIndex) {
        return (bytes >> startIndex) & ~(~0 << stopIndex);
    }
    inline int shiftBitsRightFromIndex(const int bytes, const char index, const unsigned char shift) {
        const int bitsToKeepMask = ~0 << index;
        const int bitsToKeep = bytes & bitsToKeepMask;
        return ((bytes & ~bitsToKeepMask) >> shift) | bitsToKeep;
    }

    inline unsigned int isqrt(unsigned int num) {
        unsigned int res = 0;
        unsigned int bit = 1 << 30; // The second-to-top bit is set: 1L<<30 for long // "bit" starts at the highest power of four <= the argument.
        while (bit > num) {
            bit >>= 2;
        }
        while (bit != 0) {
            if (num >= res + bit) {
                num -= res + bit;
                res = (res >> 1) + bit;
            }
            else {
                res >>= 1;
            }
            bit >>= 2;
        }
        return res;
    }
    inline unsigned short isqrt16(unsigned short num) {
        unsigned short res = 0;
        unsigned short bit = 1 << 14; // The second-to-top bit is set: 1L<<30 for long
        // "bit" starts at the highest power of four <= the argument.
        while (bit > num)
            bit >>= 2;
        while (bit != 0) {
            if (num >= res + bit) {
                num -= res + bit;
                res = (res >> 1) + bit;
            }
            else
                res >>= 1;
            bit >>= 2;
        }
        return res;
    }
    inline void addIntNoOverflowOrUnderflow(int& n, const int add) {
        if (add >= 0) {
            if (n + add < n) {
                n = sCap;
                return;
            }
        }
        else {
            if (n + add > n) {
                n = sCapMin;
                return;
            }
        }
        n += add;
    }
    inline int getAddIntNoOverflowOrUnderflow(const int n, const int add) {
        if (add >= 0) {
            if (n + add < n) {
                return sCap;
            }
        }
        else {
            if (n + add > n) {
                return sCapMin;
            }
        }
        return n + add;
    }
    inline void addUnsignedIntNoOverflowOrUnderflow(unsigned int& n, const int add) {
        if (add >= 0) {
            if (n + add < n) {
                n = uCap;
                return;
            }
        }
        else {
            if (n + add > n) {
                n = 0;
                return;
            }
        }
        n = n + add;
    }
    inline int powInt(int n, const unsigned short p) {
        int nH = n;
        for (unsigned int i = 0; i < p; i++) {
            n *= nH;
        }
        return n;
    }


    inline void capInt(int &n, const int low, const int high) {
        if (n > high) {
            n = high;
            return;
        }
        if (n < low) {
            n = low;
        }
    }
    inline void cap(float& n, const float low, const float high) {
        if (n > high) {
            n = high;
            return;
        }
        if (n < low) {
            n = low;
        }
    }

    inline unsigned int fastIntDistSqr(const int* a, const int* b) {
        const int x = a[0] - b[0];
        const int y = a[1] - b[1];
        const int z = a[2] - b[2];
        return (x * x) + (y * y) + (z * z);
    }
    inline float distSqr(const float* a, const float* b) {
        const float x = a[0] - b[0];
        const float y = a[1] - b[1];
        const float z = a[2] - b[2];
        return (x * x) + (y * y) + (z * z);
    }
    inline int dotProductInt(const int* a, const int* b) {
        return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
    }
    inline float dotProduct(const float* a, const float* b) {
        return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
    }

    inline void addInt3s(int* dest, const int* n) {
        dest[0] += n[0];
        dest[1] += n[1];
        dest[2] += n[2];
    }
    inline void add3Int3s(int* dest, const int* n, const int* n1) {
        dest[0] += n[0] + n1[0];
        dest[1] += n[1] + n1[1];
        dest[2] += n[2] + n1[2];
    }
    inline void subInt3s(int* dest, const int* n) {
        dest[0] -= n[0];
        dest[1] -= n[1];
        dest[2] -= n[2];
    }
    inline void subFloat3s(float* dest, const float* f) {
        dest[0] -= f[0];
        dest[1] -= f[1];
        dest[2] -= f[2];
    }
    inline void getSubFloat3s(const float* f1, const float* f2, float* returnPtr) {
        returnPtr[0] = f1[0] - f2[0];
        returnPtr[1] = f1[1] - f2[1];
        returnPtr[2] = f1[2] - f2[2];
    }
    inline void multInt3ByInt(int* dest, const int n) {
        dest[0] *= n;
        dest[1] *= n;
        dest[2] *= n;
    }
    inline void multInt3s(int* dest, const int* n) {
        dest[0] *= n[0];
        dest[1] *= n[1];
        dest[2] *= n[2];
    }
    inline void multFloat3ByInt(float* dest, const int n) {
        dest[0] *= n;
        dest[1] *= n;
        dest[2] *= n;
    }
    inline void multFloat3ByF(float* dest, const float n) {
        dest[0] *= n;
        dest[1] *= n;
        dest[2] *= n;
    }
    inline void getFloat3MultByF(const float* float3, const float n, float* returnPtr) {
       returnPtr[0] = float3[0] * n;
       returnPtr[1] = float3[1] * n;
       returnPtr[2] = float3[2] * n;
    }
    inline void add3Float3s(float* dest, const float* n, const float* n1) {
        dest[0] += n[0] + n1[0];
        dest[1] += n[1] + n1[1];
        dest[2] += n[2] + n1[2];
    }

    inline void crossProductInt(const int* a, const int* b, int* returnPtr)
    {
        returnPtr[0] = (a[1] * b[2]) - (b[1] * a[2]);
        returnPtr[1] = (b[0] * a[2]) - (a[0] * b[2]);
        returnPtr[2] = (a[0] * b[1]) - (b[0] * a[1]);
    }
    inline void crossProduct(const float* a, const float* b, float* returnPtr)
    {
        returnPtr[0] = (a[1] * b[2]) - (b[1] * a[2]);
        returnPtr[1] = (b[0] * a[2]) - (a[0] * b[2]);
        returnPtr[2] = (a[0] * b[1]) - (b[0] * a[1]);
    }

    inline void calculateNormal(Plane& plane) {
        float abVector[3];
        getSubFloat3s(plane.a, plane.b, abVector);
        float acVector[3];
        getSubFloat3s(plane.a, plane.c, acVector);
        crossProduct(abVector, acVector, plane.n);
    }


    inline bool pointIsOnPositiveSideOfPlane(const float* point, const Plane plane)
    {
        float paVector[3];
        getSubFloat3s(point, plane.a, paVector);
        return dotProduct(plane.n, paVector) >= 0;
    }


    inline void localPointToWorldSpace(const Transform transform, const float* p, float* returnPtr) {

        float r[3];
        float u[3];
        float f[3];

        getFloat3MultByF(transform.right, p[0], r);
        getFloat3MultByF(transform.up, p[1], u);
        getFloat3MultByF(transform.forward, p[2], f);

        multFloat3ByF(r, p[0]);
        multFloat3ByF(u, p[1]);
        multFloat3ByF(f, p[2]);

        add3Float3s(r, u, f);

        returnPtr[0] = r[0] + transform.position[0];
        returnPtr[1] = r[1] + transform.position[1];
        returnPtr[2] = r[2] + transform.position[2];
    }

    inline bool aabbIsInFrustumPlanes(const float** corners, const Plane* frustumPlanes)
    {
        bool positiveSideOfPlane;
        for (int i = 0; i < 4; i++) // for each plane
        {
            positiveSideOfPlane = false;
            for (int j = 0; j < 8; j++) // for each corner
            {
                if (pointIsOnPositiveSideOfPlane(corners[j], frustumPlanes[i]))
                {
                    positiveSideOfPlane = true;
                    break; // at least one corner is in the frustmu
                }
            }
            if (!positiveSideOfPlane) // if all of the corners are outside of even one plane, we know it is not in the frustum
            {
                return false;
            }
        }
        return true; // if there is at least one corner on the positive side of all four frustum planes
    }

}