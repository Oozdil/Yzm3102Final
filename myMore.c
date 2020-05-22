#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	//Uygulamanın çalışması için printdata pid ve ppid argumanları gerekiyor
	if (argc != 4)
	{
		printf("Bu dosya 3 arguman gerektirir!\n");
		return 0;
	}
	else
	{
		//argumanlar alınıyor
		char *printData = argv[1];
		char *_pid = argv[2];
		char *_ppid = argv[3];

		//prosesin pid ve ppid tespit ediliyor
		char pid[10];
		int pidInt = getpid();
		sprintf(pid, "%d", pidInt);
		char ppid[10];
		int ppidInt = getppid();
		sprintf(ppid, "%d", ppidInt);

		//arguman olarak gelen pid ve ppid ile prose pid ve ppid eşleşmesi gerekiyor
		//execv ile çağrılan bir uygulama, çağıran uygulamanın devamı olarak aynı pid ppid değerlerine sahiptir
		if (strcmp(pid, _pid) == 0 && strcmp(ppid, _ppid) == 0)
		{
			printf("%s\n", printData);
		}
		else
		{
			printf("Bu uygulama tek başına çalışamaz (%s   %s)\n", pid, ppid);
		}

	}

}
