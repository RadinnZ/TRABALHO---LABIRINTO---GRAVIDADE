// Mapa 1: D Q D D W W W W W
// Mapa 2: S D D D D Q W A A W W W
// Mapa 3: D Q W D W W W A Q W A A A

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <termios.h>
#include <unistd.h>
using namespace std;

#define TAM 11

// Le uma tecla do teclado sem precisar enter
int getch(void) {
    int ch;
    termios oldt;
    termios newt;
    tcgetattr(STDIN_FILENO, &oldt);// guarda as configuracoes antigas
    newt = oldt;// copia as configuracoes antigas para as novas
    newt.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // aplica as novas configuracoes
    ch = getchar();// pega a tecla
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restaura as configuracoes antigas
    return ch;
}

// Para a tela ate o jogador apertar uma tecla
void AguardaTecla() {
    cout << "(pressione uma tecla para continuar) ";
    getch();
}

// Limpa a tela do terminal /clear
void LimpaTela() {
    system("clear");
}

const int VAZIO   = 0; 
const int PAREDE  = 1;
const int JOGADOR = 2;
const int BLOCO   = 3;
const int ALAVANCA = 4;
const int SAIDA   = 5;
const int PORTA_H = 6; // porta horizontal: fica fechada com o mapa deitado (0 ou 180 graus)
const int PORTA_V = 7; // porta vertical: fica fechada com o mapa em pe (90 ou 270 graus)

// Carrega um dos 3 mapas
void CarregaMapa(int mapa[TAM][TAM], int numeroMapa) {
    int mapa1[TAM][TAM] = {
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,2,4,1,1,1,1,1,1,1},
        {1,1,0,0,1,1,1,1,1,1,1},
        {1,1,0,0,3,0,0,0,5,1,1},
        {1,1,0,0,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1}
    };
    int mapa2[TAM][TAM] = {
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,1,1,1,1},
        {1,0,0,0,0,0,3,6,0,5,1},
        {1,2,0,0,0,0,0,1,1,1,1},
        {1,0,0,0,0,4,0,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1}
    };
    int mapa3[TAM][TAM] = {
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,2,4,0,1,0,4,0,1,1,1},
        {1,0,0,3,6,0,0,0,7,5,1},
        {1,0,0,0,1,0,0,0,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1}
    };

    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            if (numeroMapa == 1) mapa[i][j] = mapa1[i][j];
            else if (numeroMapa == 2) mapa[i][j] = mapa2[i][j];
            else mapa[i][j] = mapa3[i][j];
        }
    }
}

// Diz se uma porta (6 ou 7) esta fechada para a orientacao atual do mapa
bool PortaFechada(int valor, int orientacao) {
    if (valor == PORTA_H) return (orientacao == 0 || orientacao == 180); //fechada em 0 e 180 
    if (valor == PORTA_V) return (orientacao == 90 || orientacao == 270); //fecahda em 90 e 270
    return false;
}

// Diz se o jogador pode andar para a casa
bool CelulaAtravessavel(int mapa[TAM][TAM], int linha, int coluna, int orientacao) {
    int valor = mapa[linha][coluna];
    if (valor == PAREDE || valor == BLOCO) return false;
    if (valor == PORTA_H || valor == PORTA_V) return !PortaFechada(valor, orientacao);
    return true; //atravesssa o resto
}

bool CelulaSustentaBloco(int mapa[TAM][TAM], int linha, int coluna, int orientacao) {
    (void) orientacao;
    int valor = mapa[linha][coluna];
    if (valor == PAREDE || valor == JOGADOR || valor == BLOCO) return true; //segura
    if (valor == SAIDA || valor == PORTA_H || valor == PORTA_V) return true; //nao apaga
    return false;
}

// Procura o jogador dentro do mapa e devolve a linha e a coluna onde ele esta
void LocalizaJogador(int mapa[TAM][TAM], int &linha, int &coluna) {
    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++)
            if (mapa[i][j] == JOGADOR) { linha = i; coluna = j; }
} //sem break

void MoveJogador(int mapa[TAM][TAM], int &px, int &py, int &celulaSobJogador, char tecla, int orientacao, bool &venceu, bool &moveu) {
    int novaX = px;
    int novaY = py;
    if (tecla == 'W' || tecla == 'w') novaX--;
    if (tecla == 'S' || tecla == 's') novaX++;
    if (tecla == 'A' || tecla == 'a') novaY--;
    if (tecla == 'D' || tecla == 'd') novaY++;

    if (!CelulaAtravessavel(mapa, novaX, novaY, orientacao)) return;

    int destinoAntigo = mapa[novaX][novaY];
    mapa[px][py] = celulaSobJogador; 
    celulaSobJogador = destinoAntigo;
    mapa[novaX][novaY] = JOGADOR;
    px = novaX;
    py = novaY;
    moveu = true;

 //identifica se o jogador esta em cima da saida o que daria a vitoria

    if (destinoAntigo == SAIDA) venceu = true;
}

void GiraDireita(int origem[TAM][TAM], int destino[TAM][TAM]) {
    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++)
            destino[j][TAM - 1 - i] = origem[i][j]; // primeira coluna da matriz original vira a primeira linha da matriz girada
}

void GiraEsquerda(int origem[TAM][TAM], int destino[TAM][TAM]) {
    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++)
            destino[TAM - 1 - j][i] = origem[i][j];
}

void AplicaGravidade(int mapa[TAM][TAM], int orientacao) {
    bool mudou = true;
    while (mudou) { // ve se o bloco se mexeu
        mudou = false;
        for (int i = TAM - 2; i >= 0; i--) { // 1 bloco por vez
            for (int j = 0; j < TAM; j++) {
                if (mapa[i][j] == BLOCO) {
                    if (!CelulaSustentaBloco(mapa, i + 1, j, orientacao)) {
                        mapa[i + 1][j] = BLOCO; // se nao segura ele desce mais uma
                        mapa[i][j] = VAZIO;
                        mudou = true;
                    }
                }
            }
        }
    }
}

// Diz se o jogador ficou esmagado por uma porta que fechou em cima dps da rotação
bool JogadorEsmagado(int celulaSobJogador, int orientacao) {
    if (celulaSobJogador == PORTA_H || celulaSobJogador == PORTA_V)
        return PortaFechada(celulaSobJogador, orientacao); //verifica se esta aberta ou nao
    return false;
} // se nao for porta

void ProcessaRotacao(int mapa[TAM][TAM], int &orientacao, int &px, int &py, int &celulaSobJogador, char tecla, bool &perdeu, bool &girou) {
    if (celulaSobJogador != ALAVANCA) {
        cout << "Voce precisa estar em cima da alavanca (A) para girar o mapa." << endl;
        AguardaTecla();
        return;
    }

    int temp[TAM][TAM];
    if (tecla == 'E' || tecla == 'e') {
        GiraDireita(mapa, temp);
        orientacao = (orientacao + 90) % 360; // valor 0 e 359
    } else {
        GiraEsquerda(mapa, temp);
        orientacao = (orientacao + 270) % 360;
    }
    for (int i = 0; i < TAM; i++)
        for (int j = 0; j < TAM; j++)
            mapa[i][j] = temp[i][j];

    LocalizaJogador(mapa, px, py);
    AplicaGravidade(mapa, orientacao);
    girou = true;

    if (JogadorEsmagado(celulaSobJogador, orientacao)) perdeu = true;
}

void DesenhaCenario(int mapa[TAM][TAM], int orientacao) {
    for (int i = 0; i < TAM; i++) {
        for (int j = 0; j < TAM; j++) {
            int valor = mapa[i][j];
            switch (valor) {
                case VAZIO:   cout << "  "; break;
                case PAREDE:  cout << "# "; break;
                case JOGADOR: cout << "@ "; break;
                case BLOCO:   cout << "0 "; break;
                case ALAVANCA:cout << "A "; break;
                case SAIDA:   cout << "S "; break;
                case PORTA_H: cout << (PortaFechada(PORTA_H, orientacao) ? "= " : ": "); break;
                case PORTA_V: cout << (PortaFechada(PORTA_V, orientacao) ? "| " : "; "); break;
            } // op ternario ? true fechada | : false aberta
        }
        cout << endl;
    }
}

void ExibeStatus(int numeroMapa, int orientacao, int movimentos, int rotacoes) {
    cout << "Mapa: " << numeroMapa << " | Orientacao: " << orientacao
         << " graus | Movimentos: " << movimentos << " | Rotacoes: " << rotacoes << endl;
}

void ExibeMenu() {
    LimpaTela();
    cout << "======= MENU =======" << endl;
    cout << "1- Novo jogo        |" << endl;
    cout << "2- Continuar o jogo |" << endl;
    cout << "3- Sobre            |" << endl;
    cout << "4- Fim              |" << endl;
    cout << "====================" << endl;
}

void ExibeSobre() {
    cout << "Equipe: Arthur Ferraz de Aquino & Vicenzo Roman Radin" << endl;
    cout << "Prof. Tiago Felski | Algoritmos e Programacao II" << endl;
    cout << "OBJETIVO: leve o jogador (@) ate a saida (S)." << endl;
    cout << "W A S D: mover | Q/E: girar (so na alavanca A) | R: reiniciar | X: menu" << endl;
}

int LeDigito() {
    char tecla = (char) getch();
    cout << tecla << endl; // mostra o que foi digitado
    return tecla - '0'; //ASCII 3 = 51, 0 = 48
}

void ReiniciaFase(int mapa[TAM][TAM], int numeroMapa, int &orientacao, int &px, int &py,
                   int &celulaSobJogador, int &movimentos, int &rotacoes) {
    CarregaMapa(mapa, numeroMapa);
    LocalizaJogador(mapa, px, py);
    celulaSobJogador = VAZIO;
    orientacao = 0;
    movimentos = 0;
    rotacoes = 0;
}

// Loop principal de uma fase, mostra o mapa, le uma tecla do jogador e decide o que fazer com ela (mover, girar, reiniciar ou voltar ao menu)
void JogaFase(int mapa[TAM][TAM], int numeroMapa, int &orientacao, int &px, int &py,
              int &celulaSobJogador, int &movimentos, int &rotacoes, bool &jogoEmAndamento) {
    char tecla;
    while (true) {
        LimpaTela();
        ExibeStatus(numeroMapa, orientacao, movimentos, rotacoes);
        DesenhaCenario(mapa, orientacao);
        cout << "Comando: ";
        tecla = (char) getch();   // le a tecla na hora
        cout << tecla << endl;

        if (tecla == 'X' || tecla == 'x') {
            cout << "Voltando ao menu. O jogo fica pausado: escolha 'Continuar o jogo' para retomar de onde parou." << endl;
            AguardaTecla();
            break;
        }

        if (tecla == 'R' || tecla == 'r') {
            ReiniciaFase(mapa, numeroMapa, orientacao, px, py, celulaSobJogador, movimentos, rotacoes);
            continue;
        }

        bool venceu = false, perdeu = false, moveu = false, girou = false;

        if (tecla == 'Q' || tecla == 'q' || tecla == 'E' || tecla == 'e') {
            ProcessaRotacao(mapa, orientacao, px, py, celulaSobJogador, tecla, perdeu, girou);
            if (girou) rotacoes++; //se tiver em A rotaciona
        } else {
            MoveJogador(mapa, px, py, celulaSobJogador, tecla, orientacao, venceu, moveu);
            if (moveu) movimentos++; //move
        }

        if (perdeu) {
            cout << "Esmagado pela porta! Reiniciando a fase..." << endl;
            ReiniciaFase(mapa, numeroMapa, orientacao, px, py, celulaSobJogador, movimentos, rotacoes);
        }

        if (venceu) {
            ExibeStatus(numeroMapa, orientacao, movimentos, rotacoes);
            DesenhaCenario(mapa, orientacao);
            cout << "PARABENS, VOCE VENCEU!" << endl;
            AguardaTecla();
            jogoEmAndamento = false;
            break;
        }
    }
}

int main() {
    int mapa[TAM][TAM];
    int numeroMapa = 0, orientacao = 0, px = 0, py = 0, celulaSobJogador = 0, movimentos = 0, rotacoes = 0;
    bool jogoEmAndamento = false;

    int escolha;
    do {
        ExibeMenu();
        cout << "Opcao: ";
        escolha = LeDigito();

        if (escolha == 4) {
            cout << "Fim de jogo" << endl;
        } else if (escolha == 3) {
            ExibeSobre();
            AguardaTecla();
        } else if (escolha == 2) {
            if (jogoEmAndamento)
                JogaFase(mapa, numeroMapa, orientacao, px, py, celulaSobJogador, movimentos, rotacoes, jogoEmAndamento);
            else {
                cout << "Nenhum jogo em andamento." << endl;
                AguardaTecla();
            }
        } else if (escolha == 1) {
            cout << "Mapa (1-3) ou 0 aleatorio: ";
            int escolhaMapa = LeDigito();
            if (escolhaMapa == 0) numeroMapa = rand() % 3 + 1;
            else if (escolhaMapa >= 1 && escolhaMapa <= 3) numeroMapa = escolhaMapa;
            else numeroMapa = 1;

            ReiniciaFase(mapa, numeroMapa, orientacao, px, py, celulaSobJogador, movimentos, rotacoes);
            jogoEmAndamento = true;
            JogaFase(mapa, numeroMapa, orientacao, px, py, celulaSobJogador, movimentos, rotacoes, jogoEmAndamento);
        }
    } while (escolha != 4);

    return 0;
}
