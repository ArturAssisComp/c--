# c--

This repository contains an implementation of a compiler for a subset of the C Programming Language.

# How is a contribution made?

1. Create a branch with a meaningful name. Ex. implementing-lexer;
2. Clone the repository: 
```bash
git clone  https://github.com/ArturAssisComp/c--.git
```
3. Change to the desired branch: 
```bash
cd c--
git checkout <branch-name>
```
4. For each meaningful change, make a commit;
    -> Fist, add the files that were changed: 
```bash
git add <filename1> <filename2> ... <filenameN>
```
    -> Commit the changes: 
```bash
git commit -m "Implemented function foo()"
```
5. Push the changes: 
```bash
git push origin <name-of-the-branch>
```
6. If more changes are necessary, go to 4. Else,
   go to 7;
7. Create a pull request and assign someone to review the changes
   that were made in the branch;

=> Other useful commands:

Check the history of commits
```bash
git log
```
Check the status of the repository
```bash
git status
```
Get changes from remote repository from branch master
```bash
git pull origin master
```
# How to use the makefile?

Run the following command in the terminal in the root directory of the c-- project:
```bash
make
```  
The compiled executable (named cmm) will be saved into the folder build.

Usage: cmm \<filename\>

or:    cmm

Executing cmm without argument makes stdin to be the input.

To clean, run:
```bash
make clean
```

In order to run all examples:
```bash
make run_examples
```

In order to run a specific example file (the default value for EXAMPLE_FILENAME is debug.c):
```bash
make run_example_file EXAMPLE_FILENAME=<filename>
```


## Testing

Before testing, install the necessary files:
```bash
make install
```  
After installation, compile the test files and run them:
```bash
make test
./build/test/*
```  





Before testing 
# Requirements:

flex-devel 2.6.4 

flex       2.6.4

make       4.3

gcc        12.1.1
