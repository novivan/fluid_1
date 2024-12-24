# Fluid Simulation

Этот проект представляет собой симуляцию поведения нескольких несжимаемых жидкостей в замкнутом пространстве.

## Описание

Код использует шаблоны для поддержки различных типов данных и размеров поля. Симуляция принимает входные данные из файла с начальным положением жидкостей.

## Требования

- CMake 3.10 или выше
- Компилятор C++ с поддержкой стандарта C++20

## Сборка

1. Создайте директорию для сборки:
    ```sh
    mkdir build
    cd build
    ```

2. Запустите CMake для генерации файлов сборки:
    ```sh
    cmake ..
    ```

3. Соберите проект:
    ```sh
    make
    ```

## Запуск

Для запуска симуляции используйте одну из следующих команд(можно и другие но для этого нужно смотреть в CMakeLists.txt или менять его):
```sh
./modernized_fluid --source="../input.txt" --p-type="DOUBLE" --v-type="FIXED<32,16>" --v-flow-type="FAST_FIXED<33,20>"
```
```sh
./modernized_fluid --source="../input.txt" --p-type="FAST_FIXED<32,16>" --v-type="FAST_FIXED<32,16>" --v-flow-type="FAST_FIXED<32,16>"
```
```sh
./modernized_fluid --source="../input.txt" --p-type="DOUBLE" --v-type="DOUBLE" --v-flow-type="DOUBLE"
```
```sh
./modernized_fluid
```