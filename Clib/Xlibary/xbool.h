#ifndef XBOOL_H
#define XBOOL_H



// types def unsigned char xbool;
typedef unsigned char xbool;

#define x_true  ((xbool)1)
#define x_false ((xbool)0)

// same type of bool from glibc 
// I thought we aren't glibc 
#define bool  xbool
#define true  x_true
#define false x_false

#endif // XBOOL_H
