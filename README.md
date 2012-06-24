ctodo
=====
A simple ncurses-based task list manager

Website: http://ctodo.apakoh.dk

License
-------
Copyright (C) 2012 Niels Sonnich Poulsen (http://apakoh.dk)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Requirements
------------
* `ncurses`
* A C-compiler like `gcc`
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

    gcc -lncurses -o ctodo ctodo.c

Usage
-----
Run from a console using:

    ctodo

This will create a `todo.txt` file in the current
directory or open the file if it exists.

Alternatively you can run ctodo using:

    ctodo someotherfile.txt

Which will open/create `someotherfile.txt` in the current directory.

Commands
--------
* Navigation:

  Use `UP` and `DOWN` arrows to move between tasks
  (or `J` and `K`).

  `PAGE UP` moves 5 tasks up and `PAGE DOWN` moves
  5 tasks down.

  `HOME` moves to the top of the list and `END` moves
  to the bottom.

* Managing tasks:

  `SPACE` and `ENTER` checks/unchecks the selected task.

  `D` and `DELETE` deletes the selected task.

  Press `E` to edit the description of the selected task.

  Press `SHIFT-E` to delete the current task description
  and create a new one.
  
  Press `N` or `INSERT` to create a new task.

* In input-mode:
  
  Press `ENTER` to save the string.

  Press `CTRL-C` or `CTRL-D` to cancel.

* Managing the task list:

  Press `T` to edit the title of the task list.

  Press `SHIFT-T` to delete the current title
  and create a new one.

  Press `S` to save the list.

  Press `R` to reload the list (discard unsaved data).

  Press `Q` to save the list and quit.




