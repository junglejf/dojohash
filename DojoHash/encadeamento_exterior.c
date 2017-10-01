#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "encadeamento_exterior.h"
#include "compartimento_hash.h"
#include "lista_compartimentos.h"
#include "cliente.h"


void cria_hash(char *nome_arquivo_hash, int tam)
{
	//TODO: criar a tabela hash
    
    FILE *hash = fopen(nome_arquivo_hash, "w+b");
    CompartimentoHash *novo;
    for(int i=0; i<tam; i++){
        novo = compartimento_hash(-1);
        salva_compartimento(novo,hash);
        free(novo);
    }   
    //ListaCompartimentos *lc =  le_compartimentos(hash);
    //imprime_compartimentos(lc); 
    fclose(hash);
    
}

int busca(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
    FILE *hash = fopen(nome_arquivo_hash, "r+b");
    FILE *out = fopen(nome_arquivo_dados, "r+b");
    
    int chave_hash = cod_cli%7;
    fseek(hash, chave_hash * sizeof(CompartimentoHash),SEEK_SET);
    
    int chave_dados = le_compartimento(hash)->prox;
    fclose(hash);
    
    fseek(out, chave_dados * sizeof(Cliente), SEEK_SET);
    
    Cliente *buscado = le_cliente(out);
    
    while(buscado){
        if(buscado->cod_cliente == cod_cli && buscado->status){
            free(buscado);
            return chave_dados;
        }
        fseek(out,(buscado->prox - chave_dados -1)*sizeof(Cliente),SEEK_CUR);
        chave_dados = buscado->prox;
        
        free(buscado);
        buscado = le_cliente(out);
    }
    fclose(out);
    return -1 ;
}


int insere(int cod_cli, char *nome_cli, char *nome_arquivo_hash, char *nome_arquivo_dados, int num_registros)
{
 
    FILE *arq_hash = fopen(nome_arquivo_hash, "r+b");
    FILE *arq_dados = fopen(nome_arquivo_dados, "r+b");
    
    //Cliente *novo = cliente(cod_cli, nome_cli, -1,OCUPADO);
    int pos_hash = cod_cli %7;
    int pos_arq_dados;
    Cliente *novo;
    fseek(arq_hash,pos_hash*sizeof(CompartimentoHash),SEEK_SET);
    CompartimentoHash *p_hash = le_compartimento(arq_hash);
    //verificar se a tabela hash tem espaço
    if(p_hash){
        //insere no ínicio
        if(p_hash->prox < 0){
            p_hash->prox = num_registros;
            fseek(arq_hash,pos_hash*sizeof(CompartimentoHash),SEEK_SET);
            salva_compartimento(p_hash, arq_hash);
            //free(p_hash);
            fseek(arq_dados,num_registros*sizeof(Cliente),SEEK_SET);
            novo = cliente(cod_cli, nome_cli, -1,OCUPADO);
            salva_cliente(novo,arq_dados);
            free(novo);
            pos_arq_dados = num_registros;
            
        }else{
            pos_arq_dados = p_hash->prox;
            fseek(arq_dados,pos_arq_dados*sizeof(Cliente),SEEK_SET);
            Cliente *atual = le_cliente(arq_dados);
            do{
                if(cod_cli == atual->cod_cliente){
                    free(atual);
                    free(p_hash);
                    fclose(arq_dados);
                    fclose(arq_hash);
                    return -1;
                }else if(atual -> prox >= 0){
                    pos_arq_dados = atual->prox;
                    fseek(arq_dados,pos_arq_dados*sizeof(Cliente),SEEK_SET);
                    free(atual);
                    atual = le_cliente(arq_dados);
                }
            }while(atual && atual->prox >= 0  && atual->status);
            //insere no fim
            if(atual->prox == -1 && atual->status){
                //atualiza o ponteiro para o elemento a ser inserido
                fseek(arq_dados, pos_arq_dados*sizeof(Cliente),SEEK_SET);
                atual->prox = num_registros;
                salva_cliente(atual, arq_dados);
                //coloca cursor no fim do arquivo e insere novo cliente
                fseek(arq_dados,num_registros*sizeof(Cliente),SEEK_SET);
                novo = cliente(cod_cli, nome_cli, -1,OCUPADO);
                salva_cliente(novo, arq_dados);
                free(novo);
                free(atual);
                pos_arq_dados = num_registros;
            //insere no elemento LIBERADO    
            }else{
                novo = cliente(cod_cli, nome_cli, atual->prox,OCUPADO);
                fseek(arq_dados,pos_arq_dados*sizeof(Cliente), SEEK_SET);
                salva_cliente(novo,arq_dados);
                free(novo);
                free(atual);
                
            }
        }
        free(p_hash);
        fclose(arq_dados);
        fclose(arq_hash);
        return pos_arq_dados;
    }    
    return -1;
}

int exclui(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
    int pos_arq_dados = busca(cod_cli,nome_arquivo_hash,nome_arquivo_dados); 
    if(pos_arq_dados <0){
        return -1;
    }
    FILE *arq_dados = fopen(nome_arquivo_dados, "r+b");
    fseek(arq_dados, pos_arq_dados * sizeof(Cliente), SEEK_SET);
    Cliente *atual = le_cliente(arq_dados);
    atual->status = LIBERADO;
    fseek(arq_dados, -1 * sizeof(Cliente), SEEK_CUR); //volta o cursor no cliente lido
    salva_cliente(atual,arq_dados);
    free(atual);
    fclose(arq_dados);
    return pos_arq_dados;

}
