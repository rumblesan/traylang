traylang
========

About
-----

The language frontend developed for my raytracer. Mostly a learning experiment.

Building
--------

```bash
cd build
cmake ..
make
make test
```

Installing
----------

Everything should be setup to 'just work', with CMake handling installing libs, headers and the executable to the right place.
```bash
make install
```

Bison & Flex
----

Traylang uses bison and flex for parsing, so these need to be installed.

On OSX, the following may be necessary to make sure that CMake finds the versions of bison and flex installed with homebrew.

```bash
export CMAKE_PREFIX_PATH=`brew --prefix`/opt/bison/bin:`brew --prefix`/opt/flex/bin
```

Contact
-------

guy@rumblesan.com if you have any questions

