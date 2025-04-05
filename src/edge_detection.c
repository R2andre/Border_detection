#include <stdio.h>              // Biblioteca padrão para entrada/saída
#include <stdbool.h>            // Biblioteca para usar o tipo booleano (true/false)
#include <SDL3/SDL.h>           // Biblioteca SDL para gráficos e janelas

// Dimensões da janela
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define APP_NAME "Border Detection"  // Nome da aplicação (título da janela)

// Protótipos das funções utilizadas no código
bool initialize_sdl(SDL_Window** window, SDL_Renderer** renderer);  // Inicializa SDL, cria janela e renderer
void cleanup(SDL_Window* window, SDL_Renderer* renderer);           // Libera recursos de janela e renderer
void process_image(SDL_Surface* surface, SDL_Surface* result);     // Processar imagem
SDL_Surface* detect_edges(SDL_Surface* surface);                    // Detecta bordas na imagem original

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;             // Ponteiro para a janela
    SDL_Renderer* renderer = NULL;         // Ponteiro para o renderer (responsável por desenhar na janela)
    SDL_Surface* image = NULL;             // Superfície original da imagem carregada
    SDL_Surface* processed_image = NULL;   // Superfície da imagem após processamento
    SDL_Texture* texture = NULL;           // Textura criada a partir da imagem processada
    bool quit = false;                     // Variável de controle para sair do loop principal
    SDL_Event event;                       // Evento SDL para tratar entradas do usuário

    // Inicializa SDL e cria janela + renderer
    if (!initialize_sdl(&window, &renderer)) {
        return 1;  // Se falhar, encerra o programa
    }

    // Verifica se uma imagem foi passada como argumento na linha de comando
    if (argc > 1) {
        image = SDL_LoadBMP(argv[1]);  // Tenta carregar a imagem BMP passada como argumento
        if (!image) {
            fprintf(stderr, "Unable to load image: %s\n", SDL_GetError());  // Erro ao carregar imagem
            cleanup(window, renderer);
            return 1;
        }

        // Processa a imagem para detectar bordas
        processed_image = detect_edges(image);
        if (!processed_image) {
            fprintf(stderr, "Failed to process image\n");  // Falha ao processar imagem
            SDL_DestroySurface(image);
            cleanup(window, renderer);
            return 1;
        }

        // Cria uma textura a partir da imagem processada
        texture = SDL_CreateTextureFromSurface(renderer, processed_image);
        if (!texture) {
            fprintf(stderr, "Unable to create texture: %s\n", SDL_GetError());  // Falha ao criar textura
            SDL_DestroySurface(processed_image);
            SDL_DestroySurface(image);
            cleanup(window, renderer);
            return 1;
        }
    }

    // Loop principal da aplicação
    while (!quit) {
        // Trata eventos do usuário (teclado, janela, etc)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {  // Evento de fechar a janela
                quit = true;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {  // Evento de tecla pressionada
                if (event.key.keysym.sym == SDLK_ESCAPE) {  // Se tecla ESC for pressionada
                    quit = true;
                }
            }
        }

        // Limpa a tela com a cor preta
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Renderiza a textura da imagem processada, se houver
        if (texture) {
            SDL_FRect dest = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};  // Define a área onde a textura será desenhada
            SDL_RenderTextureRotated(renderer, texture, NULL, &dest, 0, NULL, SDL_FLIP_NONE);  // Desenha a textura
        }

        // Atualiza o conteúdo da janela
        SDL_RenderPresent(renderer);
    }

    // Libera os recursos usados
    if (texture) SDL_DestroyTexture(texture);
    if (processed_image) SDL_DestroySurface(processed_image);
    if (image) SDL_DestroySurface(image);
    cleanup(window, renderer);  // Fecha janela e renderer

    return 0;  // Encerra o programa com sucesso
}

bool initialize_sdl(SDL_Window** window, SDL_Renderer** renderer) {
    // Inicializa a biblioteca SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Cria uma nova janela
    *window = SDL_CreateWindow(APP_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!*window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Cria um renderer para desenhar na janela
    *renderer = SDL_CreateRenderer(*window, NULL, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }

    return true;  // SDL inicializado com sucesso
}

void cleanup(SDL_Window* window, SDL_Renderer* renderer) {
    // Destroi o renderer se ele existir
    if (renderer) SDL_DestroyRenderer(renderer);
    // Destroi a janela se ela existir
    if (window) SDL_DestroyWindow(window);
    // Encerra a SDL
    SDL_Quit();
}
