# ctodo
A simple ncurses-based task list manager

Website: http://ctodo.apakoh.dk

![Screenshot.](http://ctodo.apakoh.dk/screenshot.png)

## License
Copyright (C) 2012 Niels Sonnich Poulsen (http://nielssp.dk)

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

## Requirements
### Linux
* `ncurses`
* A C-compiler like `gcc`
* `cmake` (not actually necessary)

### Windows
* Windows XP or newer

## Installation
### Linux
* Run cmake:

        cmake .

* Compile:

         make

* Install:

         make install

### Windows
Run the installer, e.g. `ctodo-1.1-setup.exe`.

## Usage
### Linux
Run from a console using:

    ctodo

This will create a `todo.txt` file in the current
directory or open the file if it exists.

Alternatively you can run ctodo using:

    ctodo someotherfile.txt

Which will open/create `someotherfile.txt` in the current directory.

### Windows
Right click in any directory (or on the desktop) and open the `New`-menu. If ctodo was installed
using the installer, the option `Task List` should be available. Click on it.

This will create an empty task list which you can open with ctodo by double clicking on it
(if you installed using the installer).

##Commands
* Navigation:

  Use `UP` and `DOWN` arrows to move between tasks
  (or `J` and `K`).

  `PAGE UP` moves 5 tasks up and `PAGE DOWN` moves
  5 tasks down.

  `HOME` and `g` moves to the top of the list and `END` and `G` moves
  to the bottom.

* Managing tasks:

  `SPACE` and `ENTER` checks/unchecks the selected task.

  `D` and `DELETE` deletes the selected task.
  
  `SHIFT-UP` moves the selected task up and `SHIFT-DOWN` moves the selected task down.

  Press `E` to edit the description of the selected task.

  Press `SHIFT-E` to delete the current task description
  and create a new one.
  
  Press `N`, `SHIFT-A` or `INSERT` to create a new task at the bottom of the list.
  
  Press `SHIFT-I` to insert a new task at the top of the list.
  
  Press `I` to insert a new task before the selected task, and `A` to insert a new task after the selected task.

* In input-mode:
  
  Press `ENTER` to save the string.

  Press `CTRL-C` or `CTRL-D` to cancel.

* Managing the task list:

  Press `T` to edit the title of the task list.

  Press `SHIFT-T` to delete the current title and create a new one.

  Press `S` to save the list.

  Press `R` to reload the list (discard unsaved data).

  Press `Q` to save the list and quit.




