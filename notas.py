from numpy import average

def calc(listas, labs, pesos):
    media_e = average(listas)
    media_l = average(labs, weights=pesos)

    media = 0
    if media_e != 0 and media_l != 0:
        media = 10 * media_e * media_l / (3 * media_l + 7 * media_e)

    print("Listas:", listas, " =>", media_e)
    print("Labs:", list(zip(labs, pesos)), " =>", media_l)
    print("Final:", media)


labs = [10, 10, 10, 10, 10, 10, 2, 10, 10, 10]
pesos = [1,  1,  2,  2,  2,  2, 3,  3,  3,  4]

labs_faltantes = [10, 10, 10, 10, 10, 10, 7.5, 10, 10, 10, 10, 0, 0]
labs_esperado = [10, 10, 10, 10, 10, 10, 7.5, 10, 10, 10, 10, 10, 10]
pesos_final = [1,  1,  2,  2,  2,  2,  3,  3,  3, 3, 4, 4, 4]

listas = [8.94, 10, 10]
listas_faltante = [8.94, 10, 10, 10,  0]
listas_esperado = [8.94, 10, 10, 10, 10]

print("PARCIAL")
calc(listas, labs, pesos)
print()
print("TOTAL")
calc(listas_faltante, labs_faltantes, pesos_final)
print()
print("ESPERADO")
calc(listas_esperado, labs_esperado, pesos_final)