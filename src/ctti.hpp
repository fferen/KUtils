/*
Copyright (c) 2012, Kevin Han
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

// Register a (non-template) type with the `TypeString` struct. Will also
// register pointer and reference types.
#define def_type(X) \
    template <> struct TypeString<X> { \
        static const char *value() { return #X; } \
    }; \
    template <> struct TypeString<X*> { \
        static const char *value() { return #X "*"; } \
    }; \
    template <> struct TypeString<X&> { \
        static const char *value() { return #X "&"; } \
    }

// Register a template type with the `TypeString` struct. The string will not
// contain template parameters types, just the base type.
#define def_templ_type(X) \
    template <typename... Ts> struct TypeString<X<Ts...>> { \
        static const char *value() { return #X; } \
    }

// The original definition. If type is not found, a link error will occur.
template <typename> 
struct TypeString {
    static const char *value();
};

def_type(unsigned char);
def_type(char);
def_type(unsigned short);
def_type(short);
def_type(unsigned int);
def_type(int);
def_type(unsigned long);
def_type(long);
def_type(long long);
def_type(bool);
def_type(float);
def_type(double);
def_type(long double);
