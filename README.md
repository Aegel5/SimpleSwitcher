# ![изображение](https://github.com/user-attachments/assets/a36dfe16-5318-4463-b4c0-c76029dfe742) SimpleSwitcher 
![GitHub release (latest by date)](https://img.shields.io/github/v/release/alexzh2/SimpleSwitcher?style=plastic)
![GitHub all releases](https://img.shields.io/github/downloads/alexzh2/SimpleSwitcher/total?style=plastic)
![GitHub last commit](https://img.shields.io/github/last-commit/alexzh2/SimpleSwitcher?style=plastic)





Simple Switcher - переключатель клавиатуры ⌨️ для быстрого исправления языка набранного текста в Windows. Полностью открыт и бесплатен, написан на современном `С++ 23`, турбо-реактивен 🏎️ и никак не сказывается на работе ОС. Список фич небольшой, но он постоянно пополняется.

## Основные возможности

- Умеет работать в программах, запущенных от администратора. 
- Удерживая клавишу Shift, можно исправлять несколько слов подряд.
- Возможность использовать практически любые клавиши и комбинации клавиш, например, Capslock, Scrolllock, RAlt, Ctrl #up, LWin + ... для любой функции программы.
- Не хранит набираемый текст в открытом виде в памяти (используется шифрование Windows).
- Позволяет произвольно переназначать любые клавиши (scancode remap)
- Показывает флаги стран в трее.
- Отключение accessibility функциональности Windows (например, отключения залипания после нажатия 5-ти Shift подряд).
- Смена регистра выделенного текста.

## Download & Install

Simple Switcher поставляется в portable виде (не требует установки).
Можно скачать последний опубликованный <a href="https://github.com/Aegel5/SimpleSwitcher/releases">релиз</a>, либо собрать из кода самостоятельно.

## Сборка из исходного кода:
- Установить требуемые программы: 
  - `Git for Windows`. Все настройки - по-умолчанию.
  - `Visual Studio 2022`. Необходим компонент C++. 
  - `CMake`. Нужно выбрать опцию "добавить в PATH".
  - `Python`. Можно выбрать опцию "добавить в PATH" для удобного запуска скриптов из командной строки.
- Скачать исходники.
  - Открыть командную строку (cmd.exe) и перейти в каталог, где будет храниться код программы.
  - Выполнить команду `git clone --recurse-submodules https://github.com/Aegel5/SimpleSwitcher.git`
- Внести патчи в библиотеку wxWidget
  - src\msw\taskbar.cpp: `m_iconAdded ? Operation_Modify : Operation_Add` заменить на `Operation_TryBoth`
- В созданной папке запустить скрипт `build.py`
- Собранная версия будет в папке `package_build/OUT`

## Известные ограничения
- При одновременном назначении (`hotkey` | `hotkey #up`) и `hotkey #double` будет применена задержка 280 ms (параметр конфига `quick_press_ms`)
- `LAlt + ...` - может работать неправильно.
- `Win + CapsLock` - хот-кей работает некорректно.
- `Ctrl + Break` = `Cancel` - это особенность Windows

## Использование CapsLock
_Рекомендуется:_ 
- Использовать клавишу CapsLock для исправления раскладки.
- Для активации режима CapsLock использовать Win + CapsLock или LAlt + CapsLock или назначить комбинацию в программе.

_Также возможно:_
- Сначала нужно переназначить CapsLock на клавишу F24 (вкладка Key Remap)
- Перезагрузить Windows
- Назначить клавишу F24 на требуемую функцию
- [Опционально] Назначить клавишу на срабатывание настоящего CapsLock (по умолчанию Ctrl + F24).
- Перед удалением программы, нужно снять галочку Remap CapsLock, чтобы вернуть исходное состояние.

## Альтернативный режим переключения (эмуляция Alt+Shift)
Если основной режим работает некорректно, то можно использовать альтернативный режим.
Если в системе всего 2 или 3 раскладки, то можно просто указать хот-кей циклической смены языка в ОС (по-умолчанию Alt+Shift). Для 3-х и более раскладок, рекомендуется настроить хот-кей на каждый язык, как в ОС, так и в SS  (выделены голубым цветом в GUI). Alt+Shift+1 - включить первый язык, Alt+Shift+2 - второй язык и т.д.

## Разделение текста на слова
При нажатии клавиши, которая продуцирует разные символы на разных раскладках разделение проходит по следующим правилам:
- Если все символы это буквы или цифры - разделение не происходит.
- Если все символы это НЕ буквы или цифры - разделение происходит.
- В остальных случаях (например, клавиша эЭ'") разделение происходит в зависимости от настроек (по умолчанию не разделяется).

Если разделение слов вообще не требуется, можно использовать функцию "Исправить весь недавно введенный текст".

## Пользовательские флаги
Для добавления пользовательских флагов необходимо создать папку `flags` рядом с `SimpleSwitcher.exe`. Пример.
```text
flags
  Name_of_Set1
    ru16.png
    ru32.png
    us16.png
  Set2
    ru.ico
    us_some_name.ico
```
- Поддерживаемые форматы: PNG (рекомендуется), ICO, JPG, BMP, SVG
- Файл должен начинаться с двух-буквенного идентификатора страны. На одну страну может быть несколько файлов разных размеров.
- Размер - любой, но рекомендуется добавлять 16x16 и 32x32 в случае high dpi.

## Отключение Ctrl + LAlt как RAlt на расширенных раскладках
Подробнее: https://github.com/Aegel5/SimpleSwitcher/issues/54

## Работа при удаленном подключении
- RDP. При использовании rdp для подключения к удаленной ОС, все должно работать "из коробки". Можно установить SS как на локальный, так и на удаленный компьютер - будет отрабатывать только тот SS, в системе которого вы сейчас находитесь.
- Другие клиенты. В случае других клиентов может быть проблема "double switch", когда срабатывают оба SS одновременно (локальный и удаленный).
Для исправления этой ситуации, в локальном SS добавить клиент программы подключения в исключения.

## Исключения для программ
Настройка пока только через `SimpleSwitcher.json`. Пример
```json
"disableInPrograms": [
    "game1.exe",
    "c:/game2.exe"
],
```

## Запуск произволных процессов по хот-кею
Настройка пока только через `SimpleSwitcher.json`. Пример
```json
"run_programs": [
    {
        "args": "",
        "elevated": false,
        "hotkey": [
            "LWin + C"
        ],
        "path": "calc.exe"
    }
]
```

## 📟 Принять участие в разработке
Если есть минимальные знания C++, то можно исправить баг / реализовать фичу или же просто немного улучшить код программы.
Также всегда приветствуются пулл-реквесты с исправлением/переводом интерфейса, обновления докуменатации и т.д. Можно исправить/улучшить иконку или полностью нарисовать новую, если есть идея.

## Telegram
https://t.me/simple_switcher

Любые вопросы и предложения можно писать туда.

## License
<a href="LICENSE">GPL-3.0 License</a>  

## Donate
Если вам нравится программа, и вы хотите поддержать ее развитие:
- 2202 2050 0289 5834, CберБанк, Александр Ж. (Рекомендуется)
- [https://yoomoney.ru/bill/pay/19AFP3RQA50.250330](https://yoomoney.ru/to/410013286626983)
- https://boosty.to/alekseshka/donate
- https://www.donationalerts.com/r/alekseshka

## 🌟 Особая благодарность! (не полный список)
- Петр
- nnmdd 
- Сергій Куліков
- Иван
