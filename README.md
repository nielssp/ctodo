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
Press the UP and DOWN arrows to move between tasks (or J and K).

Press PAGE UP and PAGE DOWN to move 5 tasks up and down.

Press HOME and END to move to the top of the list and the bottom
of the list.

Press SPACE or ENTER to check/uncheck the selected task.

Press D or DELETE to delete the selected task.

Press E to edit the selected task.

Press SHIFT-E to edit the selected task (delete current task-description).

Press T to edit the title of the task list.

Press SHIFT-E to edit the title of the task list (delete current title).

Press N to create a new task.

Press S to save the list.

Press Q to save the list and quit.

Press R to reload the list (discard unsaved data).



