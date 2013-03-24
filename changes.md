# How to compile, and improve, C code that we found on the internet

This is the code for the play-by-email game Atlantis 1.0, and it's very old. It was written for MS-DOS, back in the days when that was what people ran on their home computers. As such, it does not run on modern compilers and operating systems without some serious modifications. This is me documenting the modifications I made to make the code palatable to modern systems.

Enno Rehling, March 2013

## Let's have some version control

I downloaded the [Atlantis 1.0](http://www.prankster.com/project/download/atlantis1.c) source code and dropped it into an atlantis directory on my computer. Then I set up a local git repository, because we all make mistakes and want to be able to rewind sometimes.

    $ mkdir atlantis
    $ cd atlantis
    $ wget http://www.prankster.com/project/download/atlantis1.c
    $ git init
    $ git add atlantis1.c
    $ git commit -m "Initial revision: the original Atlantis 1.0 source"

## using CMake as our build tool

I like CMake, because it takes away the agony of writing Makefiles. Here is my initial CMakelists.txt:

    cmake_minimum_required(VERSION 2.6)
    project (atlantis C)
    add_executable(atlantis atlantis1.c)

Again, I add this file to source control. Because I like to build my projects outside of the source tree, I create a build directory that I exclude from git, because binaries do not belong in a repository, ever. I also like to exclude backup files:

    $ echo  CMakeLists.txt >> .gitignore
    $ echo  '*~' >> .gitignore
    $ git add CMakeLists.txt .gitignore
    $ git commit -m "use cmake to build"
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Those last two steps create the Makefiles to build the project, and try to build it.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/76c4f888c40eeab0c0025089b3a3454fcc8066b8)

## compiler history, and missing headers.

The first build fails with an error message `atlantis1.c:10:16: fatal error: io.h: No such file or directory`. What is [io.h](http://www.digitalmars.com/rtl/io.html) and why don't we have it? A few seconds with Google yield the answer: It's a header that was shipped with old versions of Digital Mars and Microsoft's compilers, and because Atlantis was written for MS-DOS, it's probably safe to assume that Russell was using one of those. The header is not, and was never, part of an official C standard, and it's safest to not depend on any compiler-specific features whenever we can, so we'll remove the line and try again. Some function declared in that header will eventually not be recognized, but we'll deal with that when it happens.
The same goes for [direct.h](http://www.digitalmars.com/rtl/dos.html) and [dos.h](http://www.digitalmars.com/rtl/dos.html), which include operating-specific functions to deal with the filesystem, mostly.
At this point, we start compiling actual code, and hit the first warnings and errors. So many of them, they scroll by too fast to see, and the warnings obscure the errors. Rule #1 of good programming: We don't tolerate warnings. A warning is a sign that something fishy is going on. So we edit CMakeLists.txt and add a few compiler flags for gcc, especially -Werror:

    IF(CMAKE_COMPILER_IS_GNUCC)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unknown-pragmas -Wstrict-prototypes -Wpointer-arith -Wno-char-subscripts -Wno-long-long")
    ELSE(CMAKE_COMPILER_IS_GNUCC)
        MESSAGE(STATUS "Unknown compiler ${CMAKE_C_COMPILER_ID}")
    ENDIF(CMAKE_COMPILER_IS_GNUCC)

YMMV, obviously, depending on what compiler you are using. The beauty of CMake is that you can define different rules for each toolset, whether it's visual C++, gcc, or something entirely different. Personally, I like [TinyCC](http://bellard.org/tcc/).

## solve all the simple problems.

There are a lot of simple problems with this code.
1. "missing braces around initializer" for arrays that have two dimensions, but are initialized with a single-dimensional array.
2. "return type defaults to ‘int’" means the function doesn't have a return type, and gcc will just return 0. Chances are that the code means to return an int, so we add the correct type to the function signature to make the warnings go away.
3. "type defaults to ‘int’ in declaration of ‘skills’" is similar to the previous warning. Sometimes variables are declared without a type, and old C compilers will assume that you meant to give them an int type, because in C, that is the safest bet, and on old architectures, it is 4 byte wide and convertible to almost anything. We are going to be explicit and will spell out the actual type everywhere.
4. "unknown type name ‘_cdecl’" is a bit of a tragedy. I have no idea why it's in this code, but it must have been meant as an optimization. It's advice to the compiler about how to hand parameters to this function, and like all premature optimizations, it is best ignored. We'll remove all occurrences of it from the code.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/2e48f3f710a5919c6807df6c38c11fcdf893f569)

## qsort signature and const-correctness

Here's an interesting error: "passing argument 4 of ‘qsort’ from incompatible pointer type". The offending line of code is

    qsort (v,n,width + 4,scramblecmp);
What's wrong with this? qsort takes a function as argument 4 which compares two elements in the array, and `man qsort` tells us that it's signature is 

    int (*compar)(const void *, const void *);
Looking at scramblecmp, it takes two arguments of type void \*, notice the absence of the const modifier. specifying an argument as const means that the function receiving the argument is not allowed to modify it, and it's helpful for the reader of a codebase to know that there is at least one side-effect that the function you're looking at *doesn't* have. Since scramblecmp does not modify the arguments, we add the const modifier.

git revision 97f8caff4323c41f2e31d423823465f5698618b5

## old compilers and missing functions

We're getting a lot of warnings about "implicit declaration of function ‘name’". These fall into two categories: file system (mkdir, findfirst, findnext, unlink) and strings/memory (strcmpl, memicmp, strlwr). In both cases, this is mostly a cause of our removing the compiler-specific header files, so we use Google to figure out what these functions did and write replacements for them.

Let's start with strcmpl, which we find in the [Digital Mars documentation][dm_docs], where it says that "strcmp is case sensitive whereas strcmpi and strcmpl are not". So, basically, strcmpl is a case-insensitive version of strcmp. A lot of early C libraries have one of these functions, under different names: gcc has strcasecmp, Visual C++ has \_stricmp. What each of them has in common is that they are not part of any ANSI C standard, and for very good reasons. case-sensitive behavior is locale-dependent, and unless you assume that all strings are encoded in ASCII and only contain letters from the English alphabet (no umlauts, no accents, etc), it's impossible to write a correct function that converts a character from upper to lower case or vice versa. As an example, in Turkish, the lowercase letter for I does not have a dot on it, while the uppercase version of i does. [Wikipedia has a page about this][wp_dottedi].
Atlantis is an English PBEM, and all input is assumed to be in English, so even if a function like this is a bad idea in the general case, we can work with these assumptions. We could replace all occurrences of strcmpl with strcasecmp, and the code would compile, but we would just be replacing one compiler-dependency with a different one. Bad Form. Instead, we are going to add a layer between our code and the runtime library of whatever compiler we happen to be using. I'm going to call it rtl.h (RTL for runtime-library). Here is our initial version, which we're going to improve in the next section:

    #ifndef RTL_H
    #define RTL_H
    #if defined(__GNUC__)
    #include <strings.h>
    #define strcmpl(a, b) strcasecmp(a, b)
    #elif defined(_MSC_VER)
    #define strcmpl(a, b) \_stricmp(a, b)
    #endif
    #endif

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/cce5f785177d5c3586d74a2f2adf71b72bf17be6)

## platform-dependent code

If you've ever used configure, you've used a set of tools called autoconf, and they figure out what platform-dependent stuff your build can do, and write a config.h file that contains a ton of macros that your code can use. Instead of writing code for every possible compiler or platform, we want to achieve the same thing with CMake.

There is a pretty good explanation of how to achieve this [on the cmake website](http://www.cmake.org/Wiki/CMake:How_To_Write_Platform_Checks "How To Write Platform Checks") that I've been following. The result is that running make will create a config.h file that we include from rtl.h, and rtl.h no longer uses compiler-specific macros, but the macros from config.h instead:

    #include "config.h"
    #if defined(HAVE_STRINGS_H)
    #include <strings.h>
    #define strcmpl(a, b) strcasecmp(a, b)
    #elif defined(HAVE__STRICMP)
    #define strcmpl(a, b) _stricmp(a, b)
    #elif defined(HAVE_STRICMP)
    #define strcmpl(a, b) stricmp(a, b)
    #else
    #error "could not define strcmpl"
    #endif

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/d6d4b7f1e626ff233cb5f22d1d66bf024ab01b48)

## adding rtl.c to write some meatier shims

Now that we have this technique understood, we should build the same kind of shim for[strlwr](http://www.digitalmars.com/rtl/string.html#_strlwr). It's a function that does an in-place conversion of all uppercase letters to their lowercase equivalent, and there is no equivalent for it in ANSI C. To make matters worse, lots of runtime libraries only have a tolower function in ctype.h that converts an individual character, not one that deals with entire strings. So let's just write our own, adding an rtl.c file to our library wrapper.

    #include "rtl.h"
    #include <ctype.h>
    #if !defined(HAVE_STRLWR)
    char * rtl_strlwr(char * str) {
      for (;*str;++str) {
        *str = tolower(*str);
      }
      return str;
    }
    #endif
We add this file to our CMakeLists.txt rules, and let HAVE_STRLWR be created through config.h.in, and that's one less problem. While we're at it, let's also write a rtl_strcmpl function that implements strcmpl if the compiler's library has none of the well-known substitutes.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/7a1182d868bf71582890df41ef902c9c64cd09ba)

## testing, 1 2 3

Now that we're writing code of our own, it's time to write tests, too. I like the [CuTest](http://cutest.sourceforge.net/) unit test framwework for C because it's small and incredibly simple to use. Because I use it a lot, I already have a slightly adapted version of it in a [github repository](https://github.com/badgerman/cutest), complete with CMake build. Let's add that as a submodule under the main atlantis/ directory:

    $ git submodule add git://github.com/badgerman/cutest.git
    $ git add cutest .gitmodules
    $ git commit

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/2938490e2d35780353b5c4e86dba4d68aa32cdf8)

The boilerplate for a simple CuTest file that does nothing looks like this:

    #include <CuTest.h>
    #include <stdio.h>
    #include "rtl.h"
    
    int main(int argc, char ** argv) {
      CuString *output = CuStringNew();
      CuSuite *suite = CuSuiteNew();
    
      CuSuiteRun(suite);
      CuSuiteSummary(suite, output);
      CuSuiteDetails(suite, output);
      printf("%s\n", output->buffer);
      return suite->failCount;
    }

We add this to our CMakeLists.txt rules by adding an executable and adding it as a test (with ADD_TEST), as well as adding the cutest library to it, and then run the tests:

    $ make rtl_tests CuTestTest test
    Running tests...
    Test project /home/enno/atlantis/build
        Start 1: rtl
    1/2 Test #1: rtl ..............................   Passed    0.00 sec
        Start 2: cutest
    2/2 Test #2: cutest ...........................   Passed    0.00 sec
    
    100% tests passed, 0 tests failed out of 2
    
    Total Test time (real) =   0.01 sec

There were already tests for cutest in the cutest/CMakeLists.txt file that we include, so a total of two tests are being executed, and since we haven't written anything that can fail, we're passing all tests.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/35a2dea6d5e7a379b0a7e250c0a037bface39a3a)

Sadly, there's a [problem in CMake](http://stackoverflow.com/questions/733475/cmake-ctest-make-test-doesnt-build-tests "Discussion of this bug on SO") that causes it to not add the test executable as a dependency to the test target, so we need to do this manually. following the advice from StackOverflow we're adding a new target named "check" to the CMakeLists.txt rules that lists all the dependencies. From here on, we can run `make check` instad of `make test` and things will Just Work.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/051f69e9f44a0848f6510259983fe2d37d67c157)

Finally, we extend rtl.test.c with a couple of tests that cover the two functions we just wrote, and add them to the test suite. In rtl.tests.c, we will test both our custom implementation and the implementation that is chosen by config.h:

    SUITE_ADD_TEST(suite, test_strcmpl);
    SUITE_ADD_TEST(suite, test_rtl_strcmpl);

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/6a77cecede3e1d247c2f60a5d7d8c79761360521)

We wrap this up by also writing tests for strlwr. And these tests are already helping! It turns out that not only did I confuse strupr and strlwr in several places, but my implementation was also wrong because it returned the wrong value. Let's fix that real quick.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/a13f7d2a2f06ad76958b1fa76e9346c223fbd2f4)

The [memicmp](http://www.digitalmars.com/rtl/string.html#_memicmp) function is a differnet beast: It is basically memcmp, but treats each byte as a character and ignores case. Or in other words, it's the strcmpl function we dealt with earlier, but with a limited number of characters. With a few small changes, we can adapt the code we've already written to handle both scenarios. At least on gcc, we can also fall back on strncasecmp.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/44e60d2c10017fef331ee6bbc7715aa3d01e6ff1)

## DOS and file system functions

We're pretty much down to solving for findnext/findfirst now. These are functions to walk a directory in DOS, and the closest equivalent on a Unix system are the opendir/readdir/closedir functions. Their API is very different from findfirst/findnext, though, so we can't just build a simple shim for them. There are two places where findfirst is used: the `reports` function, and `initgame`. Both of these functions also use mkdir, which is different from one operating system to the next.
initgame uses findfirst/findnext to figure out what the most recent data file is, to determine the current turn. This is overly convoluted, and we might want to replace it with a command line argument. This has the added benefit of making it easier to re-run a turn of the game. First step: Accepting a command line argument. Second step: removing all the logic that calculates the global `turn` variable in `initgame`.

Currently, main is defined as `int main ()`, and not accepting any arguments. Let's accept an argument of the form -t123 or -t 123, and keep our options open for future arguments.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/2110733ada30dac70ad7dfb84e957b1cad963d4f)

We should be adding tests here, but the code is not structured well enough for that to be easy. Also, it would help if it was compiling, so let's remember this for when that's the case.

After we've cleaned up in initgame in the second step, the reports function is now our main problem. We'll get to that next.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/8248210d9715332faf1026df87531a08edd1c125)

## dealing with mkdir

We are going to write another RTL wrapper for mkdir. The problem with mkdir between DOS and other operating systems is that DOS did not require the programmer to specify permissions, because it didn't know more than one user anyway. On Unix systems, mkdir usually takes a second argument specifying permissions for the new directory, but this should generally be specified through [umask](https://en.wikipedia.org/wiki/Umask), and if the umask for our process is set correctly, we can just attempt to create the directory with maximum permissions (0777), and the OS will reduce that to the actually allowed permissions. It's not a good idea to second-guess what the user wants the permissions to be, anyway. Who knows, maybe there is a separate process for emailing the reports, or a web server, under another user (but in a shared group) that needs read-access to the reports? Let's use the OS environment deal with that.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/f5164e746223276c1b3f5b0fd30dc3d0ff4eabca)

## report directories

The final thing we need to do is that the reports() function tries to delete any old reports before it writes new ones. It uses findfirst/findnext to walk the `reports` and `nreports` directories, deleting anything in there. This is the sort of administrative stuff that is much better done in a script outside the server, or not at all, so we're going to just wholesale delete the code.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/d5b317cfc05a9023f42a06b9fc73096c97c3596f)

At this point, the server compiles for the first time.

## Success! Or is it?

We should do some basic functional testing now. It's surprising how few games have a basic test that verifies the game can even start, so let's be better than 70% of the professional games industry, and write that test. We want to test that we can start the server and entering a Q (for quit) exits it with an exit code of 0. For various reasons mostly to do with CMake, this is best done in a small shell script that receives the executable's location as an argument and looks like this:

    #!/bin/sh
    if [ ! -x $1 ] ; then
      echo "usage: $0 <executable>" ; exit 1
    fi
    echo Q | $1
We create this script in `tests/atlantis_starts`, and add a CMakeLists.txt file for tests in this directory that contains this command:

    ADD_TEST(NAME atlantis_starts
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/atlantis_starts $<TARGET_FILE:atlantis>)

This seems like a simple enough blueprint for any other functional tests we'll want to write in the future.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/c5652a00a2e122ca6756afcb38ce57e0fcd0158b)

## Other compilers, especially Visual C++

The good news is, we have a working atlantis server that compiled on Linux with gcc. The bad news is, not everyone has this setup, and what we've produced here will probably not compile in the Microsoft Visual C++ (or short, MSVC) compiler. That's a whole other story, though.

[dm_docs]: http://www.digitalmars.com/rtl/rtl.html "Digital Mars RTL documentation"
[wp_dottedi]: https://en.wikipedia.org/wiki/Dotted_and_dotless_I "Wikipedia on dotted vs. dotless I"
