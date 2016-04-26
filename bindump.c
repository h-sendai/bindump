#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

int usage(void)
{
    char *msg = "Usage: bindump [-l] [-C] [-n one_read_bytes] [filename]\n"
                "       -l: print data length at the end of lines\n"
                "       -C: print printable characters\n"
                "       -n: specify one read byte size (4 bytes default)\n";
    fprintf(stderr, "%s", msg);

    return 0;
} 

int is_printable(unsigned char s)
{
    if (s >= ' ' && s <= '~') {
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int n_read_bytes = 4;
    FILE *fp;
    char *filename;
    unsigned char *buf;
    int line_num = 0;
    int print_so_far_data_len = 0;
    int print_printable_chars = 0;
    int so_far_data_len = 0;
    int c;

    while ( (c = getopt(argc, argv, "Cln:")) != -1) {
        switch (c) {
            case 'l':
                print_so_far_data_len = 1;
                break;
            case 'n':
                n_read_bytes = strtol(optarg, NULL, 0);
                break;
            case 'C':
                print_printable_chars = 1;
                break;
            default:
                break;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc != 1) {
        usage();
        exit(EXIT_FAILURE);
    }

    filename = argv[0];

    fp = fopen(filename, "r");
    if (fp == NULL) {
        err(EXIT_FAILURE, "%s", filename);
    }

    buf = malloc(n_read_bytes);
    if (buf == NULL) {
        err(EXIT_FAILURE, "malloc");
    }

    for ( ; ; ) {
        int n = fread(buf, 1, n_read_bytes, fp);
        so_far_data_len += n;
        if (n == 0) {
            if (ferror(fp)) {
                err(EXIT_FAILURE, "fread");
            }
            if (feof(fp)) {
                break;
            }
        }
        printf("%08d 0x%08x ", line_num, line_num);
        line_num ++;
        for (int i = 0; i < n; i++) {
            printf("%02x", buf[i]);
            if (i != (n - 1)) {
                printf(" ");
            }
        }

        if (print_printable_chars) {
            printf(" |");
            for (int i = 0; i < n; i++) {
                if (is_printable(buf[i])) {
                    printf("%c", buf[i]);
                }
                else {
                    printf(".");
                }
            }
            printf("|");
        }

        if (print_so_far_data_len) {
            printf(" (%d)", so_far_data_len);
        }
        printf("\n");
    }
        
    return 0;
}
