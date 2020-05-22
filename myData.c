#include <stdio.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

/* Bu uygulama kendisine gönderilen argumanda yolu belirtilen dosyası
** satır satır okur
** Eğer '=myMore' uzantısı belirtilmediyse tüm datayı ekrana bastırır
** Eğer '=myMore' uzantısı belirtildiyse datayı 24'er satır olarak pipe aracılığı ile
** myMore uygulamasına gönderir ve ekrana bastırır
*/



#define MSGSIZE 10000 //Mesaj boyutu tanımlıyoruz


int main(int argc, char *argv[])
{

	char inbuf[MSGSIZE];
	int p[2], pid, nbytes;

	//Pipe oluştur, eğer oluşturmazsa hata döndür
	if (pipe(p) < 0)
		return 1;

	char strUserInput[20];//Devam veya kesme işlemi için kullanıcıdan istenecek


	/* fork() işlemi yapıyoruz
	** Parent, 24 lü bloklar halinde verileri pipe'a yazacak
	** Cild, pipe'dan gelen verileri okuyacak ve myMore'u execvile çalıştıracak
	*/


	//Parent form işlemleri	
	if ((pid = fork()) > 0)//fork başarılı
	{

		if (argc != 2)
		{
			// Dosya adı arguman olarak gönderilmedi, hata döndürür
			printf("Bu uygulama dosya adini arguman olarak gerektiriyor\n");
			return 0;
		}


		else
		{
			// argumanı parse ediyoruz
			char *arguman;
			arguman = argv[1];
			char *fileName;
			char *procName = "";
			char * token = strtok(arguman, "=");
			fileName = token;
			while (token != NULL)
			{
				token = strtok(NULL, "=");
				if (token != NULL)
					procName = token;
			}


			//Dosyası açmaya çalışıyoruz
			FILE *file = fopen(fileName, "r");
			if (file == NULL)
			{
				//Dosya yolu hatalı
				printf("%s, Dosya yolu hatalı \n", fileName);
				return 0;
			}

			if (strcmp(procName, "myMore") != 0 && strcmp(procName, "") != 0)
			{
				// = işsatinden sonra gelen komut hatalı
				printf("Dosya adinda sonra '=myMore' ekleyiniz...\n'");
				return 0;
			}



			//Pagination ile listeleme yapıyoruz '=myMore'
			if (strcmp(procName, "myMore") == 0)
			{
				//satır syısını tespit ediyoruz
				FILE *file2 = fopen(fileName, "r");
				char line2[128];
				int co = 0;//satır sayısı
				while (fgets(line2, sizeof line2, file2))
				{
					co++;
				}
				fclose(file2);


				char line[128]; /* satır uzunluğu değiştrilebilir*/
				char data[100000];
				int counter = 0;

				while (fgets(line, sizeof line, file) != NULL && strcmp(strUserInput, "q") != 0) /* satırları okuyoruz*/
				{
					strcat(data, line);//satırları data değişkeninde topluyoruz
					counter++;
					strcpy(strUserInput, "");
					/*
					** 24 satır olduysa veya dosyanın sonuna gelindiyse
					** data pipe'a yazılır
					*/
					if (counter % 24 == 0 || counter == co)
					{
						write(p[1], data, MSGSIZE);//pipe'a ayzma işlemi
						strcpy(data, "");
						sleep(1);
						//Dosya sonuna gelinmediyse, kullanıcıdan devam için değer istenir
						if (counter < co){
							// q değeri girilmesi ile while döngüsünden çıkılır
							// space değeri ile devam edilir
							while (strcmp(strUserInput, "q") != 0 && strcmp(strUserInput, " ") != 0)
							{
								printf("More ? (cikmak icin 'q', devam icin Bosluk-Space giriniz): ");
								gets(strUserInput);
							}
						}
					}
				}
				fclose(file);
			}

			//Tüm datayı listeliyoruz
			else
			{
				printf("*************************************************************\n");
				printf("*****************Tumu satirlar listelenecek******************\n");
				char line[128]; /*satır uzunluğu değiştirelibilir*/
				int counter = 0;//Satır sayısı

				while (fgets(line, sizeof line, file) != NULL) /*satır satır okuyoruz */
				{
					fputs(line, stdout); /*satırı ekrana bastırıyoruz*/
					counter++;
				}
				fclose(file);
				printf("**************%d adet satir listelendi***********************\n", counter);
				printf("*************************************************************\n");
			}
		}
	}


	//Child işlemleri
	else
	{
		// Pipe'yazmayı kapatıyoruz		 
		close(p[1]);

		while ((nbytes = read(p[0], inbuf, MSGSIZE)) > 0)//Pipe'dan okuduğu sürece çalışır
		{
			//fork işlemi yapıyoruz
			if (fork() == 0)
			{
				char pid[10];
				int pidInt = getpid();
				sprintf(pid, "%d", pidInt);
				char ppid[10];
				int ppidInt = getppid();
				sprintf(ppid, "%d", ppidInt);

				//myMore argumanları oluşturuluyor
				char *args[] = { "./myMore", inbuf, pid, ppid, NULL };
				//myMore execv ile çalıştırılıyor
				execvp(args[0], args);
			}
			wait(NULL);
		}

		if (nbytes != 0)
			return 0;

	}


	return 0;
}














