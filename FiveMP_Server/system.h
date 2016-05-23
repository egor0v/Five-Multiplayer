//----------------------------------------------------
//
// VC:MP Multiplayer Modification For GTA:VC
// Copyright 2004-2005 SA:MP team
//
// File Author(s): kyeman
// License: See LICENSE in root directory
//
//----------------------------------------------------

#pragma once

typedef struct _Vector3 {
	float X;
	DWORD _paddingx;
	float Y;
	DWORD _paddingy;
	float Z;
	DWORD _paddingz;
} Vector3, *PVector3;

typedef struct _VECTOR_PAD {
	float X,Y,Z,pad;
} VECTOR_PAD, *PVECTOR_PAD;

typedef struct _MATRIX2X4 {
	//float vecRoll[4];
	//VECTOR_PAD vLookUp;
	//VECTOR_PAD vLookAt;
	Vector3 vPos;
} MATRIX2X4, *PMATRIX2X4;

//----------------------------------------------------

#ifdef _WIN32
# include <windows.h>
#else
typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;
#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif
#define far
#define near
#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST  const
#endif

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL near           *PBOOL;
typedef BOOL far            *LPBOOL;
typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;
typedef char                CHAR;
typedef char				*PCHAR;
typedef int near            *PINT;
typedef int far             *LPINT;
typedef WORD near           *PWORD;
typedef WORD far            *LPWORD;
typedef long far            *LPLONG;
typedef DWORD near          *PDWORD;
typedef DWORD far           *LPDWORD;
typedef void far            *LPVOID;
typedef CONST void far      *LPCVOID;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif //!Win32

//----------------------------------------------------
