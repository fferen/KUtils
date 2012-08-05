Miscellaneous C++ utilities, mainly with a focus on math and computer vision using OpenCV.

Major features (not an exhaustive list):

* Computer vision
    * [Hand gesture recognition framework][cfinder]
        * Fingertip detection
        * Color-based skin segmentation
* OSX-specific
    * [Simple interface][osx] for mouse control and screen scraping
* Math
    * Implementation of [automatic differentiation][ad]
* Sequences
    * Python's [xrange][xrange] and [range][range].
    * [Functional programming tools][func] like `map`, `filter`, `any`, `all`
* I/O
    * Convenience functions [print][print] and [printImm][printImm] to replace the messy `cout <<` syntax (simply calls `cout` underneath).
    * Overloaded `operator<<` to print many common types, including STL containers, `unordered_map`s, `pair`s, and most OpenCV types, using a uniform, XML-like format

See the full documentation [here][docs].

The code relies heavily on C++11 features, so will not build with older compilers.

I haven't had time to write a proper configuration script/Makefile, so the included Makefile is specific to my system. Sorry :X

[cfinder]: http://fferen.github.com/KUtils/classhumancv_1_1_cursor_finder.html
[osx]: http://fferen.github.com/KUtils/namespaceosx.html
[ad]: http://fferen.github.com/KUtils/namespaceautodiff.html#details
[xrange]: http://fferen.github.com/KUtils/namespaceseq_1_1math.html#abfe793e999a374a4d5e6b1ef3f268b59
[range]: http://fferen.github.com/KUtils/namespaceseq_1_1math.html#a6bd86d848fb47f455aff84c38c175ea4
[func]: http://fferen.github.com/KUtils/namespaceseq_1_1functional.html
[print]: http://fferen.github.com/KUtils/namespaceio.html#ab9528b2fe57a002bb153f1b9f8dde743
[printImm]: http://fferen.github.com/KUtils/namespaceio.html#ae85db8edfd95c17427c2606360bb97af
[osx_cv]: http://fferen.github.com/KUtils/namespaceosx__cv.html
[docs]: http://fferen.github.com/KUtils/namespaces.html
