# ================================================================
# PRÁCTICA: TABLAS HASH CON DATOS REALES / SINTÉTICOS
# ================================================================

import pandas as pd
import time
import matplotlib.pyplot as plt
import random

# ───────────────────────────────────────────────────────────────
# 1. CARGA DE DATOS
# ───────────────────────────────────────────────────────────────
def cargar_datos(ruta_csv: str, columna_clave: str) -> list:
    df = pd.read_csv(ruta_csv, low_memory=False)
    df.dropna(subset=[columna_clave], inplace=True)
    df[columna_clave] = df[columna_clave].astype(str).str.strip()
    claves = df[columna_clave].tolist()
    print(f'Registros cargados: {len(claves)}')
    return claves

# ───────────────────────────────────────────────────────────────
# 2. FUNCIÓN HASH
# ───────────────────────────────────────────────────────────────
def funcion_hash(clave: str, tamanio: int) -> int:
    hash_val = 0
    for c in str(clave):
        hash_val = (hash_val * 31 + ord(c)) % tamanio
    return hash_val

# ───────────────────────────────────────────────────────────────
# 3. TABLA HASH - ENCADENAMIENTO
# ───────────────────────────────────────────────────────────────
class TablaHashEncadenamiento:
    def __init__(self, tamanio: int):
        self.tamanio = tamanio
        self.tabla = [[] for _ in range(tamanio)]
        self.colisiones = 0
        self.elementos = 0

    def insertar(self, clave: str, valor=None):
        idx = funcion_hash(clave, self.tamanio)
        if self.tabla[idx]:
            self.colisiones += 1
        self.tabla[idx].append((clave, valor))
        self.elementos += 1

    def buscar(self, clave: str):
        idx = funcion_hash(clave, self.tamanio)
        for k, v in self.tabla[idx]:
            if k == clave:
                return v
        return None

    def factor_de_carga(self):
        return self.elementos / self.tamanio

# ───────────────────────────────────────────────────────────────
# 4. TABLA HASH - SONDEO LINEAL
# ───────────────────────────────────────────────────────────────
class TablaHashSondeoLineal:
    ELIMINADO = '__ELIMINADO__'

    def __init__(self, tamanio: int):
        self.tamanio = tamanio
        self.tabla = [None] * tamanio
        self.colisiones = 0
        self.elementos = 0

    def insertar(self, clave: str, valor=None):
        if self.elementos >= self.tamanio * 0.75:
            raise OverflowError('Tabla llena')

        idx = funcion_hash(clave, self.tamanio)
        pasos = 0

        while self.tabla[idx] is not None and self.tabla[idx] != self.ELIMINADO:
            self.colisiones += 1
            idx = (idx + 1) % self.tamanio
            pasos += 1
            if pasos >= self.tamanio:
                raise OverflowError('Sin espacio')

        self.tabla[idx] = (clave, valor)
        self.elementos += 1

    def buscar(self, clave: str):
        idx = funcion_hash(clave, self.tamanio)
        pasos = 0

        while self.tabla[idx] is not None:
            if self.tabla[idx] != self.ELIMINADO and self.tabla[idx][0] == clave:
                return self.tabla[idx][1]
            idx = (idx + 1) % self.tamanio
            pasos += 1
            if pasos >= self.tamanio:
                break

        return None

    def factor_de_carga(self):
        return self.elementos / self.tamanio

# ───────────────────────────────────────────────────────────────
# 5. BENCHMARK
# ───────────────────────────────────────────────────────────────
def benchmark_completo(claves, tamanios=[1009, 2003, 4001, 8009], repeticiones=5):
    resultados = []

    for m in tamanios:
        subclaves = claves[:min(len(claves), int(m * 0.65))]

        tiempos_enc = []
        tiempos_sl = []
        tiempos_py = []

        for _ in range(repeticiones):

            # Encadenamiento
            th_enc = TablaHashEncadenamiento(m)
            t0 = time.perf_counter()
            for c in subclaves:
                th_enc.insertar(c)
            tiempos_enc.append((time.perf_counter() - t0) * 1000)

            # Sondeo lineal
            th_sl = TablaHashSondeoLineal(m)
            t0 = time.perf_counter()
            for c in subclaves:
                try:
                    th_sl.insertar(c)
                except:
                    break
            tiempos_sl.append((time.perf_counter() - t0) * 1000)

            # dict Python
            dict_py = {}
            t0 = time.perf_counter()
            for c in subclaves:
                dict_py[c] = None
            tiempos_py.append((time.perf_counter() - t0) * 1000)

        resultados.append({
            'tamanio': m,
            'n': len(subclaves),
            'lambda_enc': round(th_enc.factor_de_carga(), 3),
            'col_enc': th_enc.colisiones,
            'col_sl': th_sl.colisiones,
            'ins_enc_ms': round(sum(tiempos_enc)/len(tiempos_enc), 3),
            'ins_sl_ms': round(sum(tiempos_sl)/len(tiempos_sl), 3),
            'ins_py_ms': round(sum(tiempos_py)/len(tiempos_py), 3),
        })

    return resultados

# ───────────────────────────────────────────────────────────────
# 6. TABLA DE RESULTADOS
# ───────────────────────────────────────────────────────────────
def mostrar_tabla(resultados):
    print('\n' + '='*100)
    print(f'{"m":>6} {"n":>6} {"λ":>6} {"Col_Enc":>10} {"Col_SL":>10} {"Ins_Enc(ms)":>12} {"Ins_SL(ms)":>12} {"Ins_Py(ms)":>12}')
    print('='*100)

    for r in resultados:
        print(f'{r["tamanio"]:>6} {r["n"]:>6} {r["lambda_enc"]:>6} '
              f'{r["col_enc"]:>10} {r["col_sl"]:>10} '
              f'{r["ins_enc_ms"]:>12} {r["ins_sl_ms"]:>12} {r["ins_py_ms"]:>12}')

# ───────────────────────────────────────────────────────────────
# 7. GRÁFICOS
# ───────────────────────────────────────────────────────────────
def graficar_resultados(resultados):
    tamanios = [r['tamanio'] for r in resultados]

    col_enc = [r['col_enc'] for r in resultados]
    col_sl = [r['col_sl'] for r in resultados]

    ins_enc = [r['ins_enc_ms'] for r in resultados]
    ins_sl = [r['ins_sl_ms'] for r in resultados]
    ins_py = [r['ins_py_ms'] for r in resultados]

    plt.figure()
    plt.plot(tamanios, col_enc, marker='o', label='Encadenamiento')
    plt.plot(tamanios, col_sl, marker='s', label='Sondeo Lineal')
    plt.title('Colisiones vs Tamaño')
    plt.xlabel('Tamaño tabla')
    plt.ylabel('Colisiones')
    plt.legend()
    plt.grid()

    plt.figure()
    plt.plot(tamanios, ins_enc, marker='o', label='Encadenamiento')
    plt.plot(tamanios, ins_sl, marker='s', label='Sondeo Lineal')
    plt.plot(tamanios, ins_py, marker='^', label='dict Python')
    plt.title('Tiempo de Inserción (ms)')
    plt.xlabel('Tamaño tabla')
    plt.ylabel('Tiempo')
    plt.legend()
    plt.grid()

    plt.show()

# ───────────────────────────────────────────────────────────────
# 8. MAIN
# ───────────────────────────────────────────────────────────────
if __name__ == '__main__':

    # OPCIÓN A: Kaggle
    # claves = cargar_datos('ecommerce_data.csv', 'user_id')

    # OPCIÓN B: Datos sintéticos
    random.seed(42)
    claves = [f'user_{random.randint(1, 50000)}' for _ in range(5000)]
    claves = list(set(claves))

    print(f'Claves únicas: {len(claves)}')

    resultados = benchmark_completo(claves)

    mostrar_tabla(resultados)
    graficar_resultados(resultados)