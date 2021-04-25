#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <termios.h>
#include <fcntl.h>
#include <limits.h>
#define __USE_XOPEN_EXTENDED
#include <stdlib.h>

#include <errno.h>


pid_t
_forkpty(int *master, char *name, struct termios *tio, struct winsize *ws)
{
	int	slave = -1;
	char   *path;
	pid_t	pid;


	if ((*master = open("/dev/ptmx", O_RDWR|O_NOCTTY)) == -1)
		return (-1);
	if (grantpt(*master) != 0)
		goto out;
	if (unlockpt(*master) != 0)
		goto out;

	if ((path = ptsname(*master)) == NULL)
		goto out;

	if (name != NULL)
		strncpy(name, path, TTY_NAME_MAX);
	if ((slave = open(path, O_RDWR|O_NOCTTY)) == -1)
		goto out;

	switch (pid = fork()) {
	case -1:
		goto out;
	case 0:
		close(*master);

		setsid();


		if (tio != NULL && tcsetattr(slave, TCSAFLUSH, tio) == -1)
			fprintf( stderr, "tcsetattr failed");
		

		dup2(slave, 0);
		dup2(slave, 1);
		dup2(slave, 2);
		if (slave > 2)
			close(slave);
		return (0);
	}

	close(slave);
	return (pid);

out:
	if (*master != -1)
		close(*master);
	if (slave != -1)
		close(slave);
	return (-1);
}



int main(int argc, char *argv[]) {

	struct pollfd fds[2];
	char buffer[1024];
	int masterfd;
	ssize_t bytes_read;
	int i;
	ssize_t written;
	char shell[1024];
	

	if(argc != 2) {
		printf("Usage: %s <shell>\n", argv[0]);
		return -1;
	}

	strncpy(shell, argv[1], 1024);


	pid_t pid = _forkpty(&masterfd, NULL, NULL, NULL);

	if(pid < 0) {
		printf("Error forking pty\n");
		return -1;	
	} else if(pid == 0){ //Child process

		execlp(shell, shell, (char*)NULL);
	} else { //Parent process



		fds[0].fd = masterfd;
		fds[0].events = POLLIN;
		
		fds[1].fd = 0;
		fds[1].events = POLLIN;


		while(1) {

			if(poll(fds, 2, 100)>0){

				for(i=0;i<2;i++){
					if(fds[i].revents &POLLIN){
						if(fds[i].fd == masterfd) {
							bytes_read = read(masterfd, buffer, 1024);
							written = write(1, buffer, bytes_read);
						} if(fds[i].fd == 0) {
							bytes_read = read(0, buffer, 1024);
							written = write(masterfd, buffer, bytes_read);
						}
					}
				}
			}

		}
		

	}

   	return 0;
}
