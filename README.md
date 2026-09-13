# Pong

[Português (Brasil)](README.md) | [English](README.en.md)

Um Pong para um jogador, escrito em C com SDL2. Enfrente o computador em tela cheia: rebata a bola com sua raquete e faça com que ela passe pelo adversário. A bola acelera a cada duas rebatidas. Uma bola perdida encerra a partida; você pode começar outra imediatamente.

## Baixar e jogar

| Plataforma | Download | Como abrir |
| --- | --- | --- |
| Windows 11, 64 bits (Intel/AMD) | [Pong — Windows](https://github.com/LukasPio/pong/releases/tag/v1.0.1-windows) | Baixe `pong-windows-x86_64.exe` e dê dois cliques. |
| Linux, 64 bits (Intel/AMD) | [Pong — Linux](https://github.com/LukasPio/pong/releases/tag/v1.0.1-linux) | Baixe `pong-linux-x86_64`, dê permissão de execução e abra. |

Cada download é um único executável com SDL2, SDL2_ttf, FreeType, fonte e avisos de licença embutidos. Não precisa instalar essas bibliotecas, copiar DLLs, baixar a pasta `font` ou compilar o projeto. O jogo funciona offline e pode ser aberto de qualquer pasta.

No Linux:

```sh
chmod +x pong-linux-x86_64
./pong-linux-x86_64
```

A release Linux é compilada no Ubuntu 22.04 e requer glibc 2.35 ou superior e uma sessão gráfica X11 ou Wayland com XWayland. O sistema fornece as bibliotecas gráficas e os drivers de vídeo. Distribuições baseadas em musl, como Alpine, não são compatíveis com esse binário. O executável Windows usa apenas DLLs do próprio Windows e não requer o Visual C++ Redistributable.

Nas releases, escolha o executável em **Assets**. Os arquivos automáticos **Source code** são para desenvolvimento. `SHA256SUMS.txt` permite conferir a integridade do download; `THIRD-PARTY-NOTICES.txt` contém uma cópia das licenças já embutidas.

## Controles

| Tecla | Ação |
| --- | --- |
| `W` ou `↑` | Mover a raquete para cima |
| `S` ou `↓` | Mover a raquete para baixo |
| `R` | Reiniciar depois de vencer ou perder |
| `Esc` ou `-` | Sair |

O jogo abre diretamente em tela cheia, mantendo a proporção da área de jogo de 1280 × 720. A raquete à esquerda é sua; a da direita é controlada pelo computador. As mensagens dentro do jogo estão em inglês.

## Compilar a partir do código

Requisitos: CMake 3.22+, um compilador C e acesso à internet na primeira configuração. O CMake baixa versões fixas das dependências e verifica seus hashes SHA-256. A fonte redistribuível DejaVu Sans Bold está no repositório; a Arial original não é usada nas builds.

No Ubuntu/Debian:

```sh
sudo apt-get update
sudo apt-get install build-essential cmake ninja-build pkg-config \
  libx11-dev libxext-dev libxcursor-dev libxi-dev libxfixes-dev \
  libxrandr-dev libxss-dev libgl1-mesa-dev
cmake -S . -B build/linux -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/linux --parallel
./build/linux/pong
```

Para gerar o `.exe` de Windows a partir do Linux, instale também `gcc-mingw-w64-x86-64`:

```sh
cmake -S . -B build/windows -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64.cmake
cmake --build build/windows --parallel
```

Saída: `build/windows/pong.exe`. Os atalhos `make`, `make test` e `make windows` também estão disponíveis.

## Validação e releases

```sh
ctest --test-dir build/linux --output-on-failure
```

O teste `--smoke-test` verifica inicialização, fonte embutida, renderização das duas telas de resultado, movimento, reinício e saída. O workflow [Release](.github/workflows/release.yml) também executa o binário Linux fora do projeto em uma tela virtual, verifica as dependências dos dois binários e executa o `.exe` em um runner Windows. Isso não substitui testar manualmente em cada distribuição ou em um PC com Windows 11.

Para publicar uma nova versão, ajuste a versão em `CMakeLists.txt` e os links de download nos READMEs, envie as alterações e execute **Actions → Release → Run workflow**, informando uma versão como `v1.0.1`. Após os testes, o workflow publica duas releases, com tags `v1.0.1-linux` e `v1.0.1-windows`, apontando para o mesmo commit. Tags existentes não são sobrescritas.

## Licença

O código do projeto está sob a [licença MIT](LICENSE). SDL2 e SDL2_ttf usam a licença zlib; FreeType usa a FreeType Project License; a fonte DejaVu usa a [licença Bitstream Vera, com alterações DejaVu em domínio público](font/LICENSE-DejaVu.txt). Portions of this software are copyright © The FreeType Project (www.freetype.org). All rights reserved.

Os avisos completos são incluídos no executável e em `THIRD-PARTY-NOTICES.txt` nas releases. No Linux, `./pong-linux-x86_64 --licenses` imprime esses avisos.
