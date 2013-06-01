#include <string.h>
#include <stdio.h>
#include <sbthread.h>
#include <strfuncs.h>
#include <fe.h>
#include <ckd_alloc.h>
#include <err.h>

#include "test_macros.h"

static const arg_t fe_args[] = {
	waveform_to_cepstral_command_line_macro(),
	{ NULL, 0, NULL, NULL }
};

static int
process(sbthread_t *th)
{
	FILE *raw, *logfh;
	int16 *buf;
	mfcc_t **cepbuf;
	size_t nsamps;
	fe_t *fe;
	long fsize;
	int32 nfr;
	char outfile[16];

	sprintf(outfile, "%03ld.log", (long)sbthread_arg(th));
	if ((logfh = fopen(outfile, "w")) == NULL)
		return -1;
	err_set_logfp(logfh);
	if ((fe = fe_init_auto_r(sbthread_config(th))) == NULL)
		return -1;
	if ((raw = fopen(TESTDATADIR "/chan3.raw", "rb")) == NULL)
		return -1;
	fseek(raw, 0, SEEK_END);
	fsize = ftell(raw);
	fseek(raw, 0, SEEK_SET);
	buf = ckd_malloc(fsize);
	fread(buf, 1, fsize, raw);
	nsamps = fsize / 2;

	fe_process_utt(fe, buf, nsamps, &cepbuf, &nfr);
	E_INFO("nfr = %d\n", nfr);
	fe_free_2d(cepbuf);
	ckd_free(buf);
	fclose(raw);
	fe_free(fe);
	fclose(logfh);

	return 0;
}

int
main(int argc, char *argv[])
{
	sbthread_t *threads[10];
	cmd_ln_t *config;
	int i;

	E_INFO("Processing chan3.raw in 10 threads\n");
	if ((config = cmd_ln_parse_r(NULL, fe_args, 0, NULL, FALSE)) == NULL)
		return -1;
	for (i = 0; i < 10; ++i) {
		config = cmd_ln_retain(config);
		threads[i] = sbthread_start(config, process, (void *)(long)i);
	}
	for (i = 0; i < 10; ++i) {
		int rv;
		rv = sbthread_wait(threads[i]);
		E_INFO("Thread %d exited with status %d\n", i, rv);
		sbthread_free(threads[i]);
	}
	/* Now check to make sure they all created logfiles with the
	 * correct contents. */
	for (i = 0; i < 10; ++i) {
		char logfile[16], line[256];
		FILE *logfh;

		sprintf(logfile, "%03d.log", i);
		TEST_ASSERT(logfh = fopen(logfile, "r"));
		while (fgets(line, sizeof(line), logfh)) {
			string_trim(line, STRING_BOTH);
			printf("%s: |%s|\n", logfile, line);
			TEST_EQUAL(0, strcmp(line, "INFO: test_tls_log.c(44): nfr = 1436"));
		}
		fclose(logfh);
	}
	cmd_ln_free_r(config);
	return 0;
}
