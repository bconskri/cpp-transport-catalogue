## Задание

Задание посвящено переводу приложения на ввод-вывод в формате JSON.

### Изменения формата ввода

**Новая секция — `routing_settings`**

Во входной JSON добавляется ключ `routing_settings`, значение которого — словарь с двумя ключами:

- `bus_wait_time` — время ожидания автобуса на остановке, в минутах. Считайте, что когда
  бы человек ни пришёл на остановку и какой бы ни была эта остановка, он
  будет ждать любой автобус в точности указанное количество минут.
  Значение — целое число от 1 до 1000.
- `bus_velocity` — скорость автобуса, в км/ч. Считайте, что скорость любого автобуса
  постоянна и в точности равна указанному числу. Время стоянки на
  остановках не учитывается, время разгона и торможения тоже. Значение —
  вещественное число от 1 до 1000.

**Пример**

```
"routing_settings": {
      "bus_wait_time": 6,
      "bus_velocity": 40
}

```

Данная конфигурация задаёт время ожидания, равным 6 минутам, и скорость автобусов, равной 40 километрам в час.

**Новый тип запросов к базе — `Route`**

В список stat_requests добавляются элементы с `"type": "Route"` — это запросы на построение маршрута между двумя остановками. Помимо стандартных свойств `id` и `type`, они содержат ещё два:

- `from` — остановка, где нужно начать маршрут.
- `to` — остановка, где нужно закончить маршрут.

Оба значения — названия существующих в базе остановок. Однако они, возможно, не принадлежат ни одному автобусному маршруту.

**Пример**

```
{
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Universam",
      "id": 4
}

```

Данный запрос означает построение маршрута от остановки “Biryulyovo Zapadnoye” до остановки “Universam”.

На
маршруте человек может использовать несколько автобусов. Один автобус
даже можно использовать несколько раз, если на некоторых участках он
делает большой крюк и проще срезать на другом автобусе.

Маршрут
должен быть наиболее оптимален по времени. Если маршрутов с минимально
возможным суммарным временем несколько, допускается вывести любой из
них: тестирующая система проверяет лишь совпадение времени маршрута с
оптимальным и корректность самого маршрута.

При прохождении маршрута время расходуется на два типа активностей:

- Ожидание автобуса. Всегда длится `bus_wait_time` минут.
- Поездка на автобусе. Всегда длится ровно такое количество времени, которое
  требуется для преодоления данного расстояния со скоростью `bus_velocity`. Расстояние между остановками вычисляется по дорогам, то есть с использованием `road_distances`.

Ходить
пешком, выпрыгивать из автобуса между остановками и использовать другие
виды транспорта запрещается. На конечных остановках все автобусы
высаживают пассажиров и уезжают в парк. Даже если человек едет на
кольцевом — `"is_roundtrip": true` — маршруте и хочет проехать мимо конечной, он будет вынужден выйти и подождать *тот же самый автобус* ровно `bus_wait_time` минут. Этот и другие случаи разобраны в примерах.

Ответ на запрос `Route` устроен следующим образом:

```
{
    "request_id": <id запроса>,
    "total_time": <суммарное время>,
    "items": [
        <элементы маршрута>
    ]
}

```

`total_time` — суммарное время в минутах, которое требуется для прохождения маршрута, выведенное в виде вещественного числа.

*Обратите внимание, что расстояние от остановки A до остановки B может быть не равно расстоянию от B до A!*

`items`
— список элементов маршрута, каждый из которых описывает непрерывную
активность пассажира, требующую временных затрат. А именно элементы
маршрута бывают двух типов.

1. `Wait` — подождать нужное количество минут (в нашем случае всегда `bus_wait_time`) на указанной остановке:

```
{
    "type": "Wait",
    "stop_name": "Biryulyovo",
    "time": 6
}

```

1. `Bus` — проехать `span_count` остановок (перегонов между остановками) на автобусе `bus`, потратив указанное количество минут:

```
{
    "type": "Bus",
    "bus": "297",
    "span_count": 2,
    "time": 5.235
}

```

Если маршрута между указанными остановками нет, выведите результат в следующем формате:

```
{
    "request_id": <id запроса>,
    "error_message": "not found"
}

```

### Реализация

Задача поиска оптимального маршрута данного вида сводится к задаче поиска кратчайшего пути во взвешенном ориентированном графе.

Вам предоставляются две небольшие библиотеки:

- graph.h — класс, реализующий взвешенный ориентированный граф,
- router.h — класс, реализующий поиск кратчайшего пути во взвешенном ориентированном графе.

Эти библиотеки **не нуждаются в модификации.** С другой стороны, вы можете не использовать их вовсе или модифицировать произвольным образом.

Как
часто бывает в реальной жизни разработчиков, обе эти библиотеки даны
без документации, но с самодокументируемым кодом: интерфейсы
представленных классов легко читаются по заголовкам публичных методов.

О классах дополнительно известно следующее:

- Вершины и рёбра графа нумеруются автоинкрементно беззнаковыми целыми числами, хранящимися в типах `VertexId` и `EdgeId`: вершины нумеруются от нуля до количества вершин минус один в
  соответствии с пользовательской логикой. Номер очередного ребра выдаётся методом `AddEdge`; он равен нулю для первого вызова метода и при каждом следующем вызове увеличивается на единицу.
- Память, нужная для хранения графа, линейна относительно суммы количеств вершин и рёбер.
- Конструктор и деструктор графа имеют линейную сложность, а остальные методы константны или амортизированно константны.
- Маршрутизатор — класс `Router` — требует квадратичного относительно количества вершин объёма памяти, не считая памяти, требуемой для хранения кэша маршрутов.
- Конструктор маршрутизатора имеет сложность O(V3+E), где V — количество вершин графа, E — количество рёбер.

  O(V3+E)O(V^3 + E)

  VV

  EE

- Маршрутизатор не работает с графами, имеющими рёбра отрицательного веса.
- Построение маршрута на готовом маршрутизаторе линейно относительно **количества рёбер в маршруте**. Таким образом, основная нагрузка построения оптимальных путей ложится на конструктор.

Нужно:

- самостоятельно разобраться с понятиями графов и путями в них;
- придумать, как по транспортному справочнику построить граф, в котором путь
  наименьшего веса соответствует оптимальному маршруту на автобусах. Но
  сначала надо определиться, что в этом графе будет вершинами, а что —
  рёбрами;
- написать код построения графа и описания маршрута по пути, полученному от маршрутизатора.

Вершины
графа в той или иной степени должны соответствовать остановкам.
Поразмышляйте над двумя вариантами, каждый из которых может привести к
правильному решению:

- Если ограничиться одной вершиной на остановку, нужно заложить время ожидания автобуса в веса ребёр, соединяющие остановки.
- Можно использовать две вершины для каждой из остановок. Первая соответствует
  состоянию «начал ждать автобус на остановке S», вторая — «садится в
  автобус на остановке S». Время ожидания автобуса учитывается благодаря
  ребру веса `bus_wait_time` из первой вершины во вторую.

Подумайте, как именно должны быть организованы рёбра графа в каждом из случаев.

### Ограничения

- Количество запросов на создание автобусов и остановок — не более 100.
- Количество запросов к уже созданной базе — не более 2000, из них запросов на отрисовку — не более двух.
- Не более 100 остановок в маршруте.
- Ограничение по времени — 3 секунды на тест, по памяти — 512 МБ.

*В
реальном маршрутизаторе допустимое время на предварительную подготовку
данных вполне могло бы занимать минуты, что позволило бы увеличить
ограничение на количество объектов.*

### Пример 1

[Иллюстрации к примеру](https://code.s3.yandex.net/C%20%20/e1_map.pdf)

- Пример 2

[Иллюстрации к примеру](https://code.s3.yandex.net/C%20%20/e2_map.pdf)

- Пример 3

[Иллюстрации к примеру](https://code.s3.yandex.net/C%20%20/e3_map.pdf)


- Пример 4

[Ввод](https://code.s3.yandex.net/C%20%20/e4_input.json)

[Вывод](https://code.s3.yandex.net/C%20%20/e4_output.json)

В файле ниже - примеры случайно сгенерированных входных файлов и корректных выводов для них.

[s12_final_opentest.zip](https://code.s3.yandex.net/CPP/s12_final_opentest.zip)

### Как будет тестироваться ваш код

Программа будет получать на `stdin` валидные JSON-документы с описанием маршрутов и запросов к справочнику. Выводимые в `stdout`
данные будут сравниваться с ожидаемым значением. Вывод вещественных
чисел должен осуществляться стандартно — без модификаторов вывода.

Маршруты, выводимые в ответ на запрос `Route`,
должны быть оптимальны по времени. Если маршрутов с минимальным
возможным суммарным временем несколько, допускается вывести любой из
них: тестирующая система проверяет лишь совпадение времени маршрута с
оптимальным и корректность самого маршрута.

### Доп

1. Решение
   с двумя вершинами на каждую остановку удобно тем, что можно построить
   граф, нарисовав по ребру на каждый возможный элемент итогового маршрута:
   `Wait` — это ребро от **начала ожидания** на некоторой остановке до **окончания ожидания** на ней же, `Bus` — это ребро от **окончания ожидания** на некоторой остановке автобусного маршрута до **начала ожидания** на другой остановке этого же маршрута.
2. В
   решении с одной вершиной на каждую остановку мы не можем позволить себе
   проводить рёбра лишь между соседними остановками, потому что в каждое
   из рёбер
   будет заложено время ожидания автобуса. Таким образом, рёбра должны
   проводиться для каждой пары остановок на каждом маршруте, аналогично
   решению с двумя вершинами на каждую остановку.
3. В решении с одной вершиной на остановку каждому ребру построенного маршрута соответствуют два элемента: `Wait` — ожидание нужного автобуса, `Bus` — проезд на этом автобусе между соответствующими остановками.
4. В
   любом варианте решения нужно проектировать структуры данных с учётом
   того, что автобус может проходить через одну остановку несколько раз:
   см. пример 3.