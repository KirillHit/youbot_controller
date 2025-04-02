# Youbot controller

Простое многопоточное приложение для управления роботом youbot. Создавалось в учебных целях, поддерживает следующие функции:

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
    ./onnx_download.sh.sh
    ```

3. Соберите приложение с помощью cmake

4. Приложение на qt можно находится в ветке app
