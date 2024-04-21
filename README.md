# Хеш-таблица

## Часть 1. Реализация хеш-таблицы

### Что такое хеш-таблица
Часто для хранения информации используют ```хеш-таблицу``` - структуру данных, представленную массивом, в котором хранятся некоторые объекты
по индексам, равным хешам соответствующих этим объектам.
```Хеш``` - некоторое число, которое ставится в соответствие хранимому в таблице объекту исходя из некоторой функции - хеш-функции.

Хеш-функция не обязательно является биективной и тогда возникают коллизии - равенства хешей у различных объектов.
Поэтому в качестве элементов массива (```backet```-ов) я использую списки из своего [другого проекта](https://github.com/ogkisque/List). В них быстро выполняются операции вставки и удаления.

Таким образом, подобрав "хорошие" хеш-функцию и размер таблицы, мы можем быстро выполнять поиск элемента в ней.
Для этого нужно посчитать его хеш, взять остаток от деления на размер таблицы - это будет индекс нужного списка в массиве.
Далее проходимся по этому списку и ищем нужный элемент (в "хорошем" случае длина списка будет небольшая).

Для оценки эффективности хеш-таблицы (равномерности распределения) мы будем использовать 2 характеристики:

```load factor``` = среднее количество элементов в непустых списках

```дисперсия``` $D = \dfrac{\sum\limits_{i=1} ^{n} (x_i - \overline{x})}{n}$

В первой части я специально сделаю такой размер таблицы, чтобы был достаточно большой load factor (40 и больше), чтобы наглядно показать диаграммы распределения разных хеш-функций. А во второй части load factor будет уменьшен (порядка 10) для увеличения скорости.

*P.S. В общем случае в хеш-таблице хранятся пары ключ-значение и хешируются ключи. В нашем случае будут храниться только ключи, поскольку это не принципиальнои и не является объектом изучения.
В качестве объектов я буду использовать строки из трагедии Шекспира "Гамлет". Уникальных слов здесь 4680. Слова дубликаты добавлять в таблицу мы не будем.*

### Сравнение эффективности различных хеш-функций

* **Хеш, всегда равный 0**

    ```C++
    size_t get_hash_zero (Elemt elem)
    {
        return 0;
    }
    ```
    Конечно, такая функция не будет эффективна, поскольку все строки попадут в один список. Это видно по диаграмме распределения:

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
    Данная функция более равномерно распределена и имеет хорошие характеристики, однако у неё есть одна проблема.
    
    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag4.png" width = 60% height = 60%>
    </p>
    
    *load factor = 46; D = 45*

    Может показаться, что эта хеш-функция является эффективной и при этом простой и быстрой, поэтому нужно использовать её. На самом деле это не так в силу её ограниченности.
    Это наглядно видно, если взять размер таблицы побольше:
    
    <p align = "center">
      <img src = "https://github.com/ogkisque/Hash-Table/blob/master/hashtable/data/diag9.png" width = 60% height = 60%>
    </p>
    
    Заметно, что много списков остаются пустыми, что как раз говорит о неэффективности этой функции в общем случае.

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

### Подведение итогов по хеш-функциям

Итоговая таблица по эффективности хеш-функций выглядит так:

Хеш-функция | Тождественный 0 | ASCII 1 символа | Длина строки | Сумма ASCII | Средний ASCII | Rol | Ror | GNU | CRC32 
---         | ---             | ---             | ---          |---          |---            |---  |---  |---  |---
Load factor | 4680            | 167             | 334          | 46          | 142           | 46  | 46  | 46  | 46
Dispersion  | 214709          | 10256           | 27580        | 45          | 17700         | 50  | 84  | 44  | 41

Видно, что лучшим является ```CRC32 hash```, его я и буду использовать во 2 части проекта.

## Часть 2. Оптимизация поиска в хеш-таблице

### Отсутствие оптимизаций

Для поиска "горячих точек" программы я использую профилировщик ```callgrind```, который показывает время выполнения каждой функции из программы.
Наша задача - оптимизировать  функцию, в которой происходит многократный поиск слов в хеш-таблице - ```hash_test_finding```.

*P.S. Будем выполнять поиск слов из того же Гамлета и еще из Гарри Поттера (чтобы были не только случаи нахождения слов). Так же каждое слово будем искать по 500 раз.*

Так выглядит листинг callgrind без каких-либо оптимизаций:

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt0main.png" width = 60% height = 60%>
</p>

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt0.png" width = 60% height = 60%>
</p>

### Отключение обработки ошибок и включение -O3

Не будем больше обрабатывать ошибки и добавим флаг компиляции -O3. Тогда мы получим  серьёзное ускорение программы:

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt1main.png" width = 60% height = 60%>
</p>

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt1.png" width = 60% height = 60%>
</p>

Эти результаты будут нашим ```base line```, то есть с ними мы будем сравнивать дальнейшие оптимизации.

### Оптимизация 1. Ассемблерная вставка

Исходя из предыдущего листинга видно, что функция ```strcmp``` вносит наибольший вклад во время выполнения. Заменим её ассемблерной вставкой:

```C++
bool search_value (List* list, Elemt value)
{
    int last_index = list->num_elems;
    for (int i = 1; i < last_index; i++)
    {
        unsigned long long equal = 0;
        asm volatile(
        ".intel_syntax noprefix\n"
        "movq rsi, %1\n"
        "movq rdi, %2\n"
        ".next:\n"
            "mov r11b, byte ptr [rsi]\n"
            "mov r10b, byte ptr [rdi]\n"
    	    "cmp r11b, 0\n"
    	    "je .end\n"
    	    "cmp r10b, 0\n"
    	    "je .end\n"
    	    "cmp r11b, r10b\n"
    	    "jne .end\n"
    	    "inc rdi\n"
    	    "inc rsi\n"
    	    "jmp .next\n"
        ".end:\n"
            "xor rax, rax\n"
            "xor rbx, rbx\n"
            "movzx rax, r11b\n"
            "movzx rbx, r10b\n"
    	    "sub rax, rbx\n"
            "movq %0, rax\n"
        ".att_syntax"
        : "=r" (equal)
        : "r" (list->nodes[i].value), "r" (value)
        : "rax", "rbx", "rsi", "rdi", "r10", "r11"
        );
        
        if (equal == 0)
            return true;
    }
    return false;
}
```

Так же заметим, что в этой задаче в списках нет вставок не в середину, удалений и прочего: все элементы вставляются в конец. Поэтому не имеет смысла перебирать итераторы, используя функцию ```next_it```. Будем просто перебирать элементы по очереди с 1 по последний.

В итоге мы получим небольшое относительное ускорение в ```1.16``` раз.

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt2main.png" width = 60% height = 60%>
</p>

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt2.png" width = 60% height = 60%>
</p>

### Оптимизация 2. SIMD инструкции

strcmp всё еще остается самой тяжелой функцией. Попробуем ускорить её по другому - через SIMD инструкции.

Длина слова в нашем тексте не превосходит 15. Поэтому вместо char* для их хранения можно использовать __m128i - вектор из 16 char.

Тогда вместо strcmp можно вызывать ```intrinsic``` функцию для сравнения векторов - ```_mm_testnzc_si128```:

```C++
bool search_value (List* list, Elemt value)
{
    int last_index = list->num_elems;
    
    for (int i = 1; i < last_index; i++)
        if (_mm_testnzc_si128 (value, list->nodes[i].value) == 0)
            return true;

    return false;
}
```

Это даёт относительное ускорение в ```1.87``` раз.

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt3main.png" width = 60% height = 60%>
</p>

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt3.png" width = 60% height = 60%>
</p>

### Оптимизация 3. Функция на ассемблере

Теперь самая долгая функция - хеширование. Перепишем её на ассемблере, причем будем использовать встроенную в него функцию crc32:

```C++
section .text
global asm_get_hash_crc32

asm_get_hash_crc32:
    xor eax, eax                    ; eax = 0
    xor r11, r11                    ; r11 = 0
    .loop:
        crc32 eax, byte [rdi+r11]
        inc r11                     ; r11++
        cmp r11, rsi
        jne .loop
    ret
```

Функция принимает не только строку, но и её длину. Чтобы не тратить время на вызовы ```strlen``` во время поиска слов, посчитаем длины слов заранее, на этапе парсинга.

В итоге получилось относительное ускорение в ```1.56``` раз.

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt4main.png" width = 60% height = 60%>
</p>

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt4.png" width = 60% height = 60%>
</p>

### Оптимизация 4. Увеличение размера таблицы

Попробуем ускорить программу увеличением размера таблицы. Тогда load factor уменьшится (в списках будет меньше элементов) и поиск будет происходить быстрее. Раньше размер был 997, сделаем его 3557. При это load factor уменьшается с 5 до 1.

На самом деле в реальных проектах эта оптимизация должна делаться в первую очередь, поскольку она может сильно ускорить программу путём простой замены константы.

Тогда относительное ускорение будет равно ```1.14```.

Это может показаться странным результатом, потому что load factor уменьшился в целых 5 раз. На самом деле нет. Уменьшив load factor, мы ускорили именно поиск элементов по спискам, однако время расчёта хеша и каких-то накладных операций осталось прежним. При уменьшении load factor в 5 раз поиск по спискам в теории должен ускориться примерно в 2.5 раз (в предположении что все слова равномерно находятся или не находятся). В нашем случае он ускорился в 1.5 раза (вероятно из-за того что многие слова находятся).

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt5main.png" width = 60% height = 60%>
</p>

<p align = "center">
  <img src = "https://github.com/ogkisque/Hash-Table/blob/master/optimisations/data/opt5.png" width = 60% height = 60%>
</p>

### Подведение итогов по оптимизациям

Посчитаем, какое ускорение (функции многократного поиска) дали оптимизации относительно base line и относительно предыдущей оптимизации:

Оптимизация                  | Абсолютное ускорение | Относительное ускорение |
---                          | ---                  | ---             
Без оптимизаций              | 0.33                 | -                         
-O3, no debug                | 1.00                 | 2.99                    
Ассемблерная вставка         | 1.16                 | 1.16
SIMD инструкции              | 2.17                 | 1.87
Функция на ассемблере        | 3.39                 | 1.56     
Увеличение размера таблицы   | 3.87                 | 1.14    

Таким образом, поиск в хеш-таблице ускорен в ```3.87``` раз.
Наиболее эффективной оптимизацией является использование SIMD инструкции для сравнения строк.
Замена strcmp ассемблерной вставкой внесла наименьший вклад. По сути её использование здесь вызвано тем, что в этом проекте должны использоваться все 3 оптимизации, чтобы показать их знание.
