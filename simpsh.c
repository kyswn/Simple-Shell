//NAME: Yining Wang
//EMAIL: wangyining@g.ucla.edu
//ID: 504983099


#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void getsegmenfault(){
	char* a=NULL;
	*a='a';
}

void signal_handler_fun(int num) {
   fprintf(stderr,"%d caught\n",num);
   exit(num);
}

int main(int argc, char** argv){
	
	
	//map fake descriptor to real descriptor, it is -1 if it doesn't correspond to any or its closed
	int * desc =malloc(sizeof(int)*100);
	for(int i=0;i<100;i++)
		desc[i]=-1;
	int filenum=0;

	int waitflag=0;

	int profileflag=0;
	int verboseflag=0;

	struct rusage usage; 	
	double startUserTime, endUserTime, userTime;
	double startSystemTime, endSystemTime, systemTime;

	int openfileflag=0;
	
	char** allcommandarg=malloc(sizeof(char*) * 100);
	int processcount=0;
	int * process=malloc(sizeof(int) * 100);  //record the process id 
	int maxsig=0;
	int maxexit=0;
	int exitnum=0;
	
	struct option options[]=
	{
		{"rdonly", 1, NULL, 12}, 
		{"wronly", 1, NULL, 13},
		{"command", 1, NULL, 14},
		{"verbose", 0, NULL, 15},
		
		
		{"append", 0, NULL, 1}, //O_APPEND
		{"cloexec", 0, NULL, 2}, //O_CLOEXEC
		{"creat", 0, NULL, 3}, //O_CREAT
		{"directory", 0, NULL, 4}, //O_DIRECTORY
		{"dsync", 0, NULL, 5}, //O_DSYNC
		{"excl", 0, NULL, 6}, //O_EXCL
		{"nofollow", 0, NULL, 7}, //O_NOFOLLOW
		{"nonblock", 0, NULL, 8}, //O_NONBLOCK
		{"rsync", 0, NULL, 9}, //O_RSYNC
		{"sync", 0, NULL, 10}, //O_SYNC
		{"trunc", 0, NULL, 11}, //O_TRUNC
		
		
		{"rdwr", 1, NULL, 16}, //O_RDWR. 
		{"pipe", 0, NULL, 17},
		{"wait", 0, NULL, 18},
		{"close", 1,NULL, 19},
		{"abort", 0, NULL, 20},
		{"catch", 1, NULL, 21},
		{"ignore", 1, NULL, 22},
		{"default", 1, NULL, 23},
		{"pause", 0, NULL, 24},
		{"profile", 0, NULL, 25},
		{0,0,0,0}
	};
	
	int ret=0;
	while((ret=getopt_long(argc, argv, "", options, NULL))!=-1){
		switch (ret){
		case 1:
		if (verboseflag) 
			printf("--append\n");
		openfileflag=openfileflag|O_APPEND;
		break;
		
		case 2:
		if (verboseflag)
			printf("--cloexec\n");
		openfileflag=openfileflag|O_CLOEXEC;
		break;
		
		case 3:
		if(verboseflag)
			printf("--creat\n");
		openfileflag=openfileflag|O_CREAT;
		break;
		
		case 4:
		if(verboseflag)
			printf("--directory \n");
		openfileflag=openfileflag|O_DIRECTORY;
		break;
		
		case 5:
		if(verboseflag)
			printf("--dsync\n");
		openfileflag=openfileflag|O_DSYNC;
		break;
		
		case 6:
		if(verboseflag)
			printf("--excl\n");
		openfileflag=openfileflag|O_EXCL;
		break;
		
		case 7:
		if(verboseflag)
			printf("--nofollow\n");
		openfileflag=openfileflag|O_NOFOLLOW;
		break;
		
		case 8:
		if(verboseflag)
			printf("--nonblock\n");
		openfileflag=openfileflag|O_NONBLOCK;
		break;

		case 9:
		if(verboseflag)
			printf("--rsync\n");
		openfileflag=openfileflag|O_RSYNC;
		break;
		
		case 10:
		if(verboseflag)
			printf("--sync\n");
		openfileflag=openfileflag|O_SYNC;
		break;
		
		case 11:
		if(verboseflag)
			printf("--trunc\n");
		openfileflag=openfileflag|O_TRUNC;
		break;
		
		
		case 12: //rdonly
		if(profileflag){
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
		}
		if(verboseflag==1){
			printf( "--rdonly ");
			printf("%s \n", optarg);
		}
		int rfid=open(optarg,  (O_RDONLY|openfileflag), 0666);
		openfileflag=0;
		//printf("%d \n" ,rfid);
		
		if(rfid==-1){
			fprintf(stderr, "error caused by --rdonly in opening the file %s , and the reason is %s \n", optarg, strerror(errno));
			exitnum=1;
		}
		else 
			desc[filenum]=rfid;
		filenum++;
		
		if (profileflag) {
				getrusage(RUSAGE_SELF, &usage);
				endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
				endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
				userTime = endUserTime - startUserTime;
				systemTime = endSystemTime - startSystemTime;
				printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
				}
		break;
		
		case 13: //wronly
		if(profileflag){
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
		}
		if(verboseflag==1){
			printf( "--wronly ");
			printf("%s \n",optarg);
		}
		int wfid=open(optarg,  (O_WRONLY|openfileflag), 0666 );
		openfileflag=0;
		
		if(wfid==-1){
			fprintf(stderr, "error caused by --wronly in opening the file %s , and the reason is %s \n", optarg, strerror(errno));
			exitnum=1;
		}
		else 
			desc[filenum]=wfid;
		filenum++;
		
		if (profileflag) {
					getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
				}
		break;
		
		case 16: //rdwr
		
		if(profileflag){
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
		}
		if(verboseflag==1){
			printf("--rdwr ");
			printf("%s \n", optarg);
		}
		
		int wrfid=open(optarg, (O_RDWR|openfileflag), 0666);
		openfileflag=0;
		
		if(wrfid==-1){
			fprintf(stderr, "error caused by --rdwr in opening the file %s , and the reason is %s \n", optarg, strerror(errno));
			exitnum=1;
		}
		else 
			desc[filenum]=wrfid;
		filenum++;
		if (profileflag) {
					getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
				}
		break;
		
		case 17: //pipe
		if(profileflag){
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
		}
		if(verboseflag)
			printf("--pipe\n");
		int pipefd[2];
		filenum+=2;
		if(pipe(pipefd)==-1){
			fprintf(stderr, "failure in using pipe(), and the reason is %s \n", strerror(errno));
			exitnum=1;
		}
		else{
			desc[filenum-2]=pipefd[0];
			desc[filenum-1]=pipefd[1];
		}
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			userTime = endUserTime - startUserTime;
			systemTime = endSystemTime - startSystemTime;
			printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
			}
		break;
		// when command, close things in child 
		
		case 18: //wait
		
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			}
		if(verboseflag){
			printf("--wait\n");
			fflush(stdout);	
		}
		int closedprocess=0;
		waitflag=1;
		while(closedprocess<processcount){
			int wstatus;
			int pid=waitpid(-1, &wstatus, 0);
			if(pid==-1){
				closedprocess++;
				continue;
			}
			if(exitnum==1){
				if(maxexit==0)
					maxexit=1;
			}
		
			int ex=0;
			int sig=0;
			
			if(WIFEXITED(wstatus)){
				ex=WEXITSTATUS(wstatus);
				printf("exit %d ", ex);
				if(ex>maxexit) maxexit=ex;
				//printf("exit %d ",ex);
			}
			else if ( WIFSIGNALED(wstatus)){
				sig=  WTERMSIG(wstatus);
				printf("signal %d ", sig);
				if(sig>maxsig) maxsig=sig;
				//printf("signal %d ",sig);
			}
			
			
			int index;
			for(int i=0;i<processcount;i++){
				if(process[i]==pid){
					index=i;
					break;
				}
			}
			
			printf("%s",allcommandarg[index]);
			printf("\n");
			closedprocess++;
		}
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			userTime = endUserTime - startUserTime;
			systemTime = endSystemTime - startSystemTime;
			printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
			
			getrusage(RUSAGE_CHILDREN, &usage);
			endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			printf("children's user time is  %fs and systme time is %fs \n", endUserTime, endSystemTime);
		
		}
		
		
		break;
		
		case 19: //close N
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			}
		if(verboseflag){
			printf("--close ");
			printf("%s\n", optarg);
		}
		close(desc[atoi(optarg)]);
		desc[atoi(optarg)]=-1;
		
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			userTime = endUserTime - startUserTime;
			systemTime = endSystemTime - startSystemTime;
			printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
			}
		break;
		
		
		
		
		
		
		case 20: //abort
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			}
		if(verboseflag){
			printf("--abort\n");
			fflush(stdout);
		}
		getsegmenfault();
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			userTime = endUserTime - startUserTime;
			systemTime = endSystemTime - startSystemTime;
			printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
			}
		break;
		
		case 21: //catch N
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			}
		if(verboseflag){
			printf("--catch ");
			printf("%s\n", optarg);
			}
		signal(atoi(optarg), signal_handler_fun);
		
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			userTime = endUserTime - startUserTime;
			systemTime = endSystemTime - startSystemTime;
			printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
			}
			
		
		break;
		
		case 22: //ignore N
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			}
		if(verboseflag){
			printf("--ignore ");
			printf("%s\n", optarg);
		}
		signal(atoi(optarg), SIG_IGN);
		
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			userTime = endUserTime - startUserTime;
			systemTime = endSystemTime - startSystemTime;
			printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
			}
			
		break;
		
		case 23: //default N
		
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			}
			
		if(verboseflag){
			printf("--default ");
			printf("%s\n", optarg);
		}
		signal(atoi(optarg), SIG_DFL);
		
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			userTime = endUserTime - startUserTime;
			systemTime = endSystemTime - startSystemTime;
			printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
			}
		break;
		
		case 24: //pause
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			}
		if(verboseflag)
			printf("--pause\n");
		pause();
		
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			userTime = endUserTime - startUserTime;
			systemTime = endSystemTime - startSystemTime;
			printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
			}
		break;
		
		
		
		
		
		case 14:;  //command
	
		if (profileflag) {
			getrusage(RUSAGE_SELF, &usage);
			startUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
			startSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
			}
		int begin=optind;
		int temp=optind;
		for( ;temp<argc;temp++){
			if(argv[temp][0]=='-'&&argv[temp][1]=='-'){
				temp=temp-1;
				break;
			}
		}
		if(temp==argc)
			temp--;
			
			
		
		int last=temp;
		
		int inp=atoi(argv[begin-1]);
		int o=atoi(argv[begin]);
		int e=atoi(argv[begin+1]);
		
		
		//check if the files are opened before or already closed
		int flag2=0;
		if(desc[inp]==-1 || desc[o]==-1 || desc[e]==-1){
			fprintf(stderr, "the argument files are not opened yet or already closed\n");
			exitnum=1;
			flag2=1;
		}
		if(flag2==1)
			break;
		int flag=0;
		if(inp>=filenum||o>=filenum||e>=filenum){
			
			fprintf(stderr, "the file descriptor does not exist");
		    flag=1;
			exitnum=1;
		}
		if(last-begin<2){
			fprintf(stderr, "missing arguments");
		
			flag=1;
			exitnum=1;
		}
			
			
		if(flag==1) break;
		char* cmd=argv[begin+2];
		int numsofargs=last-begin-2; //not including the command 
		optind=optind+numsofargs+3;
	
	
	




	char** argu=malloc(sizeof(char*) * (numsofargs+2));
		int i=0;
		argu[0]=cmd;
		for(i=1;i<=numsofargs;i++){
			argu[i]=argv[begin+2+i];		
		}
		argu[numsofargs+1]= (char*) NULL;
		
		//create a string to used by the argument --wait if it is called
		char * waitargu=malloc(sizeof(char) * 100);
		int charcount=0;
		
		
		//for(int i=0;i<numsofargs+2;i++){
			//printf("%s \n" ,argu[i]);
	//	}
		
		
		
		for (int i=0;i<numsofargs+1;i++){
			int j=0;
			while(argu[i][j]!='\0'){
				waitargu[charcount]=argu[i][j];
				charcount++;
				j++;
			}
			waitargu[charcount]=' ';
			charcount++;
		}
		waitargu[charcount-1]='\0';
		allcommandarg[processcount]=waitargu;
		


	
		if(verboseflag==1){
			printf("--command %d %d %d ", inp,o,e);
			for(i=0;i<numsofargs+1;i++){
				printf("%s ",argu[i]);
			}
			printf("\n");
		}
		//for(int i=0;i<numsofargs+5;i++){
			//printf("%s \n" ,argu[i]);
		//}
		
		int pid=fork();
		
		if(pid==-1){
			fprintf(stderr,"failure in creating a child process");
			exitnum=1;
		}
		
		if(pid==0){
			//child
			//printf("%d ,%d, %d \n", inp,o,e);
			dup2(desc[inp],0);
			dup2(desc[o],1);
			dup2(desc[e],2);
			
			for ( int i=0; i < filenum; i++) {
						close(i+3); 
					}
			
				//printf("%d %d %d \n", inp,o,e);
			//printf("haven;t done execvp yet \n");
			int exre=execvp(cmd,argu);
			//printf("i have done execvp \n");
			if(exre==-1) {
				fprintf(stderr, "failure when using execvp");
				exitnum=1;;
			}
		}
		else{   
		//parent process, do nothing
				process[processcount]=pid;
				processcount++;
				if (profileflag) {
					getrusage(RUSAGE_SELF, &usage);
					endUserTime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec * 0.000001);
					endSystemTime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec * 0.000001);
					userTime = endUserTime - startUserTime;
					systemTime = endSystemTime - startSystemTime;
					printf("user time is  %fs and system time is  %fs \n", userTime, systemTime);
				}
		}
		
		
		
			
		
		break;
		
		case 15:
		verboseflag=1;
		break;
		
		case 25:  //profile
		if(verboseflag)
			printf("--profile\n");
		
		profileflag=1;
		break;
		
		default:
		fprintf(stderr,"option not recognized");
		exitnum=1;
		
	
		}
	}
	if(waitflag==0)
		exit(exitnum);
	if(maxsig!=0){
		fflush(stdout);
		signal(maxsig, SIG_DFL);
		raise(maxsig);
	}
	
	exit(maxexit);
	

}