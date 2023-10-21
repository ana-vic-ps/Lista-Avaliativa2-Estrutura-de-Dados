#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Musica {
    char artista[100];
    char titulo[100];
    struct Musica* proxima;
    struct Musica* anterior;
} Musica;

Musica* playlist = NULL;
Musica* musicaAtual = NULL;

void carregaPlaylist() {
    FILE* file = fopen("musicas.txt", "r");
    if (file == NULL) {
        printf("Erro: Não foi possível abrir o arquivo.\n");
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), file)) {
        char *artista, *titulo;
        artista = strtok(linha, ";");
        titulo = strtok(NULL, ";\n");

        Musica* novaMusica = (Musica*)malloc(sizeof(Musica));
        strcpy(novaMusica->artista, artista);
        strcpy(novaMusica->titulo, titulo);
        novaMusica->proxima = NULL;

        if (playlist == NULL) {
            playlist = novaMusica;
            playlist->proxima = playlist;
            playlist->anterior = playlist;
            musicaAtual = playlist;
        } else {
            novaMusica->proxima = playlist;
            novaMusica->anterior = playlist->anterior;
            playlist->anterior->proxima = novaMusica;
            playlist->anterior = novaMusica;
        }
    }

    fclose(file);
}

void exibePlaylist() {
    if (playlist == NULL) {
        printf("A playlist está vazia.\n");
        return;
    }

    Musica* atual = playlist;
    printf("\n===========================================================\n");
    printf("PLAYLIST:\n\n");
    int numeroMusica = 1;
    do {
        printf("%d. Artist: %s, Title: %s\n", numeroMusica, atual->artista, atual->titulo);
        atual = atual->proxima;
        numeroMusica++;
    } while (atual != playlist);
}

void exibeMusica(Musica* musica) {
    printf("\n===========================================================\n");
    printf("MÚSICA ATUAL:\n\n");
    printf("Artista: %s, Título: %s\n", musica->artista, musica->titulo);
}

void salvaPlaylist() {
    FILE* file = fopen("musicas.txt", "w");
    if (file == NULL) {
        printf("Erro: Não foi possível abrir o arquivo para escrita.\n");
        return;
    }

    Musica* atual = playlist;
    do {
        fprintf(file, "%s;%s\n", atual->artista, atual->titulo);
        atual = atual->proxima;
    } while (atual != playlist);

    fclose(file);
}

void avancaMusica() {
    if (musicaAtual != NULL) {
        musicaAtual = musicaAtual->proxima;
        exibeMusica(musicaAtual);
    } else {
        printf("A playlist está vazia.\n");
    }
}

void retomaMusica() {
    if (musicaAtual != NULL) {
        musicaAtual = musicaAtual->anterior;
        exibeMusica(musicaAtual);
    } else {
        printf("A playlist está vazia.\n");
    }
}

void insereMusica(char* artista, char* titulo) {
    Musica* novaMusica = (Musica*)malloc(sizeof(Musica));
    strcpy(novaMusica->artista, artista);
    strcpy(novaMusica->titulo, titulo);
    novaMusica->proxima = NULL;

    if (playlist == NULL) {
        playlist = novaMusica;
        playlist->proxima = playlist;
        playlist->anterior = playlist;
        musicaAtual = playlist;
    } else {
        novaMusica->proxima = playlist;
        novaMusica->anterior = playlist->anterior;
        playlist->anterior->proxima = novaMusica;
        playlist->anterior = novaMusica;
    }

    printf("\nNOVA MÚSICA ADICIONADA À PLAYLIST:\n");
    exibeMusica(novaMusica);
    salvaPlaylist();  // Salvar alterações no arquivo
}

void removeMusica(char* titulo) {
    if (playlist == NULL) {
        printf("A playlist está vazia.\n");
        return;
    }

    Musica* atual = playlist;
    do {
        if (strcmp(atual->titulo, titulo) == 0) {
            if (atual == musicaAtual) {
                musicaAtual = atual->proxima;
            }
            atual->anterior->proxima = atual->proxima;
            atual->proxima->anterior = atual->proxima;
            free(atual);
            printf("\nMÚSICA '%s' REMOVIDA DA PLAYLIST.\n", titulo);
            salvaPlaylist();  // Salvar alterações no arquivo
            return;
        }
        atual = atual->proxima;
    } while (atual != playlist);

    printf("\nMúsica '%s' não encontrada na playlist.\n", titulo);
}

void buscaMusica(char* titulo) {
    if (playlist == NULL) {
        printf("A playlist está vazia.\n");
        return;
    }

    Musica* atual = playlist;
    int numeroMusica = 1;
    do {
        if (strcmp(atual->titulo, titulo) == 0) {
            printf("\nMÚSICA '%s' ENCONTRADA NA PLAYLIST:\n", titulo);
            printf("%d. Artista: %s, Título: %s\n", numeroMusica, atual->artista, atual->titulo);
            return;
        }
        atual = atual->proxima;
        numeroMusica++;
    } while (atual != playlist);

    printf("\nMúsica '%s' não encontrada na playlist.\n", titulo);
}

// Função para comparar duas músicas com base no título
int comparaMusicaPorTitulo(const void* a, const void* b) {
    Musica* itemA = *(Musica**)a;
    Musica* itemB = *(Musica**)b;
    return strcmp(itemA->titulo, itemB->titulo);
    //return strcmp(((Song*)a)->title, ((Song*)b)->title);
}

// Função para exibir a lista de reprodução ordenada por título
void exibeListaOrdenada() {
    if (playlist == NULL) {
        printf("\nA playlist está vazia.\n");
        return;
    }

    // Copie as músicas para um array de estruturas de música
    Musica* musicasOrdenadas[100]; // Assumindo um máximo de 100 músicas
    Musica* atual = playlist;
    int contador = 0;

    do {
        musicasOrdenadas[contador] = atual;
        atual = atual->proxima;
        contador++;
    } while (atual != playlist);

    // Ordene o array de músicas com base no título
    qsort(musicasOrdenadas, contador, sizeof(Musica*), comparaMusicaPorTitulo);

    // Exiba a lista de reprodução ordenada
    printf("LISTA DE REPRODUÇÃO ORDENADA POR TÍTULOS:\n\n");
    for (int i = 0; i < contador; i++) {
        printf("%d. Artista: %s, Título: %s\n", i + 1, musicasOrdenadas[i]->artista, musicasOrdenadas[i]->titulo);
    }
}

int main() {
    carregaPlaylist();
    int escolha;

    while (1) {
        printf("\nOpções:\n");
        printf("1 - Exibir a playlist por ordem de cadastro\n");
        printf("2 - Exibir a playlist ordenada por nome de musica\n");
        printf("3 - Inserir uma nova música\n");
        printf("4 - Remover uma música\n");
        printf("5 - Pesquisar uma música\n");
        printf("6 - Tocar a próxima música\n");
        printf("7 - Tocar a música anterior\n");
        printf("8 - Sair\n");
        printf("\nDigite sua escolha: ");
        scanf("%d", &escolha);

        switch (escolha) {
            case 1:
                exibePlaylist();
                printf("\n===========================================================\n");
                break;
            case 2:
                printf("\n===========================================================\n");
                exibeListaOrdenada();
                printf("\n===========================================================\n");
                break;
            case 3:
                char novoArtista[100], novoTitulo[100];
                printf("\nDigite o artista: ");
                scanf(" %[^\n]s", novoArtista);
                printf("Digite o título: ");
                scanf(" %[^\n]s", novoTitulo);
                insereMusica(novoArtista, novoTitulo);
                break;
            case 4:
                char musicaParaRemover[100];
                printf("\nDigite o título da música a ser removida: ");
                scanf(" %[^\n]s", musicaParaRemover);
                removeMusica(musicaParaRemover);
                break;
            case 5:
                char musicaParaBusca[100];
                printf("\nDigite o título da música a ser pesquisada: ");
                scanf(" %[^\n]s", musicaParaBusca);
                buscaMusica(musicaParaBusca);
                break;
            case 6:
                avancaMusica();
                printf("===========================================================\n");
                break;
            case 7:
                retomaMusica();
                printf("===========================================================\n");
                break;
            case 8:
                salvaPlaylist();
                exit(0);
            default:
                printf("\nEscolha inválida. Tente novamente.\n");
        }
    }

    return 0;
}
