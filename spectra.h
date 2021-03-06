struct rundata {
      int WINHEIGHT;
      int WINWIDTH;
      char argc;
      char ** argv;
      SDL_Rect canv;
      SDL_Rect menb;
      SDL_Rect menb_num;
      SDL_Rect menb_color[5];
      int close_requested;
      SDL_Texture * layer[8];
      int last_mouse_x;
      int last_mouse_y;
      SDL_Texture * numfont[8];
      int curr_layer;
      int curr_color;
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

void set_color_by(struct program * prg, int index);
int try_set_color(struct program * prg, int mouse_x, int mouse_y);
