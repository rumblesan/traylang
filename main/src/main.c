#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dbg.h"

#include "traylang.h"

int main(int argc, char *argv[]) {

    char *input_file = NULL;
    int c;

    while ((c = getopt(argc, argv, "i:")) != -1) {
        switch(c) {
            case 'i':
                input_file = optarg;
                break;
        }
    }

    check(input_file, "Need to supply input file path");

    FILE *fp = fopen(input_file, "r");
    check(fp, "Could not open file %s", input_file);


    TrayLang *tray = traylang_init();
    int result = traylang_interpret_file(tray, fp);
    traylang_cleanup(tray);

    fclose(fp);

    check(
        result == 0,
        "Error whilst running traylang: %s", tray->interpreter->err_message->data
    );

    return 0;
error:
    return 1;
}

