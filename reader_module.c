char *sh_read_line(void)
{
    char *line = NULL;
    size_t bufsize = 0;

    if(getline(&line, &bufsize, stdin) == -1) {
        if (feo(stdin)) {
            if (line) {
                free(line);
            }
            return NULL;
        } else {
            perror("sh_read_line");
            if (line) {
                free(line);
            }
            return NULL;
        }
    }
}