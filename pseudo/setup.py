from setuptools import setup, find_packages

setup (
  name='pseudolexer',
  packages=find_packages(),
  entry_points = """
    [pygments.lexers]
    pseudolexer = pseudolexer.lexer:PseudoLexer
    """,
)
