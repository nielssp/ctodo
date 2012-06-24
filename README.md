ctodo
=====
A simple ncurses-based tasklist manager

License
-------
MIT

Requirements
------------
* `ncurses`
* `cmake` (not actually necessary)

Installation
------------
* Run cmake:

        cmake .

* Compile:

         make

* Install:

         make install

Alternatively you could just use `gcc` to compile the
single source file:

    gcc -lncurses -o ctodo ctodo

Usage
-----
Run from a console using:

    ctodo

This will create a `todo.txt` file in the current
directory or open the file if it exists.

Alternatively you can run cdoto using:

    ctodo someotherfile.txt

Which will open/create `someotherfile.txt` in the current directory.

