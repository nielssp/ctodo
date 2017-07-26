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

## Commands
* Navigation:

  Use <kbd>UP</kbd> and <kbd>DOWN</kbd> arrows to move between tasks
  (or <kbd>J</kbd> and <kbd>K</kbd>).

  <kbd>PAGE UP</kbd> moves 5 tasks up and <kbd>PAGE DOWN</kbd> moves
  5 tasks down.

  <kbd>HOME</kbd> and <kbd>g</kbd> moves to the top of the list and
  <kbd>END</kbd> and <kbd>G</kbd> moves to the bottom.

* Managing tasks:

  <kbd>SPACE</kbd> and <kbd>ENTER</kbd> checks/unchecks the selected task.

  <kbd>D</kbd> and <kbd>DELETE</kbd> deletes the selected task.
  
  <kbd>SHIFT-UP</kbd> and <kbd>M</kbd> moves the selected task up and <kbd>SHIFT-DOWN</kbd> and <kbd>SHIFT-M</kbd>
  moves the selected task down.

  Press <kbd>E</kbd> to edit the description of the selected task.

  Press <kbd>SHIFT-E</kbd> to delete the current task description
  and create a new one.
  
  Press <kbd>N</kbd>, <kbd>SHIFT-A</kbd> or <kbd>INSERT</kbd> to create a new
  task at the bottom of the list.
  
  Press <kbd>SHIFT-I</kbd> to insert a new task at the top of the list.
  
  Press <kbd>I</kbd> to insert a new task before the selected task, and
  <kbd>A</kbd> to insert a new task after the selected task.

* In input-mode:
  
  Press <kbd>ENTER</kbd> to save the string.

  Press <kbd>CTRL-C</kbd> or <kbd>CTRL-D</kbd> to cancel.

* Managing the task list:

  Press <kbd>T</kbd> to edit the title of the task list.

  Press <kbd>SHIFT-T</kbd> to delete the current title and create a new one.

  Press <kbd>S</kbd> to save the list.

  Press <kbd>R</kbd> to reload the list (discard unsaved data).

  Press <kbd>Q</kbd> to save the list and quit.
