#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <spectra.h>

#define NAME ("spectra")
#define DEFAULT_WINHEIGHT (480)
#define DEFAULT_WINWIDTH (640)
#define MENB_HEIGHT (24)

// define theme
SDL_Color menubar_color = {255, 255, 255, 255};

// define color palette
SDL_Color color_red = {216, 39, 53, 255};
SDL_Color color_yellow = {255, 161, 53, 255};
SDL_Color color_green = {0, 158, 71, 255};
SDL_Color color_blue = {0, 121, 231, 255};
SDL_Color color_purple = {125, 60, 181, 255};


int main(int argc, char **argv){

      struct program prg;

      // parsing args
      prg.rundata.argc = argc;
      prg.rundata.argv = argv;
      if(parseargs(&prg)) return 1;

      // initializing program
      if(program_init(&prg)) return 1;

      // main loop
      prgloop_start(&prg);
      prg.rundata.close_requested = 0;

      while(!prg.rundata.close_requested){

            // program tick
            if(!prgloop_tick(&prg)) break;

            // run at 144 fps
            SDL_Delay(1000/240);
      }

      if(program_quit(&prg)) return 1;
      return 0;
}

int prgloop_start(struct program * prg){

      /*
      * This is the place for program loop specific initialization.
      */

      // start on layer 1
      prg->rundata.curr_layer = 0;
      prg->rundata.curr_color = 1;

      // init canvas dimensions
      prg->rundata.canv.x = 0;
      prg->rundata.canv.y = 0;
      prg->rundata.canv.h = prg->rundata.WINHEIGHT;
      prg->rundata.canv.w = prg->rundata.WINWIDTH;

      // init menubar dimensions
      prg->rundata.menb.x = 0;
      prg->rundata.menb.y = 0;
      prg->rundata.menb.h = MENB_HEIGHT;
      prg->rundata.menb.w = prg->rundata.WINWIDTH;

      // init menubar layer number dimensions
      prg->rundata.menb_num.x = prg->rundata.WINWIDTH-50;
      prg->rundata.menb_num.y = 2;
      prg->rundata.menb_num.h = MENB_HEIGHT-4;
      prg->rundata.menb_num.w = 46;

      // init color palette rects
      int i;
      for(i = 0; i < 5; i++){
            prg->rundata.menb_color[i].x = prg->rundata.WINWIDTH-50-(24 * (i+1));
            prg->rundata.menb_color[i].y = 2;
            prg->rundata.menb_color[i].h = MENB_HEIGHT-4;
            prg->rundata.menb_color[i].w = 20;
      }

      // create canvas textures
      for(i = 0; i < 8; i++){
            SDL_Texture * lr = SDL_CreateTexture(prg->renderer,
                        SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET,
                        prg->rundata.WINWIDTH,
                        prg->rundata.WINHEIGHT);
            SDL_SetTextureBlendMode(lr, SDL_BLENDMODE_BLEND);
            prg->rundata.layer[i] = lr;
      }

      // create font textures
      for(i = 0; i < 8; i++){
            char img_src[24];
            sprintf(img_src, "resources/numfont/%d.png", i+1);
            SDL_Surface * sf = IMG_Load(img_src);
            if(!sf){
                  printf("error loading numfont %d\n", i+1);
                  return 1;
            }
            SDL_Texture * numtex = SDL_CreateTextureFromSurface(prg->renderer, sf);
            SDL_FreeSurface(sf);
            if(!numtex){
                  printf("error creating numtex %d\n", i+1);
                  return 1;
            }
            SDL_SetTextureBlendMode(numtex, SDL_BLENDMODE_BLEND);
            prg->rundata.numfont[i] = numtex;
      }

      return 0;
}

int prgloop_tick(struct program * prg){

      /*
      * This is the place for program loop ticks.
      * This function gets called every frame.
      */

      // process events
      SDL_Event event;
      while(SDL_PollEvent(&event))
            switch(event.type){
                  case SDL_QUIT:
                        prg->rundata.close_requested = 1;
                        return 0;
                  case SDL_KEYDOWN:
                        // could have done this by just extracting the number from the keycode, but this is (a lot) better in terms of performance.
                        switch (event.key.keysym.scancode){
                              case SDL_SCANCODE_1:
                                    prg->rundata.curr_layer = 0;
                                    break;
                              case SDL_SCANCODE_2:
                                    prg->rundata.curr_layer = 1;
                                    break;
                              case SDL_SCANCODE_3:
                                    prg->rundata.curr_layer = 2;
                                    break;
                              case SDL_SCANCODE_4:
                                    prg->rundata.curr_layer = 3;
                                    break;
                              case SDL_SCANCODE_5:
                                    prg->rundata.curr_layer = 4;
                                    break;
                              case SDL_SCANCODE_6:
                                    prg->rundata.curr_layer = 5;
                                    break;
                              case SDL_SCANCODE_7:
                                    prg->rundata.curr_layer = 6;
                                    break;
                              case SDL_SCANCODE_8:
                                    prg->rundata.curr_layer = 7;
                                    break;
                              default:
                                    printf("unrecognized key pressed!\n");
                                    break;
                        }
                        break;
            }
            if(event.type == SDL_QUIT){
            }
      //Clear the canvas with a white background
      SDL_SetRenderDrawColor(prg->renderer, 0, 0, 0, 255);
      SDL_RenderClear(prg->renderer);
      SDL_SetRenderDrawColor(prg->renderer, 255, 255, 255, 255);
      SDL_RenderFillRect(prg->renderer, &(prg->rundata.canv));

      // process input and render drawing.
      int mouse_x, mouse_y;
      int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

      // & resolves to the bitmap that corresponds to the left mousebutton being pressed
      if(buttons & SDL_BUTTON(SDL_BUTTON_LEFT)){
            // for persistance, this draws to a layer texture instead of the backbuffer
            SDL_SetRenderTarget(prg->renderer,
                        prg->rundata.layer[prg->rundata.curr_layer]);
            // look if cursor is on the menubar and change color if necessary
            try_set_color(prg, mouse_x, mouse_y); // change current color index
            set_color_by(prg, prg->rundata.curr_color); // set the color to index
            // draw the cursor movements between this frame and the last
            SDL_RenderDrawLine(prg->renderer,
                        prg->rundata.last_mouse_x,
                        prg->rundata.last_mouse_y,
                        mouse_x,
                        mouse_y);
            // unbind the render target
            SDL_SetRenderTarget(prg->renderer, NULL);
      }

      // draw all textures to the renderbuffer, from layer 1 to 8, using transparency
      SDL_SetRenderDrawBlendMode(prg->renderer, SDL_BLENDMODE_BLEND);
      int i;
      for(i = 0; i < 8; i++){
            SDL_RenderCopy(prg->renderer, prg->rundata.layer[i], NULL, NULL);
      }

      // render menubar (above everything else)
      // TODO: when saving, dont draw this, to prevent it from getting saved to the img.
      if(1){
            SDL_SetRenderDrawColor(prg->renderer,
                  menubar_color.r,
                  menubar_color.g,
                  menubar_color.b,
                  menubar_color.a);
            SDL_RenderFillRect(prg->renderer, &(prg->rundata.menb));
            SDL_RenderCopy(prg->renderer,
                  prg->rundata.numfont[prg->rundata.curr_layer],
                  NULL,
                  &(prg->rundata.menb_num));

            // draw color palette
            for(i = 0; i < 5; i++){
                  set_color_by(prg, i+1); // colors start counting with 1
                  SDL_RenderFillRect(prg->renderer, &(prg->rundata.menb_color[i]));
            }
      }

      // switch backbuffer to screen
      SDL_RenderPresent(prg->renderer);

      // keep mouse pos for next frame
      prg->rundata.last_mouse_x = mouse_x;
      prg->rundata.last_mouse_y = mouse_y;

      //TODO: implement eraser, line thickness and loading/saving from/to file.
}

int parseargs(struct program * prg){

      /*
      * The program can be launched without parameters, then
      * DEFAULT_WINHEIGHT and DEFAULT_WINWIDTH are used as
      * dimensions.
      * Otherwise either give [HEIGHT] and [WIDTH] as numeric
      * parameters or call with "-h" for usage help.
      */

      if(prg->rundata.argc > 1){
            if(!strcmp("-h", prg->rundata.argv[1])){
                  printf("Usage:\nspectra [HEIGHT] [WIDTH]\n");
                  return 0;
            }
            prg->rundata.WINHEIGHT = atoi(prg->rundata.argv[1]);
            prg->rundata.WINWIDTH = atoi(prg->rundata.argv[2]);
      } else if(prg->rundata.argc == 1) {
            prg->rundata.WINHEIGHT = DEFAULT_WINHEIGHT;
            prg->rundata.WINWIDTH = DEFAULT_WINWIDTH;
      }

      return 0;
}

int program_init(struct program * prg){

      /*
      * SDL has to be initialized, and a window and a renderer
      * have to be created. Return 1 if an error occurs.
      */

      // initializing video and timer
      if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
            printf("error initializing SDL: %s\n", SDL_GetError());
            return 1;
      }

      // creating the window
      prg->window = SDL_CreateWindow(
                  NAME,
                  SDL_WINDOWPOS_CENTERED,
                  SDL_WINDOWPOS_CENTERED,
                  prg->rundata.WINWIDTH, prg->rundata.WINHEIGHT, 0
                  );
      if(!prg->window){
            printf("error creating window: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
      }

      // creating the renderer, which inits the graphics hardware
      Uint32 rend_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
      prg->renderer = SDL_CreateRenderer(prg->window, -1, rend_flags);
      if(!prg->renderer){
            printf("error creating renderer: %s\n", SDL_GetError());
            SDL_DestroyWindow(prg->window);
            SDL_Quit();
            return 1;
      }

      // init PNG loading
      int imgFlags = IMG_INIT_PNG;
      if(!(IMG_Init(imgFlags) & imgFlags)){
            printf("error initializing SDL_image: %s\n", IMG_GetError());
            return 1;
      }
      return 0;
}

void set_color_by(struct program * prg, int index){
            switch(index){
                  case 1:
                        SDL_SetRenderDrawColor(prg->renderer,
                                    color_red.r,
                                    color_red.g,
                                    color_red.b,
                                    color_red.a);
                        break;
                  case 2:
                        SDL_SetRenderDrawColor(prg->renderer,
                                    color_yellow.r,
                                    color_yellow.g,
                                    color_yellow.b,
                                    color_yellow.a);
                        break;
                  case 3:
                        SDL_SetRenderDrawColor(prg->renderer,
                                    color_green.r,
                                    color_green.g,
                                    color_green.b,
                                    color_green.a);
                        break;
                  case 4:
                        SDL_SetRenderDrawColor(prg->renderer,
                                    color_blue.r,
                                    color_blue.g,
                                    color_blue.b,
                                    color_blue.a);
                        break;
                  case 5:
                        SDL_SetRenderDrawColor(prg->renderer,
                                    color_purple.r,
                                    color_purple.g,
                                    color_purple.b,
                                    color_purple.a);
                        break;
                  default:
                        printf("Unknown index given while trying to switch color!\n");
                        break;
            }

            return;
}

int try_set_color(struct program * prg, int mouse_x, int mouse_y){
      if(mouse_y <= MENB_HEIGHT){ // only try when mouse is on menubar
            int i;
            for(i = 0; i < 6; i++){
                  if((mouse_x > prg->rundata.menb_color[i].x)
                              && (mouse_x < prg->rundata.menb_color[i].x+20)){
                        break;
                  }
            }
            if(i == 6) return 1; // no match was found
            else {
                  prg->rundata.curr_color = i+1;
                  return 0;
            }
      }
      return 1;
}

int program_quit(struct program * prg){

      /*
      * Window and Renderer have to be destroyed, and all allocated memory
      * freed, then the function can return 0. Otherwise return 1;
      */

      // destroy all layer textures
      int i;
      for(i = 0; i < 8; i++){
            SDL_DestroyTexture(prg->rundata.layer[i]);
      }
      // destroy all numfont textures
      for(i = 0; i < 8; i++){
            SDL_DestroyTexture(prg->rundata.numfont[i]);
      }

      // destroy renderer and window
      SDL_DestroyRenderer(prg->renderer);
      SDL_DestroyWindow(prg->window);

      // finally quit
      SDL_Quit();
      return 0;
}
