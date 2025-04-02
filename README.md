# Youbot controller

Простое многопоточное приложение для управления роботом youbot. Создавалась в учебных целях, поддерживает следующие функции:

* Многопоточное выполнение с помощью менеджера потоков
* Логирование
* Настройка через файлы конфигурации
* Управление через qt приложение по TCP
* Трансляция изображения с камеры по UDP
* Разметка изображения с помощью YOLO10 с использованием [YOLOs-CPP](https://github.com/Geekgineer/YOLOs-CPP/tree/main)
* Возможность получения данных с лидара

## Запуск

1. Клонируйте репозиторий

    ``` bash
    git clone https://github.com/KirillHit/youbot_controller.git --recurse-submodules
    cd youbot_controller
    ```

2. Запустите скрипт для установки onnxruntime

    ``` bash
    ./onnx_downlad.sh
    ```

3. Собирите приложение с помощью cmake