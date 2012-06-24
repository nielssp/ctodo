ctodo
=====
A simple ncurses-based task list manager

License
-------
MIT

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
  
  Press `N` to create a new task.

* Managing the task list:

  Press `T` to edit the title of the task list.

  Press `SHIFT-T` to delete the current title
  and create a new one.

  Press S to save the list.

  Press R to reload the list (discard unsaved data).

  Press Q to save the list and quit.




