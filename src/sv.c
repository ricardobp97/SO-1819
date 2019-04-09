/* Servidor de Vendas

Este programa deve controlar stocks, receber pedidos do cliente de vendas,
e registar as vendas efectuadas.
A quantidade em stock de cada artigo deverá ser mantida num único ficheiro STOCKS,
para todos os artigos. Cada venda efectuada deverá ser registada,
acrescentando uma entrada a um ficheiro VENDAS, contendo código, quantidade e montante total da venda.
O servidor de vendas deve ainda providenciar para correr o agregador a pedido,
fazendo com que este receba o intervalo (para ser agregado) do ficheiro de vendas desde a última agregação,
e fazendo com que o resultado da agregação seja escrito num ficheiro
cujo nome reflecte o momento em que a agregação foi solicitada (e.g., 2019-03-29T14:23:56).

*/