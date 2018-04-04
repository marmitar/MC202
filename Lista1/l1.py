from pylatex import Document, Section, Subsection, Command, Package, NewLine
from pylatex.utils import NoEscape, bold

def gera_titulo(titulo, subtitulo = None):
    return NoEscape(r'\textbf{'+titulo+r'}' + ((r'\\\Large '+subtitulo) if subtitulo else r''))

class DefineColor(Command):
    def __init__(self, arguments, options = None):
        super().__init__('definecolor', arguments, options)

class Exercicio(Document):
    style_options = [
        "bgcolor=darkergray",
        "style=tango",
        "breaklines",
        "encoding=utf8",
        "frame=lines",
        "numbersep=2px",
        "linenos"
    ]

    def __init__(self):
        super().__init__()

        self.packages.append(Package('babel', 'brazilian'))
        self.packages.append(Package('inputenc', 'utf8'))
        self.packages.append(Package('fontenc', 'T1'))
        self.packages.append(Package('geometry', ['a4paper', 'margin=2cm']))

        self.packages.append(Package('minted'))
        self.packages.append(Package('xcolor'))
        self.packages.append(DefineColor(['darkergray', 'rgb', '.86,.86,.86']))

        self.preamble.append(Command('title', gera_titulo('Estrutura de Dados - MC202 A', '1º Semestre de 2018')))
        self.preamble.append(Command('author', bold('Tiago de Paula Alves - R.A.: 187679')))
        self.preamble.append(Command('date', Command('today')))
        self.append(Command('maketitle'))

    def adiciona_codigo(self, titulo, arq):
        with self.create(Section(titulo, False)):
            self.append(Command('inputminted', ['c', arq], self.style_options))


if __name__ == '__main__':
    doc = Exercicio()
    doc.adiciona_codigo('Lista 1 - Exercício 1', 'l1ex1.c')
    doc.generate_pdf('l1ex1', compiler_args=['-shell-escape'])

    doc = Exercicio()
    with doc.create(Section('Lista 1 - Exercício 2', False)):
        with doc.create(Subsection('a)', False)):
            doc.append(Command('inputminted', ['c', 'l1ex2.c'], doc.style_options))
        with doc.create(Subsection('b)', False)):
            doc.append("Pela assinatura da função ")
            doc.append(Command('mintinline', ['c', NoEscape('vetor_dobrar_tamanho')]))
            doc.append(", é provável que a sua implementação nem sempre funcione como esperado. Isso é devido a variável ")
            doc.append(Command('mintinline', ['c', 'vec']))
            doc.append(", que recebe seu valor de entrada durante a chamada em uma posição de memória diferente a da função superior, assim, qualquer alteração nela é perdida ao encerrar a chamada, de onde vem a parte inesperada na função. ")
            doc.append("É possível, no entanto, que função funcione se, por exemplo, foi implementada usando ")
            doc.append(Command('mintinline', ['c', 'realloc']))
            doc.append(NoEscape(r"\ e, por acaso, o sistema reconheceu que dava para alocar mais memória em sequência com a original e não precisou alterar o valor númerico do ponteiro. "))
            doc.append("Porém, esse problema como um todo pode ser facilmente evitado usando uma referência para o ponteiro do vetor, em vez do ponteiro em si, e ir atualizando o seu valor quando necessário. ")
            doc.append("Outra opção também é retornar o novo ponteiro e esperar que o usuário cuide da atualização do apontador. ")
            doc.append("Existe ainda uma solução mais críptica, possível até de ser implementada com essa assinatura, que é usando variáveis globais, porém não é recomendado, exatamente por dificultar a leitura e ser mais suscetível a modificações inesperadas e, possivelmente, erros.")
    doc.generate_pdf('l1ex2', compiler_args=['-shell-escape'])

    doc = Exercicio()
    with doc.create(Section('Lista 1 - Exercício 3', False)):
        with doc.create(Subsection('a)', False)):
            doc.append("Para um solucionador de labirintos, o melhor seria um estrutura LIFO, como uma pilha, em que as posições podem ser guardadas e facilmente retiradas na ordem em que foram colocadas, para quando se volta um caminho para tentar outro. ")
            doc.append("Tanto é que pilha é o mais encontrado em algoritmos de labirintos e, no mais genérico, busca em grafos, apesar de normalmente aparecer de maneira bem modificada.")
        with doc.create(Subsection('b)', False)):
            doc.append(Command('inputminted', ['c', 'l1ex3.c'], doc.style_options))
    doc.generate_pdf('l1ex3', compiler_args=['-shell-escape'])