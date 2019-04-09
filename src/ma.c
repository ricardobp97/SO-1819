/* Manutenção de Artigos

Este programa deverá permitir inserir novos artigos (especificando o nome e preço de venda),
ou alterar atributos de um dado artigo (nome ou preço).
Cada artigo tem um código numérico, atribuído na criação
como o próximo de uma sequência (1, 2, 3, . . . ).
Para o acesso a um artigo no ficheiro ARTIGOS poder ser feito
usando o código como índice, as entradas devem ter um tamanho fixo.
Sendo o nome de tamanho variável, este atributo no ficheiro de artigos deve conter
apenas “referências” (e.g., posição) onde se encontram num ficheiro STRINGS,
em separado, contendo os respectivos nomes.
É aceitável que o ficheiro STRINGS contenha nomes obsoletos, desperdiçando algum espaço,
de forma a que se possa apenas acrescentar conteúdo mesmo aquando da alteração de nomes.
Este programa recebe todo o seu input pelo seu stdin,
lendo linhas de texto com o formato do exemplo seguinte:

$ ma
i <nome> <preço> // insere novo artigo, mostra o código
n <código> <novo nome> // altera nome do artigo
p <código> <novo preço> //altera preço do artigo
...
<EOF>

*/