# SimpleSwitcher
![GitHub release (latest by date)](https://img.shields.io/github/v/release/alexzh2/SimpleSwitcher?style=plastic)
![GitHub all releases](https://img.shields.io/github/downloads/alexzh2/SimpleSwitcher/total?style=plastic)
![GitHub last commit](https://img.shields.io/github/last-commit/alexzh2/SimpleSwitcher?style=plastic)

SimpleSwitcher - программа для быстрого исправления раскладки набранного текста в Windows.

## Features

- Умеет работать в программах, запущенных от администратора. 
- Удерживая клавишу Shift, можно исправлять несколько слов подряд.
- Есть возможность использовать Capslock для любой функции программы.
- Не хранит набираемый текст в открытом виде (используется шифрование Windows)
- Запрещает другим программам перехватывать клавиатуру
- Позволяет произвольно переназначать любые клавиши (scancode remap)
- Показывает флаги стран в трее.

## Download & Install

Программа поставляется в portable виде (не требует установки).
Можно скачать последний опубликованный <a href="https://github.com/Aegel5/SimpleSwitcher/releases">релиз</a>

Либо собрать из кода самостоятельно.

### Инструкция по сборке:
- Установить требуемые программы: 
  - `Git for Windows`. Все настройки - по-умолчанию.
  - `Visual Studio 2022`. Необходим компонент C++. 
  - `CMake`. Нужно выбрать опцию "добавить в PATH".
  - `Python`. Можно выбрать опцию "связать py файлы с питоном" для удобного запуска скриптов из командной строки.
- Скачать исходники.
  - Открыть командную строку (cmd.exe) и перейти в каталог, где будет храниться код программы.
  - Выполнить команду `git clone --recurse-submodules https://github.com/Aegel5/SimpleSwitcher.git`
- В созданной папке запустить скрипт `build.py`
- Собранная версия будет в папке `package_build/OUT`

## License

<a href="LICENSE">GPL-3.0 License</a>  
Copyright © 2022 Aegel5

## Donate

Если вам нравится программа, вы можете поддержать автора.  
<a href="https://yoomoney.ru/quickpay/shop-widget?writer=buyer&targets=%D0%9D%D0%B0%20%D0%BF%D0%B5%D1%87%D0%B5%D0%BD%D1%8C%D0%BA%D0%B8&targets-hint=%D0%9D%D0%B0%20%D0%BF%D0%B5%D1%87%D0%B5%D0%BD%D1%8C%D0%BA%D0%B8&default-sum=100&button-text=12&hint=&successURL=&quickpay=shop&account=410013286626983&" target="_blank"><img src="https://www.buymeacoffee.com/assets/img/custom_images/yellow_img.png" alt="Buy Me A Coffee" style="height: auto !important;width: auto !important;" ></a>  
Или же сделать перевод на карту Сбербанка: 4276 1609 6336 6548
