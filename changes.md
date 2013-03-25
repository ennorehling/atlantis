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

## CMake and MSVC

It is now time to try our code with a second compiler. Writin code that works on one compiler is easy, but we have gone out of our way to make it compiler-agnostic. Even our build scripts are mostly compiler-agnostic, but the proof of that is in the pudding. Microsoft's Visual C++ compiler is in many ways the polar opposite of gcc: It omes with a big IDE, doesn't use Makefiles, but its own proprietary solution file format, and supports a differnt subset of the C standard as well as shipping with a differnt C library.

The best news is that CMake doesn't just generate Makefiles, but can create Solution files for a number of MSVC versions as well (there are a number of other supported toolchains, actually). Before we install and configure CMake for Windows, we are going to add a folder of custom cmake modules to the project that I wrote earlier. It's in the git://github.com/eressea/cmake.git repository:

    git submodule add git://github.com/eressea/cmake.git

The file we are interested from this is cmake/Modules/FindMSVC.cmake, and it contains some macros to change compiler settings for MSVC that we're going to use. We need to configure CMake with a CMAKE_MODULE_PATH that points at this Modules directory (you can do that from the CMake GUI using the `Add Entry` button), then configure and generate our project. Select the generator that matches your version of MSVC (I'm testing this with Microsoft Visual Studio 10). Open the Solution in Visual Studio and build it.

## min and max

Trying to compile, we see this:

    3>C:\Users\Enno\Documents\Eressea\atlantis1\atlantis1.c(23): warning C4005: 'min' : macro redefinition
    3>          C:\Program Files\Microsoft Visual Studio 10.0\VC\include\stdlib.h(855) : see previous definition of 'min'

This is one of the most common problems with old C code. The C standard does not prescribe the existence of macros for min and max, but many programs want them, and define their own. They are so popular that most compilers added them eventually, but each in a different header or under a different name. We're going to remove these defines and add our own to rtl.h. There is no point in re-using any pre-existing macros, because these are so easy to write. We'll side-step the whole issue by renaming min and max to MIN and MAX, which doesn't collide with any existing headers that I know of.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/21893c165ebdbee1757612bfad2d3d35d823a850)

## Microsoft is reinventing the libc

This is a common sight when trying to compile C code with MSVC:

    C:\Users\Enno\Documents\Eressea\atlantis1\atlantis1.c(1152): warning C4996: 'strcat': This function or variable may be unsafe. Consider using strcat_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
              C:\Program Files\Microsoft Visual Studio 10.0\VC\include\string.h(110) : see declaration of 'strcat'

In their wisdom, Microsoft have taken it upon themselves to deprecate valid functions in the standard C library, and supplying their own versions that are somehow "safer", but using them would take us down the terrible path of writing compiler-dependent code, something we've been trying to avoid. We'd much rather turn off that deprecation warning, and continue to risk whatever dangers there are. The warning gives us a pretty clear recipe for doing that: define _CRT_SECURE_NO_WARNINGS. We're going back to our CMakeLists.txt file, and if the compiler is MSVC, we want to add that to the CFLAGS. So far, we have only ever defined CFLAGS for gcc, but there is a CMake module in the cmake submodule we just added that defines a function for doing that:
    IF (MSVC)
        find_package (MSVC MODULE)
        MSVC_CRT_SECURE_NO_WARNINGS (${PROJECT_NAME} atlantis)
    ENDIF (MSVC)

## Microsoft and POSIX names

Here is another warning that only Microsoft's compiler will give you:

    C:\Users\Enno\Documents\Eressea\atlantis1\atlantis1.c(3068): warning C4996: 'memicmp': The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: _memicmp. See online help for details.
              C:\Program Files\Microsoft Visual Studio 10.0\VC\include\string.h(93) : see declaration of 'memicmp'

Microsoft decided to rename their library extensions with a leading underscore, because implementation specific extensions should use names starting with an underscore in the global namespace if they want to adhere to the C or C++ Standard. That is actually a quite sensible move, even if it breaks compatibility with just about any existing program that used those extensions. Serves you right for writing compiler-specific code! We should go along with this, and rename the extensions in our rtl.h file to have a leading underscore, also. That means strlwr, memicp, etc. will get a leading underscore, and we will fix the Atlantis code to call the underscore functions instead.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/f132777e41e831820aa498c489d521cde19994c1)

## Integers, Characters, and other abominations

We are now left with code that compiles, but still has warnings about integers being converted to characters, like this:

    warning C4244: '=' : conversion from 'int' to 'char', possible loss of data

What MSVC is warnign us about here is that the code is assigning a value from an int to a variable that only holds a char. In C, these types are not guaranteed to be any particular size, and they are usually not the same size, so there are valid int values that do not fit into a char, and would be truncated. For a common 32-bit system, a char is usually 1 byte and holds values from 0 to 255, while an int is 32 bits wide and can hold values between negative and positive 2 billion. When we assign from the latter to the former, we need to be very sure that the value in the int is inside the 0-255 range.

### Boolean values

Prior to C99, there was no bool type in C, but any numeric type can store valuees of 0 and 1, so everyone just helped themselves to whatever type they could find. One example in this code base is the following:

    f2->seesbattle = ispresent (f2,r);

Here, `seesbattle` is a char, while ispresent() returns an int. We know this assignment is going to be okay, because all values are in the [0,1] range. we could just add a (char) type coercion here to suppress the warning. Alternatively, we can ask ourselves "why is seesbattle a char?", and the most likely reason here is memory. Back in the days of MS-DOS, this game had to run inside 640KB of memory, and every byte was precious, so saving 3 bytes for every fction in the game must have seemd like a good idea. Given that modern computers have gigabytes of memory, this would be a premature optimization if we wrote it today, so it sounds like we should just make `seesbattle` an int instead. But since we're in the future now, why can't we just use bool? Good question.
Compiler vendors are notoriously slow at shipping new language features, and a lot of systems run compilers that are either not fully ANSI-compliant, or comply with some old version of it. For a long time, my personal rule of thumb was that if it wasn't in C89, I wouldn't use it, but for bool, I'm willing to make an exception, because having a specialized type for boolean values is expressive and useful. The problem arises when you interact with a code base or compiler that have defined their own boolean types, either as a typedef or #define. After a lot of false starts, I have arrived at a bool.h header that I use in situations like this which looks like so:

    #if HAVE_STDBOOL_H
    # include <stdbool.h>
    #else
    # if ! HAVE__BOOL
    #  ifdef __cplusplus
    typedef bool _Bool;
    #  else
    typedef unsigned char _Bool;
    #  endif
    # endif
    # define bool _Bool
    # define false 0
    # define true 1
    # define __bool_true_false_are_defined 1
    #endif

The header stdbool.h is defined i C99, and the best possible implementation of a boolean. If our system doesn't have it, we typedef bool to an unsigned char, with a little bit of #define magic that overrides the case where someone has already defined a bool.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/0a2cdcf9239a0218559b7435a0c9d4b8e5098e5c)

I forgot to add the required entries to config.h.in, which is a constant source of irritation, so to make that work with gcc, I needed to make a few small tweaks.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/b0c877a113f997272d0c387abb5064cde2bfb47f)

### stdio functions returning int

Another source of int-to-char conversion warnings are functions like fgetc() that return a character as an int value. These functions always return a character (or EOF), so most of the time, casting the return value to (char) is a fairly safe way to get rid of the warning. Also, for our newly created bool type, assigning an int to it is going to cause a warning message, so we want to rewrite those lines, too.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/0d198c97baea585e1b1df6f96c1b87de67d1d27e)

### enum vs. int

There is a curious line in the code that makes ships:

    sh->type = i;

As you'd expect, i is an int that contains the intended ship-type, but sh-type is a char (again, for space reasons). If we ignore the space reasons, we could simply mak ship.type an int, but we can do better: ship types are not ints, because their valid domain is the set of values (SH_LONGBOAT, SH_CLIPPER, SH_GALLEON) which are defined in an enum at the top of the code. Let's turn that enum into a typedef (I'm going to call the type ship_t), and use ship_t everywhere. This is more expressive than using an int everywhere, and will come in handy if we ever use a debugger, because if it's smart, it will tell us the name of the enum value instead of just an integer that we need to decode.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/94726e57b4e385eb6516f08bcf95584c7b2f683a)

Next, let's add the spell_t, terrain_t, and skill_t enum types.

View these latest changes on [github](https://github.com/badgerman/atlantis1/commit/6afc66e46af191ec19d552e7982ee67805cddde5)

At this point, the code is compiling wihtout warnings on two compilers. That's pretty neat. In fact, because we've done such a good job at writing the code and the build scripts in a compiler-agnostic way, it should run on more than that. Installing TinyCC on my linux machine, I can configure CMake to use tcc instead of gcc:

    cmake .. -DCMAKE_C_COMPILER=tcc

And voila! the code compiles and works without any further modifications.

[dm_docs]: http://www.digitalmars.com/rtl/rtl.html "Digital Mars RTL documentation"
[wp_dottedi]: https://en.wikipedia.org/wiki/Dotted_and_dotless_I "Wikipedia on dotted vs. dotless I"
