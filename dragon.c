#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include <time.h>

#define ANIM
//#define VANISH
//#define FOUR
//#define DRAW

#ifdef ANIM
#ifdef VANISH
#error Nie mozna wybrac wiecej niz jednej opcji kompilacji
#endif
#endif

BITMAP *bufor;

char *makeseq(int n)
{
	/* Alokuj pamiec na sekwencje (= 2^n-1 bajtow) */
	--n;
	if(n < 0) n = 0;
	int len = (1 << (n+1));
	char *seq = malloc(len);

	/* Wyzeruj sekwencje */
	int i;
	for(i = 0; i < len; *(seq + i++) = 0);

	char *p = seq;

	/* Iteruj */
	do {
		*p = 0;
		int l = strlen(seq);
		strcat(p+1, seq);
		if(l)
			if(*(seq + l/2) == '0')
				*(p + l/2 + 1) = '1';
			else
				*(p + l/2 + 1) = '0';
		*p++ = '1';
		p += l;
	} while(n-- > 0);

	return seq;
}

void rysuj(int x, int y, int color, char *seq, unsigned char dir)
{
	const int step = 1;

	int nx = x, ny = y;
	int l = strlen(seq);

	unsigned int i = 0;
	do {
		switch(dir) {
			case 0:
				nx = x + step;
				ny = y;
				break;
			case 1:
				nx = x;
				ny = y + step;
				break;
			case 2:
				nx = x - step;
				ny = y;
				break;
			case 3:
				nx = x;
				ny = y - step;
				break;
		}

		#ifdef VANISH
		line(screen, x, y, nx, ny, makecol(255 - ((1.0 * i) / l) * 255, 160 - ((1.0 * i) / l) * 160, 25));
		#endif
		#ifdef ANIM
		line(bufor, x, y, nx, ny, makecol(255 - ((1.0 * i) / l) * 255, 160 - ((1.0 * i) / l) * 160, 25));
		//line(bufor, x, y, nx, ny, color);
		#endif
		#ifdef FOUR
		line(bufor, x, y, nx, ny, color);
		#endif
		#ifdef DRAW
		line(screen, x, y, nx, ny, makecol(255, 255, 0));
		rest(1);
		#endif
		x = nx;
		y = ny;

		if(*seq == '0') --dir;
		if(*seq == '1') ++dir;
		dir = dir % 4;
		++i;
	} while(*++seq && !keypressed());
	#ifdef ANIM
	blit(bufor, screen, 0, 0, 0, 0, bufor->w, bufor->h);
	#endif
	#ifdef FOUR
	blit(bufor, screen, 0, 0, 0, 0, bufor->w, bufor->h);
	#endif

}

int main(int argc, char *argv[])
{
	srand(time(NULL));

	/* Ilosc iteracji */
	int n;
	if(argc < 2)
		n = 17;
	else n = atoi(argv[1]);

	allegro_init();
	install_keyboard();
	install_timer();

	set_color_depth(24);
	set_gfx_mode(GFX_AUTODETECT, 1024, 768, 0, 0);

	bufor = create_bitmap(1024, 768);
	clear_bitmap(bufor);

	n = 1;
	#ifdef ANIM
	int nplus = 1;
	char *pat = makeseq(n);
	unsigned int i;
	int run = 1;
	while(run) {
		if(keypressed()) {
			if(key[KEY_Q]) run = 0;
			if(key[KEY_S]) {
				PALETTE pal;
				get_palette(pal);
				save_bitmap("screen.bmp", screen, pal);
			}
			clear_keybuf();
		}
		if(!(i % 30)) {
			free(pat);
			n += nplus;
			pat = makeseq(n);
			if(n > 16) nplus = -1;
			if(n < 2) nplus = 1;
		}
		rysuj(512, 384, makecol(i%256, 0, 0), pat, i%4);
		i += 11;
		rest(50);
	}
	#endif

	#ifdef VANISH
	char *pat = makeseq(17);
	rysuj(350, 300, makecol(250, 200, 100), pat, 0);

	while(!keypressed());
	#endif

	#ifdef FOUR
	char *pat = makeseq(17);
	unsigned int i;
	for(i = 0; i < 4; ++i)
		rysuj(512, 384, makecol(rand() % 255, rand() % 255, rand() % 255), pat, i);

	while(!keypressed());
	#endif

	#ifdef DRAW
	char *pat = makeseq(12);
	rysuj(350, 300, makecol(250, 200, 100), pat, 0);
	while(!keypressed());
	#endif

	allegro_exit();

	return 0;
}
