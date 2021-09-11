struct rundata {
      int WINHEIGHT;
      int WINWIDTH;
      char argc;
      char ** argv;
      SDL_Rect canv;
      int close_requested;
};

struct program {
      SDL_Window * window;
      SDL_Renderer * renderer;
      struct rundata rundata;
};

int parseargs(struct program * prg);
int program_init(struct program * prg);
int program_quit(struct program * prg);

int prgloop_start(struct program * prg);
int prgloop_tick(struct program * prg);
