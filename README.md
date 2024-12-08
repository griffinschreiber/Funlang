# Funlang
A programming language, for fun :).

**NOTE:** this is not complete and may not even compile.

## Compilation
This project uses automake, which you can read about it here: [GNU -- creating amhello](https://www.gnu.org/software/automake/manual/html_node/Creating-amhello.html).

You can initiate the build system with `autoreconf --install` and configure it with `./configure`. Then you can make it normally with `make`, and run it with `./src/funlang`.

When changing `configure.ac`, `Makefile.am`, or `src/Makefile.am`, remember to run `./configure`.
