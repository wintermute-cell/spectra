#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <spectra.h>

#define NAME ("spectra")
#define DEFAULT_WINHEIGHT (480);
#define DEFAULT_WINWIDTH (640);

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
            prgloop_tick(&prg);

            // process events
            SDL_Event event;
            while(SDL_PollEvent(&event))
                  if(event.type == SDL_QUIT) prg.rundata.close_requested = 1;

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

      prg->rundata.canv.x = 0;
      prg->rundata.canv.y = 0;
      prg->rundata.canv.h = prg->rundata.WINHEIGHT;
      prg->rundata.canv.w = prg->rundata.WINWIDTH;

      int i;
      for(i = 0; i < 8; i++){
            SDL_Texture * lr = SDL_CreateTexture(prg->renderer,
                        SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET,
                        prg->rundata.WINWIDTH,
                        prg->rundata.WINHEIGHT);
            SDL_SetTextureBlendMode(lr, SDL_BLENDMODE_BLEND);
            prg->rundata.layer[i] = lr;
      }
}

int prgloop_tick(struct program * prg){

      /*
      * This is the place for program loop ticks.
      * This function gets called every frame.
      */

      //Clear the canvas with a white background
      SDL_SetRenderDrawColor(prg->renderer, 0, 0, 0, 255);
      SDL_RenderClear(prg->renderer);
      SDL_SetRenderDrawColor(prg->renderer, 255, 255, 255, 255);
      SDL_RenderDrawRect(prg->renderer, &(prg->rundata.canv));
      SDL_RenderFillRect(prg->renderer, &(prg->rundata.canv));

      int mouse_x, mouse_y;
      int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

      // & resolves to the bitmap that corresponds to the left mousebutton being pressed
      if(buttons & SDL_BUTTON(SDL_BUTTON_LEFT)){
            SDL_SetRenderTarget(prg->renderer, prg->rundata.layer[0]);
            SDL_SetRenderDrawColor(prg->renderer, 255, 45, 45, 255);
            SDL_RenderDrawLine(prg->renderer,
                        prg->rundata.last_mouse_x,
                        prg->rundata.last_mouse_y,
                        mouse_x,
                        mouse_y);
            SDL_SetRenderTarget(prg->renderer, NULL);
      }
      SDL_SetRenderDrawBlendMode(prg->renderer, SDL_BLENDMODE_BLEND);
      SDL_RenderCopy(prg->renderer, prg->rundata.layer[0], NULL, NULL);

      SDL_RenderPresent(prg->renderer);

      // keep mouse pos for next frame
      prg->rundata.last_mouse_x = mouse_x;
      prg->rundata.last_mouse_y = mouse_y;


      //TODO: Fully implement layers, colors, line thickness and loading/saving from/to file.
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
      return 0;
}

int program_quit(struct program * prg){

      /*
      * Window and Renderer have to be destroyed, and all allocated memory
      * freed, then the function can return 0. Otherwise return 1;
      */

      // destroy all layers
      int i;
      for(i = 0; i < 8; i++){
            SDL_DestroyTexture(prg->rundata.layer[i]);
      }

      // destroy renderer and window
      SDL_DestroyRenderer(prg->renderer);
      SDL_DestroyWindow(prg->window);

      // finally quit
      SDL_Quit();
      return 0;
}
