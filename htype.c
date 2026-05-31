#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#define DEFAULT_WPM 120
#define DEFAULT_ERROR_RATE 0.0
#define DEFAULT_JITTER 0.0
typedef struct{
	int wpm;
	double error_rate;
	double jitter;
	int debug_timing; //boolean flag to actually SEE the jitter
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
/* Box-Muller transformer. The logic is going crazy at this time so I am adding a todo below.*/
static double muller_rand(void){
	double u1=(rand()+1.0)/(RAND_MAX+1.0);
	double u2=(double)(rand())/RAND_MAX;
	return sqrt(-2.0*log(u1))*cos(2.0*M_PI*u2);
}
/* TODO: make a separate source file and header file for custom utility functions like this transformer.*/
static int parse_args(int argc, char *argv[], config_t *cfg){
	cfg->wpm=DEFAULT_WPM;
	cfg->error_rate=DEFAULT_ERROR_RATE;
	cfg->jitter=DEFAULT_JITTER;
	cfg->debug_timing=0;
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
		if (strcmp(argv[i], "--jitter") == 0) {
 			   if (i + 1 >= argc) {
        			fprintf(stderr, "%s: --jitter requires a value\n", argv[0]);
       				 return -1;
   		 }		
   		 char *end;
    		double val = strtod(argv[++i], &end);
    		if (*end != '\0' || val < 0.0 || val > 1.0) {
        		fprintf(stderr, "%s: --jitter must be a float between 0.0 and 1.0\n", argv[0]);
        		return -1;
   		 }		
   		 cfg->jitter = val;
   		 continue;
		}
		if(strcmp(argv[i],"--debug-timing")==0){
			cfg->debug_timing=1;
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
    /* TODO: This jittering is GOOD code. Add proper documentation.*/
    int c;

    while ((c = fgetc(stdin)) != EOF) {
    unsigned char ch = (unsigned char)c;

    long d = delay_us;
    if (cfg.jitter > 0.0) {
        double sigma = delay_us * cfg.jitter;
        d = (long)(delay_us + muller_rand() * sigma);
        long min = (long)(delay_us * (1.0 - cfg.jitter));
        long max = (long)(delay_us * (1.0 + cfg.jitter));
        if (d < min) d = min;
        if (d > max) d = max;
    }

    if (cfg.debug_timing)
        fprintf(stderr, "%c %ld\n", ch, d);

    if (cfg.error_rate > 0.0
            && (c >= 'a' && c <= 'z')
            && ((double)rand() / RAND_MAX) < cfg.error_rate) {
        char wrong = get_adjacent((char)c);
        write_char((unsigned char)wrong);
        sleep_us(d);
        write_char('\b');
        write_char(' ');
        write_char('\b');
        sleep_us(d);
    }

    write_char(ch);
    sleep_us(d);
}
    if (ferror(stdin)) { perror("humantype: read"); return 1; }
    return 0;
}
