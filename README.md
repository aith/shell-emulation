## Shell Emulation in Shell
### Usage
```
make
yshell
```
### Commands
```
# string
     If the first non-space character on a line is a hash, the
     line is a comment and is ignored.
cat pathname...
     The contents of each file is copied to the standard output.
     An error is reported if no files are specified, a file does
     not exist, or is a directory.
cd [pathname]
     The current directory is set the the pathname given.  If no
     pathname is specified, the root directory (/) is used.  
echo [words...]
     The string, which may be empty, is echoed to the standard
     output on a line by itself.
exit [status]
     Exit the program with the given status.  If the status is
     missing, exit with status 0.  If a non-numeric argument is
     given, exit with status 127.
ls [pathname...]
     For each file listed, output consists of the inode number,
     then the size, then the filename.
lsr [pathname...]
     As for ls, but a recursive depth-first preorder traversal is
     done for subdirectories.
make pathname [words...]
     The file specified is created and the rest of the words are
     put in that file.  If the file already exists, a new one is
     not created, but its contents are replaced.  
mkdir pathname
     A new directory is created, unless one with the given name
     exists.  
prompt string(s)
     Set the prompt to the words specified on the command line.
pwd
     Prints the current working directory.
rm pathname
     The specified file or directory is deleted.
rmr pathname
     A recursive removal is done, using a depth-first postorder
     traversal.
```
#### Assignment given by Wesley Mackey at UCSC, Advanced Programming