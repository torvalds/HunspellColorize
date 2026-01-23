#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include <hunspell.h>

const char *aff_path = "/usr/share/hunspell/en_US.aff";
const char *dic_path = "/usr/share/hunspell/en_US.dic";

#define START "\033[31m"
#define STOP "\033[0m"

#define SMALLBUF 80

struct state {
	Hunhandle *hunhandle;
	enum {
		Newline,
		Noise,
		Word,
		NotAWord,
		Escape,
	} state;
	int esclen;
	char escape[SMALLBUF];
	int wordlen;
	char word[SMALLBUF];
	int resetlen;
	char reset[SMALLBUF];
};

static int check_word(struct state *st)
{
	// We're not doing German of Finnish...
	if (st->wordlen == SMALLBUF)
		return 1;
	st->word[st->wordlen] = 0;
	return Hunspell_spell(st->hunhandle, st->word);
}

static void check_and_print(struct state *st)
{
	if (check_word(st)) {
		write(1, st->word, st->wordlen);
		return;
	}

	write(1, START, strlen(START));
	write(1, st->word, st->wordlen);
	write(1, STOP, strlen(STOP));
	write(1, st->reset, st->resetlen);
}

static void handle_escape(struct state *st, char c)
{
	if (st->esclen < SMALLBUF)
		st->escape[st->esclen++] = c;
}

static void remember_escape(struct state *st)
{
	st->resetlen = st->esclen;
	memcpy(st->reset, st->escape, st->esclen);
}

// Print a single line, colorizing unrecognized words
//
// This is incredibly stupid, and only handles plain
// US-ASCII text.
static void process(struct state *st, const char *buf, size_t len)
{
	const char *last = buf;

	for ( ; len > 0 ; len--, buf++) {
		unsigned char c = *buf;
		switch (c) {
		case 128 ... 255:
		case 'A' ... 'Z':
		case 'a' ... 'z':
			switch (st->state) {
			case Word:
				if (st->wordlen == SMALLBUF) {
					write(1, st->word, SMALLBUF);
					st->state = NotAWord;
					continue;
				}
				st->word[st->wordlen++] = c;
				continue;
			case NotAWord:
				continue;
			case Escape:
				handle_escape(st, c);
				if (c == 'm')
					remember_escape(st);
				st->state = Noise;
				continue;
			default:
				if (last < buf)
					write(1, last, buf-last);
				st->state = Word;
				st->wordlen = 1;
				st->word[0] = c;
				continue;
			}

		// Mixed letters and numbers / underscores are C identifiers
		case '0' ... '9':
		case '_':
			switch (st->state) {
			case Escape:
				handle_escape(st, c);
				continue;
			case Word:
				write(1, st->word, st->wordlen);
				last = buf;
				st->state = NotAWord;
				continue;
			default:
				st->state = NotAWord;
				continue;
			}

		// Special case
		case '\'':
			if (st->state == Word && len > 1 && isalpha(buf[1]) && st->wordlen < SMALLBUF) {
				st->word[st->wordlen++] = c;
				continue;
			}
			/* fallthrough */
		default:
			switch (st->state) {
			case Escape:
				handle_escape(st, c);
				continue;
			case Word:
				check_and_print(st);
				last = buf;
				break;
			default:
				break;
			}
			switch (c) {
			case '\n':
				st->state = Newline;
				continue;
			case '\033':
				st->state = Escape;
				st->esclen = 1;
				st->escape[0] = '\033';
				continue;
			default:
				st->state = Noise;
				continue;
			}
		}
	}

	// We always flush the buffer at at the end
	if (st->state != Word && last < buf)
		write(1, last, buf-last);
}

static void exec_less(char **argv)
{
	execvp("less", argv);
	perror("Couldn't exec 'less'");
	exit(1);
}

static void local_dictionary(Hunhandle *handle, const char *filename)
{
	struct stat st;
	if (!stat(filename, &st) && S_ISREG(st.st_mode))
		Hunspell_add_dic(handle, filename);
}

#define BUFSIZE 1024

int main(int argc, char **argv)
{
	int fd[2];
	char buf[BUFSIZE];

	if (isatty(0))
		exec_less(argv);

	Hunhandle *hunhandle = Hunspell_create(aff_path, dic_path);

	if (!hunhandle || pipe(fd))
		exec_less(argv);

	// Add local dictionaries from cwd and $HOME
	local_dictionary(hunhandle, ".dictionary");
	char *home = getenv("HOME");
	if (home) {
		snprintf(buf, BUFSIZE, "%s/.dictionary", home);
		local_dictionary(hunhandle, buf);
	}

	if (fork()) {
		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
		exec_less(argv);
	}
	dup2(fd[1], 1);
	close(fd[0]);
	close(fd[1]);

	struct state state = {
		.hunhandle = hunhandle,
		.state = Newline,
		.resetlen = strlen(STOP),
		.reset = STOP,
	};

	for (;;) {
		ssize_t len = read(0, buf, sizeof(buf));
		if (len <= 0)
			break;

		process(&state, buf, len);
	}

	// process() flushes the input buffer at the
	// end, but will leave any partial words for
	// the next iteration. Deal with that here.
	if (state.state == Word)
		check_and_print(&state);

	Hunspell_destroy(hunhandle);

	return 0;
}
