#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#define DEFAULT_WPM 120
#define DEFAULT_ERROR_RATE 0.0
typedef struct{
	int wpm;
	double error_rate;
} config_t;
static const char *adj[128] = {
    ['q'] = "was",  ['w'] = "qase", ['e'] = "wsdr", ['r'] = "edft",
    ['t'] = "rfgy", ['y'] = "tghu", ['u'] = "yhji", ['i'] = "ujko",
    ['o'] = "iklp", ['p'] = "ol",
    ['a'] = "qwsz", ['s'] = "awedxz", ['d'] = "serfcx", ['f'] = "drtgvc",
    ['g'] = "ftyhbv", ['h'] = "gyujnb", ['j'] = "huikmn", ['k'] = "jiolm",
    ['l'] = "kop",
    ['z'] = "asx",  ['x'] = "zsdc",  ['c'] = "xdfv",  ['v'] = "cfgb",
    ['b'] = "vghn", ['n'] = "bhjm",  ['m'] = "njk",
};
static void usage(const char *prog){
	fprintf(stderr,"Usage: %s [--wpm N]\n",prog);
}
static int parse_args(int argc, char *argv[], config_t *cfg){
	cfg->wpm=DEFAULT_WPM;
	cfg->error_rate=DEFAULT_ERROR_RATE;
	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],"--help")==0){
			usage(argv[0]);
			exit(0);
		}
		if(strcmp(argv[i],"--wpm")==0){
			if(i+1>=argc){
				fprintf(stderr,"%s: --wpm requires a value\n",argv[0]);
				return -1;
			}
			char *end;
			long val=strtol(argv[++i],&end,10);
			if(*end!='\0'||val<1){
				fprintf(stderr,"%s: --wpm must be a positive integer\n",argv[0]);
				return -1;
			}
			cfg->wpm=(int)val;
			continue;
		}
		if(strcmp(argv[i],"--error")==0){
			if(i+1>=argc){
				fprintf(stderr,"%s: --error requires a value\n",argv[0]);
				return -1;
			}
			char *end;
			double val=strtod(argv[++i],&end);
			if(*end!='\0'||val<0.0||val>1.0){
				fprintf(stderr,"%s: --error must be a positive value\n",argv[0]);
				return -1;
			}
			cfg->error_rate=val;
			continue;
		}
		fprintf(stderr,"%s: unknown option '%s'\n",argv[0],argv[i]);
		return -1;
	}
	return 0;
}
static void sleep_us(long us){
	struct timespec req={ us/1000000L, (us%1000000L)*1000L};
	while(clock_nanosleep(CLOCK_MONOTONIC,0,&req,&req)==EINTR);
}
static char get_adjacent(char c){
	const char *neighbours =adj[(unsigned char)c];
	if(!neighbours||neighbours[0]=='\0')
		return c;
	return neighbours[rand()%strlen(neighbours)];
}
static void write_char(unsigned char c){
	ssize_t w;
	do{
		w=write(STDOUT_FILENO,&c,1);
	}
	while(w==-1&&errno==EINTR);
}
int main(int argc, char *argv[])
{
    config_t cfg;
    if (parse_args(argc, argv, &cfg) != 0)
        return 1;

    srand((unsigned)time(NULL));

    long delay_us = 60000000L / ((long)cfg.wpm * 5);
    int c;

    while ((c = fgetc(stdin)) != EOF) {
        unsigned char ch = (unsigned char)c;

        if (cfg.error_rate > 0.0
                && (c >= 'a' && c <= 'z')
                && ((double)rand() / RAND_MAX) < cfg.error_rate) {

            char wrong = get_adjacent((char)c);
            write_char((unsigned char)wrong);  /* mistyped char  */
            sleep_us(delay_us);
            write_char('\b');                   /* backspace      */
            write_char(' ');                    /* erase          */
            write_char('\b');                   /* reposition     */
            sleep_us(delay_us);
        }

        write_char(ch);                         /* correct char   */
        sleep_us(delay_us);
    }

    if (ferror(stdin)) { perror("humantype: read"); return 1; }
    return 0;
}
