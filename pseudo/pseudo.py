from pygments.style import Style
from pygments.styles.algol import AlgolStyle
from pygments import token

from pseudolexer.lexer import comment_token

class PseudoStyle(Style):
    default_style = "algol"
    styles = AlgolStyle.styles
    styles[token.Keyword] = 'nounderline bold'
    styles[token.Name.Function] = 'bold italic #333'
    styles[token.Number] = 'bold #333'
    styles[token.Name.Variable] = 'italic #444'
    styles[token.Operator] = '#777'
    styles[token.Keyword.Pseudo] = 'noinherit nobold #222'
    styles[comment_token] = 'italic #333'
