#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void verificar_exiftool() {
    /* Verifica se o ExifTool está disponível no sistema */
    FILE *pipe = popen("exiftool -ver", "r");
    if (pipe != NULL) {
        char exiftool_version[100];
        if (fgets(exiftool_version, sizeof(exiftool_version), pipe) != NULL) {
            printf("ExifTool versão: %s", exiftool_version);
        }
        pclose(pipe);
    } else {
        printf("ExifTool não está instalado ou não está no PATH. Por favor, instale o ExifTool.\n");
        exit(1);
    }
}

void extrair_metadados(char *caminho_pasta) {
    /* Lista de extensões de arquivos suportados */
    char *extensoes_suportadas[] = {".docx", ".pdf", ".mp4", ".mov", ".avi", ".jpg", ".jpeg", ".png", ".gif", ".tiff", ".mp3",  NULL};

    /* Caminho do arquivo de saída */
    char arquivo_saida[1000];
    snprintf(arquivo_saida, sizeof(arquivo_saida), "%s/metadados_extrados.txt", caminho_pasta);

    FILE *saida = fopen(arquivo_saida, "w");
    if (saida == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        exit(1);
    }

    /* Itera sobre os arquivos na pasta fornecida */
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(caminho_pasta)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            char *nome_arquivo = ent->d_name;
            char *extensao = strrchr(nome_arquivo, '.');
            if (extensao != NULL && strcmp(extensao, ".") != 0 && strcmp(extensao, "..") != 0) {
                /* Verifica se a extensão do arquivo está na lista de extensões suportadas */
                int i;
                for (i = 0; extensoes_suportadas[i] != NULL; i++) {
                    if (strcmp(extensao, extensoes_suportadas[i]) == 0) {
                        /* Caminho completo do arquivo */
                        char caminho_arquivo[1000];
                        snprintf(caminho_arquivo, sizeof(caminho_arquivo), "%s/%s", caminho_pasta, nome_arquivo);

                        /* Comando ExifTool para extrair metadados */
                        char comando[2000];
                        snprintf(comando, sizeof(comando), "exiftool \"%s\"", caminho_arquivo);
                        FILE *pipe = popen(comando, "r");
                        if (pipe != NULL) {
                            char linha[2000];
                            while (fgets(linha, sizeof(linha), pipe) != NULL) {
                                /* Escreve os metadados no arquivo de saída */
                                fputs(linha, saida);
                            }
                            pclose(pipe);
                            fprintf(saida, "\n-------------------------------------------------------------\n");
                            printf("Metadados extraídos de: %s\n", caminho_arquivo);
                        } else {
                            printf("Erro ao extrair metadados de %s.\n", caminho_arquivo);
                        }
                        break;
                    }
                }
            }
        }
        closedir(dir);
    } else {
        printf("Erro ao abrir o diretório.\n");
        exit(1);
    }

    fclose(saida);
    printf("Todos os metadados foram salvos em %s\n", arquivo_saida);
}

int main() {
    char caminho_pasta[1000];
    printf("Digite o caminho da pasta: ");
    fgets(caminho_pasta, sizeof(caminho_pasta), stdin);
    caminho_pasta[strcspn(caminho_pasta, "\n")] = '\0';

    if (opendir(caminho_pasta) != NULL) {
        verificar_exiftool();
        extrair_metadados(caminho_pasta);
    } else {
        printf("O caminho fornecido não é uma pasta válida.\n");
        exit(1);
    }

    return 0;
}
