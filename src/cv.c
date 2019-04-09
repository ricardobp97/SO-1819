/* Cliente de Vendas

O cliente de vendas deve interagir com o servidor de vendas,
solicitando-lhe a execução de (uma sequência de) operações
que se distinguem facilmente pelo número de parâmetros introduzidos.
Uma das operações retorna a quantidade em stock
e o preço de um artigo (identificado pelo código).
A outra operação permite efectuar vendas ou entrada em stock,
especificando o código e quantidade (negativa ou positiva, respectivamente):

$ cv
<código_numérico>
 --> mostra no stdout stock e preço
<código_numérico> <quantidade> --> actualiza stock e mostra novo stock
...
<EOF>

O sistema deverá permitir a execução concorrente de vários clientes de vendas.
Teste o seu trabalho com dois ou mais clientes a "martelarem" simultaneamente o mesmo artigo!
A interação dos utilizadores com este cliente de vendas é feita apenas pelo stdin,
sendo os resultados retornados no stdout.
Por favor não invente, não escreva mais nenhuma mensagem para o stdout.
Inicialmente poderá querer testar manualmente o servidor de vendas
mas depois não se esqueça de criar um ficheiro contendo
(muitos, centenas ou milhares de) comandos de compra e venda de um artigo,
lançar uma dúzia de clientes que lêem esse ficheiro
e verificar pelo stock final se alguma operação se "perdeu".

*/