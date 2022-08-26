# c--

This repository contains an implementation of a compiler for a subset of the C Programming Language.

# How is a contribution made?

1. Create a branch with a meaningful name. Ex. implementing-lexer;
2. Clone the repository: 
    $ git clone  https://github.com/ArturAssisComp/c--.git
3. Change to the desired branch: 
    $ cd c--
    $ git checkout <branch-name>
4. For each meaningful change, make a commit;
    -> Fist, add the files that were changed: 
    $ git add <filename1> <filename2> ... <filenameN>
    -> Commit the changes: 
    $ git commit -m "Implemented function foo()"
5. Push the changes: 
    $ git push origin <name-of-the-branch>
6. If more changes are necessary, go to 4. Else,
   go to 7;
7. Create a pull request and assign someone to review the changes
   that were made in the branch;

=> Other useful commands:
    //Check the history of commits
    $ git log

    //Check the status of the repository
    $ git status

    //Get changes from remote repository from branch master
    $ git pull origin master

# How to use the makefile?

Run the following command in the terminal in the root directory of the c-- project:
    $ make
The compiled executables will be saved into the folder build.

To clean, run:
    $ make clean

# Requirements:

flex-devel 2.6.4 
flex       2.6.4
make       4.3
gcc        12.1.1
