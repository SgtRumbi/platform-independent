#if !defined(PFIND_MATH_H)

union v2 {
    struct {
        real32 x, y;
    };
    struct {
        real32 X, Y;
    };
    struct {
        real32 U, V;
    };
    real32 E[2];
};

union v3 {
    struct {
        real32 x, y, z;
    };
    struct {
        real32 X, Y, Z;
    };
    struct {
        real32 R, G, B;
    };
    struct {
        v2 XY;
        real32 _Unused0;
    };
    struct {
        real32 _Unused1;
        v2 YZ;
    };
    real32 E[3];
};

union v4 {
    struct {
        real32 x, y, z, w;
    };
    struct {
        real32 X, Y, Z, W;
    };
    struct {
        real32 R, G, B, A;
    };
    struct {
        v3 XYZ;
        real32 _Unused0;
    };
    real32 E[4];
};

union m4 {
    struct {

    };
    real32 E[16];
};

inline v2
V2() {
    v2 Result = {};
    return(Result);
}

inline v2
V2(real32 Both) {
    v2 Result;
    Result.x = Both;
    Result.y = Both;
    return(Result);
}

inline v2
V2(real32 X, real32 Y) {
    v2 Result;
    Result.x = X;
    Result.y = Y;
    return(Result);
}

inline v2
operator+(v2 A, v2 B) {
    v2 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    return(Result);
}

inline v2
operator-(v2 A, v2 B) {
    v2 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    return(Result);
}

inline v2
operator*(v2 A, v2 B) {
    v2 Result;
    Result.X = A.X*B.X;
    Result.Y = A.Y*B.Y;
    return(Result);
}

inline v2
operator/(v2 A, v2 B) {
    v2 Result;
    Result.X = A.X/B.X;
    Result.Y = A.Y/B.Y;
    return(Result);
}

inline v3
V3() {
    v3 Result = {};
    return(Result);
}

inline v3
V3(real32 All) {
    v3 Result;
    Result.X = All;
    Result.Y = All;
    Result.Z = All;
    return(Result);
}

inline v3
V3(real32 X, real32 Y, real32 Z) {
    v3 Result;
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    return(Result);
}

inline v3
operator+(v3 A, v3 B) {
    v3 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    return(Result);
}

inline v3
operator-(v3 A, v3 B) {
    v3 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    return(Result);
}

inline v3
operator*(v3 A, v3 B) {
    v3 Result;
    Result.X = A.X*B.X;
    Result.Y = A.Y*B.Y;
    Result.Z = A.Z*B.Z;
    return(Result);
}

inline v3
operator/(v3 A, v3 B) {
    v3 Result;
    Result.X = A.X/B.X;
    Result.Y = A.Y/B.Y;
    Result.Z = A.Z/B.Z;
    return(Result);
}

inline v4
V4() {
    v4 Result = {};
    Result.W = 1.0f;
    return(Result);
}

inline v4
V4(real32 XYZ) {
    v4 Result;
    Result.X = XYZ;
    Result.Y = XYZ;
    Result.Z = XYZ;
    Result.W = 1.0f;
    return(Result);
}

inline v4
V4(real32 X, real32 Y, real32 Z) {
    v4 Result;
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    Result.W = 1.0f;
    return(Result);
}

inline v4
operator+(v4 A, v4 B) {
    v4 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    Result.W = A.W + B.W;
    return(Result);
}

inline v4
operator-(v4 A, v4 B) {
    v4 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    Result.W = A.W - B.W;
    return(Result);
}

inline v4
operator*(v4 A, v4 B) {
    v4 Result;
    Result.X = A.X*B.X;
    Result.Y = A.Y*B.Y;
    Result.Z = A.Z*B.Z;
    Result.W = A.W*B.W;
    return(Result);
}

inline v4
operator/(v4 A, v4 B) {
    v4 Result;
    Result.X = A.X/B.X;
    Result.Y = A.Y/B.Y;
    Result.Z = A.Z/B.Z;
    Result.W = A.W/B.W;
    return(Result);
}

#define PFIND_MATH_H
#endif // PFIND_MATH_H
