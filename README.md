# Spreadsheet
Электронная таблица. Реализует упрощённый аналог листа таблицы Microsoft Excel или Google Sheets. Ячейки таблицы могут содержать текст или формулы. Формулы могут содержать индексы ячеек. Формулы поддерживают только основные арифметические операции. Лексический и синтаксический анализатор реализован с применением генератора ANTLR.

Проект имеет большой потенциал для расширения. В первую очередь это реализация поддержки логических операций и математических функций для вычисления формул в ячейках. В дальнейшем возможна реализация одновременной работы сразу с несколькими таблицами. Также есть планы на включение в проект графической библиотеки, позволяющий строить графики.

Для сборки проекта необходим CMake, компилятор С++, поддерживающий 17 стандарт языка, или более поздние версии.
