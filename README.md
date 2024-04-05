# Хеш-таблица

## Часть 1. Написание хеш-таблицы

### Что такое хеш-таблица
Часто для хранения информации используют ```хеш-таблицу``` - структуру данных, представленную массивом, индексами которого являются хеши соответствующих им значений.
```Хеш``` - некоторое число, которое ставится в соответствие хранимому в таблице объекту исходя из хеш-функции.

Часто хеш-функция не является биективной и тогда возникают коллизии - равенства хешей у различных объектов.
Поэтому в качестве элементов массива (```backet```-ов) я использую списки из своего [другого проекта](https://github.com/ogkisque/List). В них быстро выполняются операции вставки и удаления.

Таким образом, подобрав "хорошие" хеш-функцию и размер таблицы, мы можем быстро выполнять поиск элемента в ней.
Для этого нужно посчитать его хеш, взять остаток от деления на размер таблицы - это будет индекс нужного списка в массиве.
Далее проходимся по этому списку и ищем нужный элемент (в "хорошем" случае длина списка будет небольшая).

Для оценки эффективности хеш-таблицы (равномерности распределения) мы будем использовать 2 характеристики:

```load factor``` = среднее количество элементов в непустых списках

```дисперсия``` $D = \dfrac{\sum\limits_{i=1} ^{n} (x_i - \overline{x})}{n}$

*P.S. В общем случае в хеш-таблице хранятся пары ключ-значение и хешируются ключи. В нашем случае будут храниться только ключи, поскольку это не принципиальнои и не является объектом изучения.
В качестве объектов я буду использовать строки из трагедии Шекспира "Гамлет"*

### Сравнение эффективности различных хеш-функций

* **Хеш, всегда равный 0**

    ```C++
    size_t get_hash_zero (Elemt elem)
    {
        return 0;
    }
    ```
    Конечно, такая функция не будет эффективна, поскольку все строки попадут в 0 список. Это видно по диаграмме распределения:

    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag1.png" width = 60% height = 60%>
    </p>

    *load factor = 4680; D = 214709*
    
* **Хеш, равный ASCII коду 1 символа**

    ```C++
    size_t get_hash_ascii1 (Elemt elem)
    {
        return elem[0];
    }
    ```
    Данная функция так же не очень хороша вследствие своей ограниченности (из-за этого многие списки остаются пустыми).

    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag2.png" width = 60% height = 60%>
    </p>

    *load factor = 167; D = 10256*

* **Хеш, равный длине строки**

    ```C++
    size_t get_hash_length (Elemt elem)
    {
        return strlen (elem);
    }
    ```
    Данная функция так же является ограниченной.
    
    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag3.png" width = 60% height = 60%>
    </p>
    
    *load factor = 334; D = 27580*

* **Хеш, равный сумме ASCII кодов всех символов**

    ```C++
    size_t get_hash_sum_ascii (Elemt elem)
    {
        size_t sum = 0;
        for (int i = 0; elem[i] != '\0'; i++)
            sum += elem[i];
        return sum;
    }
    ```
    Данная функция более равномерно распределена и имеет хорошие характеристики, однако у неё есть один нюанс, о котором я скажу позже.
    
    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag4.png" width = 60% height = 60%>
    </p>
    
    *load factor = 46; D = 45*

* **Хеш, равный сумме ASCII кодов всех символов, деленной на длину слова**

    ```C++
    size_t get_hash_aver_ascii (Elemt elem)
    {
        size_t sum = get_hash_sum_ascii (elem);
        size_t len = strlen (elem);
    
        return sum / len;
    }
    ```
    Данная функция ограничена (так как это по сути среднее значение ASCII кода символа в слове), поэтому имеет плохие характеристики.
    
    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag5.png" width = 60% height = 60%>
    </p>
    
    *load factor = 142; D = 17700*

* **Хеш, основанный на циклическом сдвиге влево (rol)**

    ```C++
    size_t get_hash_rol (Elemt elem)
    {
        size_t hash = 0;
        for (int i = 0; elem[i] != '\0'; i++)
            hash = ROL(hash) ^ elem[i];
        
        return hash;
    }
    ```
    Данная функция имеет неплохие характеристики эффективности.
    
    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag6.png" width = 60% height = 60%>
    </p>
    
    *load factor = 46; D = 50*

* **Хеш, основанный на циклическом сдвиге вправо (ror)**

    ```C++
    size_t get_hash_ror (Elemt elem)
    {
        size_t hash = 0;
        for (int i = 0; elem[i] != '\0'; i++)
            hash = ROR(hash) ^ elem[i];
        
        return hash;
    }
    ```
    Данная функция похожа на предыдущую, однако имеет бОльшую дисперсию.
    
    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag7.png" width = 60% height = 60%>
    </p>
    
    *load factor = 46; D = 84*

    Интересно, что циклический сдвиг есть в ассемблере, но нет в си. Скомпилируем функцию ror на сайте [godbolt](https://godbolt.org):

    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/ror_c.png" width = 60% height = 60%>
    </p>

    И даже без оптимизаций компилятор заменяет этот код на вызов ассемблерной функции ror:

    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/ror_asm.png" width = 45% height = 45%>
    </p>

    С оптимизацией -O3 генерируется следующий код:

    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/ror_asm_O3.png" width = 30% height = 30%>
    </p>

* **GNU хеш**

    ```C++
    size_t get_hash_gnu (Elemt elem)
    {
        size_t hash = 5381;
        for (int i = 0; elem[i] != '\0'; i++)
            hash = ((hash << 5) + hash) + elem[i];
    
        return hash;
    }
    ```
    Данная функция имеет хорошее, но не лучшее распределение.
    
    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag8.png" width = 60% height = 60%>
    </p>
    
    *load factor = 46; D = 44*

* **CRC32 хеш**

    ```C++
    size_t get_hash_crc32 (Elemt elem)
    {
        unsigned int hash = 0;
        int size = 8 * sizeof (size_t);
    
        for (int i = 0; elem[i] != '\0'; i++)
            hash = crc_table[elem[i] ^ (hash >> (size - 8))] ^ (hash << 8);
    
        return (size_t) hash;
    }
    ```
    Данная функция имеет наиболее равномерное распределение среди представленных.
    
    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag10.png" width = 60% height = 60%>
    </p>
    
    *load factor = 46; D = 41*

### Проблема хеша суммы ASCII кодов всех символов

Может показаться, что хеш-функция суммы всех ASCII кодов является эффективной и при этом простой и быстрой, поэтому нужно использовать её. На самом деле это не так в силу её ограниченности.
Это наглядно видно, если взять размер таблицы побольше:

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag9.png" width = 60% height = 60%>
</p>

Заметно, что много списков остаются пустыми, что как раз говорит о неэффективности этой функции в общем случае.

### Подведение итогов по хеш-функциям

Итоговая таблица по эффективности хеш-функций выглядит так:

Хеш-функция | Тождественный 0 | ASCII 1 символа | Длина строки | Сумма ASCII | Средний ASCII | Rol | Ror | GNU | CRC32 
---         | ---             | ---             | ---          |---          |---            |---  |---  |---  |---
Load factor | 4680            | 167             | 334          | 46          | 142           | 46  | 46  | 46  | 46
Dispersion  | 214709          | 10256           | 27580        | 45          | 17700         | 50  | 84  | 44  | 41

Видно, что лучшим является ```CRC32 hash```, его я и буду использовать во 2 части проекта.

## Часть 2. Оптимизация алгоритмов хеш-таблицы

