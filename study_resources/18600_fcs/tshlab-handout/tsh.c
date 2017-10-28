/*
 * tsh - A tiny shell program with job control
 *
 * Name: Kshitij Khode
 * Andrew ID: kkhode
 * Note: Non-solution based online references were referred in order to grasp the
 * structure of the lab
 *
 * This version of tiny shell supports bg/fg execution, signal handling and I/O
 * redirection.
 *
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "csapp.h"

/* Misc manifest constants */
#define MAXLINE_TSH    1024   /* max line size */
#define MAXARGS         128   /* max args on a command line */
#define MAXJOBS         16    /* max jobs at any point in time */
#define MAXJID          1<<16 /* max job ID */

/* Job states */
#define UNDEF         0   /* undefined */
#define FG            1   /* running in foreground */
#define BG            2   /* running in background */
#define ST            3   /* stopped */

/*
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Parsing states */
#define ST_NORMAL   0x0   /* next token is an argument */
#define ST_INFILE   0x1   /* next token is the input file */
#define ST_OUTFILE  0x2   /* next token is the output file */

/* Global variables */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE_TSH];  /* command line */
};

struct job_t job_list[MAXJOBS]; /* The job list */

struct cmdline_tokens {
    int argc;               /* Number of arguments */
    char *argv[MAXARGS];    /* The arguments list */
    char *infile;           /* The input file */
    char *outfile;          /* The output file */
    enum builtins_t {       /* Indicates if argv[0] is a builtin command */
        BUILTIN_NONE,
        BUILTIN_QUIT,
        BUILTIN_JOBS,
        BUILTIN_BG,
        BUILTIN_FG} builtins;
};

extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose   = 0;          /* if true, print additional output */
int nextjid   = 1;          /* next job ID to allocate */
char sbuf[MAXLINE_TSH];     /* for composing sprintf messages */

/* End global variables */

/* Function prototypes */
void eval(char *cmdline);

/* Custom major functions */
int exec_builtin_command(struct cmdline_tokens tok);

/* Custom signal handlers */
void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, struct cmdline_tokens *tok);
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *job_list);
int maxjid(struct job_t *job_list);
int addjob(struct job_t *job_list, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *job_list, pid_t pid);
pid_t fgpid(struct job_t *job_list);
struct job_t *getjobpid(struct job_t *job_list, pid_t pid);
struct job_t *getjobjid(struct job_t *job_list, int jid);
int pid2jid(pid_t pid);
void listjobs(struct job_t *job_list, int output_fd);

void usage(void);


/*
 * main - The shell's main routine
 */
int
main(int argc, char **argv)
{
    char c;
    char cmdline[MAXLINE_TSH];    /* cmdline for fgets */
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
            break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
            break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
            break;
        default:
            usage();
        }
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
    Signal(SIGTTIN, SIG_IGN);
    Signal(SIGTTOU, SIG_IGN);

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler);

    /* Initialize the job list */
    initjobs(job_list);

    /* Execute the shell's read/eval loop */
    while (1) {

        if (emit_prompt) {
            printf("%s", prompt);
            fflush(stdout);
        }
        if ((fgets(cmdline, MAXLINE_TSH, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");
        if (feof(stdin)) {
            /* End of file (ctrl-d) */
            printf ("\n");
            fflush(stdout);
            fflush(stderr);
            exit(0);
        }

        /* Remove the trailing newline */
        cmdline[strlen(cmdline)-1] = '\0';

        /* Evaluate the command line */
        eval(cmdline);

        fflush(stdout);
        fflush(stdout);
    }

    exit(0); /* control never reaches here */
}

/*
 * eval - Evaluate the command line that the user has just typed in
 *
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.
 *
 * The function attempts to execute commands if they're builtin commands
 * such as jobs, fg etc. After that it preps up mask for signals dealing with
 * termination of child processes i.e. shell commands. I/O redirection is
 * achieved by having different child processes instantiated with i/o file
 * buffers contained in the tok struct.
 *
 */
void
eval(char *cmdline)
{
    int bg, pid, redir_in, redir_out;   /* should the job run in bg or fg? */
    struct cmdline_tokens tok;
    sigset_t mask_0;

    /* Parse command line */
    bg = parseline(cmdline, &tok);

    if (bg == -1) /* parsing error */
        return;
    if (tok.argv[0] == NULL) /* ignore empty lines */
        return;

    /* Attempt to execute arv parsed struct cmdline commands, if they are built in */
    if (!exec_builtin_command(tok)) {

        /* Setup mask for various child processes i.e. commands in case they are
        terminated through signal or naturally (ctrl-c or end), stopped (ctrl-z).
        Block signal until child processes are created to avoid race */
        Sigemptyset(&mask_0);
        Sigaddset(&mask_0, SIGCHLD);
        Sigaddset(&mask_0, SIGINT);
        Sigaddset(&mask_0, SIGTSTP);
        Sigprocmask(SIG_BLOCK, &mask_0, NULL);

        /* Fork child and run setup struct cmdline buffers, if < or > detected in commands
        for I/O redirection. Can't simply use posix defined open() functions for I/O, since
        later traces redirect to /dev/null and utilize < and > together, and simultaneously
        utilize struct cmdline  requiring error checking and utilization of c defined
        fopen() */
        if ((pid = fork()) == 0) {
            FILE *infile = NULL;
            FILE *outfile = NULL;
            if (tok.infile != NULL) {
                if ((redir_in = open(tok.infile, O_RDWR)) < 0) {
                    infile = fopen(tok.infile, "r");
                    redir_in = open(tok.infile, O_RDWR);
                }
                Dup2(redir_in, STDIN_FILENO);
                if (infile != NULL) fclose(infile);
                close(redir_in);
            }
            if (tok.outfile != NULL) {
                if ((redir_out = open(tok.outfile, O_RDWR)) < 0) {
                    outfile = fopen(tok.outfile, "w");
                    redir_out = open(tok.outfile, O_RDWR);
                }
                Dup2(redir_out, STDOUT_FILENO);
                if (outfile != NULL) fclose(outfile);
                close(redir_out);
            }

            /* Group piped or redirected child process command, so that they can be terminated
            together if required, and execute non builtin command */
            Sigprocmask(SIG_UNBLOCK, &mask_0, NULL);
            Setpgid(0, 0);
            if (execve(tok.argv[0], tok.argv, environ) < 0) {
                printf("%s: Command not found\n", tok.argv[0]);
                exit(0);
            }
        }

        /* If non builtin command then fg and bg utilization possible. Add to job list as a
        result. Allow unblocking to handle signals in child processes. */
        addjob(job_list, pid, bg+1, cmdline);
        Sigprocmask(SIG_UNBLOCK, &mask_0, NULL);

        /* Imitate tshref when bg'ing a job. If fg, use Sigsuspend to block parent until child
        returns. Save signals address'ls from the time command executed and restore, when
        finished. */
        struct job_t *job = getjobpid(job_list, pid);
        if (bg) {
            printf("[%d] (%d) %s", job->jid, pid, cmdline);
        }
        else {
            sigset_t mask_1, prev;
            Sigaddset(&mask_1, SIGCHLD);
            Sigprocmask(SIG_BLOCK, &mask_1, &prev);
            while (fgpid(job_list) != 0 && getjobpid(job_list, pid)->state != ST) {
                Sigemptyset(&mask_1);
                Sigsuspend(&mask_1);
            }
            Sigprocmask(SIG_SETMASK, &prev, NULL);
        }
    }
    return;
}

/*
 * exec_builtin_command - Execute commands if they're builtin commands
 *
 * Checks if builtin commands, based on helper function parseline generated
 * struct cmdline from arv contents. Builtin commands are bg, fg, quit and jobs.
 *
 * Parameters:
 *     tok: cmdline struct parsed from parseline, contains info about if command
 *     provided is builtin and if > or < provided for I/O redirection. Contains buf
 *     if < or > provided.
 *
 *  Returns:
 *     0, if command is not builtin command.
 *     1, if command is builtin and execution completed successfully.
 *    -1, if command is erroneously parsed as builtin or execution completed
 *        unsucessfully.
 *
 * Note: Only jobs command is tested for I/O redirection in last traces, so only added
 * I/O redirection there. I had tried I/O redirection simply exec_builtin_commands
 * but ran into segmentation fault. Due to lack of time, restricted the implementation
 * to bare minimum.
 *
 */

int exec_builtin_command(struct cmdline_tokens tok)
{
    struct job_t *job;
    sigset_t mask, prev;

    switch(tok.builtins) {
        /* Command is not builtin. so return to eval and allow child process for external
        commands. */
        case BUILTIN_NONE: return 0;
        case BUILTIN_QUIT: exit(0);

        /* Allow I/O redirection for jobs, since it is requested in last few traces, else
        simply dump to stdout. */
        case BUILTIN_JOBS:
            if (tok.outfile != NULL) {
                int redir_out = open(tok.outfile, O_RDWR);
                listjobs(job_list, redir_out);
                close(redir_out);
            }
            else listjobs(job_list, STDOUT_FILENO);
            return 1;

        /* if bg 1% kind of command, then bg using job id, else use pid. Restart child process
        sending cigcont and set job state to BG. */
        case BUILTIN_BG:
            if (tok.argv[tok.argc-1][0] == '%') {
                job = getjobjid(job_list, atoi(tok.argv[tok.argc-1]+1));
                if (job == NULL) {
                    printf("%s: No such job\n", tok.argv[1]);
                    return 1;
                }
            }
            else {
                job = getjobpid(job_list, atoi(tok.argv[tok.argc-1]));
                if (job == NULL) {
                   printf("(%s): No such process\n", tok.argv[1]);
                   return 1;
                }
            }
            Kill(job->pid, SIGCONT);
            job->state = BG;
            printf("[%d] (%d) %s\n", job->jid, job->pid, job->cmdline);
            return 1;

        /* Command "regex" handling same as bg, but pause parent process using Sigsuspend
        until child returns. Save signals address'ls from the time command executed
        and restore, when finished. */
        case BUILTIN_FG:
            if (tok.argv[tok.argc-1][0] == '%') {
                job = getjobjid(job_list, atoi(tok.argv[tok.argc-1]+1));
                if (job == NULL) {
                    printf("%s: No such job\n", tok.argv[1]);
                    return 1;
                }
            }
            else {
                job = getjobpid(job_list, atoi(tok.argv[tok.argc - 1]));
                if (job == NULL) {
                    printf("%s: No such process\n", tok.argv[1]);
                    return 1;
                }
            }
            Sigemptyset(&mask);
            Sigaddset(&mask, SIGCHLD);
            Sigprocmask(SIG_BLOCK, &mask, &prev);
            while (fgpid(job_list) != 0 && getjobpid(job_list, job->pid)->state != ST) {
                Sigsuspend(&mask);
            }
            Sigprocmask(SIG_SETMASK, &prev, NULL);
            Kill(job->pid, SIGCONT);
            job->state = FG;
            return 1;
        default: return -1;
    }
    return -1;
}

/*
 * parseline - Parse the command line and build the argv array.
 *
 * Parameters:
 *   cmdline:  The command line, in the form:
 *
 *                command [arguments...] [< infile] [> oufile] [&]
 *
 *   tok:      Pointer to a cmdline_tokens structure. The elements of this
 *             structure will be populated with the parsed tokens. Characters
 *             enclosed in single or double quotes are treated as a single
 *             argument.
 * Returns:
 *   1:        if the user has requested a BG job
 *   0:        if the user has requested a FG job
 *  -1:        if cmdline is incorrectly formatted
 *
 * Note:       The string elements of tok (e.g., argv[], infile, outfile)
 *             are statically allocated inside parseline() and will be
 *             overwritten the next time this function is invoked.
 */
int
parseline(const char *cmdline, struct cmdline_tokens *tok)
{

    static char array[MAXLINE_TSH];          /* holds local copy of command line */
    const char delims[10] = " \t\r\n";   /* argument delimiters (white-space) */
    char *buf = array;                   /* ptr that traverses command line */
    char *next;                          /* ptr to the end of the current arg */
    char *endbuf;                        /* ptr to end of cmdline string */
    int is_bg;                           /* background job? */

    int parsing_state;                   /* indicates if the next token is the
                                            input or output file */

    if (cmdline == NULL) {
        (void) fprintf(stderr, "Error: command line is NULL\n");
        return -1;
    }

    (void) strncpy(buf, cmdline, MAXLINE_TSH);
    endbuf = buf + strlen(buf);

    tok->infile = NULL;
    tok->outfile = NULL;

    /* Build the argv list */
    parsing_state = ST_NORMAL;
    tok->argc = 0;

    while (buf < endbuf) {
        /* Skip the white-spaces */
        buf += strspn (buf, delims);
        if (buf >= endbuf) break;

        /* Check for I/O redirection specifiers */
        if (*buf == '<') {
            if (tok->infile) {
                (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
                return -1;
            }
            parsing_state |= ST_INFILE;
            buf++;
            continue;
        }
        if (*buf == '>') {
            if (tok->outfile) {
                (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
                return -1;
            }
            parsing_state |= ST_OUTFILE;
            buf ++;
            continue;
        }

        if (*buf == '\'' || *buf == '\"') {
            /* Detect quoted tokens */
            buf++;
            next = strchr (buf, *(buf-1));
        } else {
            /* Find next delimiter */
            next = buf + strcspn (buf, delims);
        }

        if (next == NULL) {
            /* Returned by strchr(); this means that the closing
               quote was not found. */
            (void) fprintf (stderr, "Error: unmatched %c.\n", *(buf-1));
            return -1;
        }

        /* Terminate the token */
        *next = '\0';

        /* Record the token as either the next argument or the i/o file */
        switch (parsing_state) {
        case ST_NORMAL:
            tok->argv[tok->argc++] = buf;
            break;
        case ST_INFILE:
            tok->infile = buf;
            break;
        case ST_OUTFILE:
            tok->outfile = buf;
            break;
        default:
            (void) fprintf(stderr, "Error: Ambiguous I/O redirection\n");
            return -1;
        }
        parsing_state = ST_NORMAL;

        /* Check if argv is full */
        if (tok->argc >= MAXARGS-1) break;

        buf = next + 1;
    }

    if (parsing_state != ST_NORMAL) {
        (void) fprintf(stderr,
                       "Error: must provide file name for redirection\n");
        return -1;
    }

    /* The argument list must end with a NULL pointer */
    tok->argv[tok->argc] = NULL;

    if (tok->argc == 0)  /* ignore blank line */
        return 1;

    if (!strcmp(tok->argv[0], "quit")) {                 /* quit command */
        tok->builtins = BUILTIN_QUIT;
    } else if (!strcmp(tok->argv[0], "jobs")) {          /* jobs command */
        tok->builtins = BUILTIN_JOBS;
    } else if (!strcmp(tok->argv[0], "bg")) {            /* bg command */
        tok->builtins = BUILTIN_BG;
    } else if (!strcmp(tok->argv[0], "fg")) {            /* fg command */
        tok->builtins = BUILTIN_FG;
    } else {
        tok->builtins = BUILTIN_NONE;
    }

    /* Should the job run in the background? */
    if ((is_bg = (*tok->argv[tok->argc-1] == '&')) != 0)
        tok->argv[--tok->argc] = NULL;

    return is_bg;
}


/*****************
 * Signal handlers
 *****************/

/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP, SIGTSTP, SIGTTIN or SIGTTOU signal. The
 *     handler reaps all available zombie children, but doesn't wait
 *     for any other currently running children to terminate.
 */
void
sigchld_handler(int sig)
{
    int status, pid;
    while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        if (WIFSTOPPED(status)) {
            printf("Job [%d] (%d) stopped by signal %d\n", pid2jid(pid), pid, WSTOPSIG(status));
            getjobpid(job_list, pid)->state = ST;
        }
        else if (WIFSIGNALED(status)) {
            printf("Job [%d] (%d) terminated by signal %d\n", pid2jid(pid), pid, WTERMSIG(status));
            deletejob(job_list, pid);
            }
        else if (WIFEXITED(status)) {
            deletejob(job_list, pid);
        }
    }
    return;
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.
 */
void
sigint_handler(int sig)
{
    pid_t pid = fgpid(job_list);
    if (pid != 0) {
       Kill(-pid, sig);
    }
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
void
sigtstp_handler(int sig)
{
    pid_t pid = fgpid(job_list);
    if (pid != 0) {
        Kill(-pid, sig);
    }
    return;
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void
sigquit_handler(int sig)
{
    sio_error("Terminating after receipt of SIGQUIT signal\n");
}



/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void
clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void
initjobs(struct job_t *job_list) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
        clearjob(&job_list[i]);
}

/* maxjid - Returns largest allocated job ID */
int
maxjid(struct job_t *job_list)
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].jid > max)
            max = job_list[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int
addjob(struct job_t *job_list, pid_t pid, int state, char *cmdline)
{
    int i;

    if (pid < 1)
        return 0;

    for (i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == 0) {
            job_list[i].pid = pid;
            job_list[i].state = state;
            job_list[i].jid = nextjid++;
            if (nextjid > MAXJOBS)
                nextjid = 1;
            strcpy(job_list[i].cmdline, cmdline);
            if(verbose){
                printf("Added job [%d] %d %s\n",
                       job_list[i].jid,
                       job_list[i].pid,
                       job_list[i].cmdline);
            }
            return 1;
        }
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int
deletejob(struct job_t *job_list, pid_t pid)
{
    int i;

    if (pid < 1)
        return 0;

    for (i = 0; i < MAXJOBS; i++) {
        if (job_list[i].pid == pid) {
            clearjob(&job_list[i]);
            nextjid = maxjid(job_list)+1;
            return 1;
        }
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t
fgpid(struct job_t *job_list) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].state == FG)
            return job_list[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t
*getjobpid(struct job_t *job_list, pid_t pid) {
    int i;

    if (pid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].pid == pid)
            return &job_list[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *job_list, int jid)
{
    int i;

    if (jid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].jid == jid)
            return &job_list[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int
pid2jid(pid_t pid)
{
    int i;

    if (pid < 1)
        return 0;
    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].pid == pid) {
            return job_list[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */
void
listjobs(struct job_t *job_list, int output_fd)
{
    int i;
    char buf[MAXLINE_TSH];

    for (i = 0; i < MAXJOBS; i++) {
        memset(buf, '\0', MAXLINE_TSH);
        if (job_list[i].pid != 0) {
            sprintf(buf, "[%d] (%d) ", job_list[i].jid, job_list[i].pid);
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
            memset(buf, '\0', MAXLINE_TSH);
            switch (job_list[i].state) {
            case BG:
                sprintf(buf, "Running    ");
                break;
            case FG:
                sprintf(buf, "Foreground ");
                break;
            case ST:
                sprintf(buf, "Stopped    ");
                break;
            default:
                sprintf(buf, "listjobs: Internal error: job[%d].state=%d ",
                        i, job_list[i].state);
            }
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
            memset(buf, '\0', MAXLINE_TSH);
            sprintf(buf, "%s\n", job_list[i].cmdline);
            if(write(output_fd, buf, strlen(buf)) < 0) {
                fprintf(stderr, "Error writing to output file\n");
                exit(1);
            }
        }
    }
}
/******************************
 * end job list helper routines
 ******************************/

/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void
usage(void)
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}
