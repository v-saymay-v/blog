#include <stdio.h>
#include <unistd.h>

int copy_file(const char *chp_from, const char *chp_to)
{
	FILE *fp_from;
	FILE *fp_to;
	char cha_buff[8192];
	int in_read;

	fp_from = fopen(chp_from, "r");
	if (!fp_from) {
		fprintf(stderr, "failed to open '%s'.\n", chp_from);
		return 1;
	}

	fp_to = fopen(chp_to, "w+");
	if (!fp_to) {
		fclose(fp_from);
		fprintf(stderr, "failed to create '%s'.\n", chp_to);
		return 1;
	}

	do {
		in_read = fread(cha_buff, 1, sizeof(cha_buff), fp_from);
		fwrite(cha_buff, 1, in_read, fp_to);
	} while (in_read >= sizeof(cha_buff));

	fclose(fp_from);
	if (fclose(fp_to)) {
		fprintf(stderr, "failed to write '%s'.\n", chp_to);
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	static char cha_dbbdic[] = "/usr/local/share/chasen/dic/ipadic/dbbdic.dict";

	if (argc < 1) {
		fprintf(stderr, "No file is specified.\n");
		return 1;
	}
	if (copy_file(argv[1], cha_dbbdic)) {
		return 1;
	}
	if (!fork()) {
		static char cha_chasendic[] = "/usr/local/share/chasen/dic/ipadic";
		static char cha_makeda[] = "/usr/local/libexec/chasen/makeda";
		chdir(cha_chasendic);
		execl(cha_makeda, cha_makeda, "dbbdic", "dbbdic.dict", NULL);
		fprintf(stderr, "failed to execute 'makeda'.\n");
		exit(0);
	}
	return 1;
}
