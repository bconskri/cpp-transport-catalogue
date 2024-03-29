# **Итоговый проект: двухстадийность в транспортном справочнике**

## Задание 1

В этом и последующих заданиях нужно разделить программу, которая отвечает за наполнение транспортного справочника и обработку запросов к нему, на две:

1. Программа make_base: создание базы транспортного справочника по запросам `base_requests` и её сериализация в файл.
2. Программа process_requests: десериализация базы из файла и использование её для ответов на запросы `stat_requests`.

Полная функциональность справочника будет поддержана поэтапно. В этой задаче
требуется сериализация лишь той части базы, которая нужна для ответа на
запросы Bus и Stop. Сама функциональность должна соответствовать той,
что была в девятом спринте. Рисовать карту и строить кратчайшие маршруты
здесь не требуется.

### Программа make_base

На вход программе make_base через стандартный поток ввода подаётся JSON со следующими ключами:

1. `base_requests`: запросы Bus и Stop на создание базы. Формат этих данных остаётся прежним.
2. `routing_settings`: настройки маршрутизации. Не используются в этой задаче, так как нет
   запросов на построение маршрута, но будут использоваться в следующих.
   Формат — прежний.
3. `render_settings`: настройки отрисовки. Не используются в этой задаче, так как нет
   запросов на построение маршрута и рисование карты, но будут
   использоваться в следующих. Формат — прежний.
4. `serialization_settings`: настройки сериализации. На текущий момент это словарь с единственным ключом `file`, которому соответствует строка — название файла. Именно в этот файл нужно сохранить сериализованную базу.

Задача
программы make_base — построить базу и сериализовать её в файл с
указанным именем. Выводить что-либо в стандартный поток вывода не
требуется.

### Программа process_requests

На
вход программе process_requests подаётся файл с сериализованной базой
(результат работы make_base), а также — через стандартный поток ввода —
JSON со следующими ключами:

1. `stat_requests`: запросы Bus и Stop к готовой базе. Формат этих данных аналогичен предыдущим частям.
2. `serialization_settings`: настройки сериализации в формате, аналогичном этой же секции на входе make_base. А именно, в ключе `file` указывается название файла, из которого нужно считать сериализованную базу.

Программа process_requests должна вывести JSON с ответами на запросы — в том же формате, что и в предыдущих частях.

В файле ниже - примеры случайно сгенерированных входных файлов и корректных выводов для них.

[s14_part1_opentest.zip](https://code.s3.yandex.net/CPP/s14_part1_opentest.zip)

### Что отправлять на проверку

- main.cpp: единственный файл с функцией main. В зависимости от переданного при
  запуске режима работает либо как make_base, либо
  как process_requests (см. заготовку кода).
- Файл transport_catalogue.proto содержит message `TransportCatalogue`. Этот message может находиться в любом package на ваше усмотрение. `TransportCatalogue` описывает proto-схему для сериализуемой базы. transport_catalogue.proto может содержать и другие proto-сообщения, а также импортировать их из
  других proto-файлов.
- CMakeLIsts.txt должен собирать бинарный файл transport_catalogue.
- Прочие файлы .h и .cpp.

### Как будет тестироваться ваш код

Схема проверки предыдущих версий транспортного справочника была следующей:

1. Запускается единственная программа, на вход подаётся большой JSON.
2. Проверяется, что вывод этой программы соответствует ожидаемому.

Теперь же происходит взаимодействие двух программ, что приводит к следующей схеме:

1. Запускается программа make_base (основной исполняемый файл с параметром `make_base`), на вход подаётся JSON с `base_requests` и общими настройками.
2. Проверяется полученный файл с сериализованной базой. А именно, проверяется
   возможность его десериализации с использованием proto-сообщения `TransportCatalogue` из transport_catalogue.proto, а также проверяется его размер: он не
   может превышать размер авторской версии сериализованной базы той же
   структуры более чем на 50 %. При превышении размера сериализованного
   файла вы получите соответствующий фидбэк.
3. Запускается программа process_requests (основной исполняемый файл с параметром `process_requests`), на вход подаётся JSON с `stat_requests` и `serialization_settings`. В указанном файле находится сериализованная база, полученная в п. 1 и проверенная в п. 2.
4. Проверяется, что вывод программы process_requests из п. 3 соответствует ожидаемому.

### Ограничения

- Не более 2000 запросов к make_base.
- Не более 2000 запросов к process_requests.
- Не более 100 остановок в маршруте.
- 20 секунд на работу make_base и 1 секунда на работу process_requests.

+

Разработайте proto-схему базы так, чтобы она исчерпывающе описывала автобусы и остановки, но при этом не хранила строковые названия дважды.