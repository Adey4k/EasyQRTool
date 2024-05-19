# EasyQRTool
Курсова робота "Створення програми для генерації та роботи з QR-кодами на мові C" Ладікова Максима (25 гр)





Додавання бібліотек
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/d8288919-e1c3-4524-89af-c05fd3777c95)
#include <iostream> - базова бібліотека для написання C++ коду, забезпечує ввід/вивід інформації;
#include <string> - Для роботи з елементами типу string;
#include <bitset> - Для роботи з бітовими операціями;
#include <vector> - Для роботи з векторами (динамічними масивами даних);
using namespace std; - для спрощення запису стандартних елементів.



Функція fullfill
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/1bfcc2d7-5459-4a17-833f-096462f9cecf)
Ця функція заповнює строку Binary(Бінарну послідовність) до розміру maxsize(залежить від версії QR-коду), додаючи строки "11101100" та "00010001" по черзі.


Фукнція gf_indexOf
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/713062c9-152b-4199-a2a0-3caee99f19c0)
Ця функція шукає індекс елемента value у векторі gf, перебираючи елементи динамічного масиву, і у випадку співпадіння з value повертає індекс
const std::vector<int>& gf - константне посилання на вектор цілих чисел. Використання константного посилання дозволяє уникнути копіювання вектора і захищає його від змін всередині функції.

Функція для побітового складення за модулем 255 у полі Галуа
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/697882bf-c50b-40b7-8685-15046a4e6463)
Результат побітового складення за модулем 255 - залишок від ділення суми доданків на 255

Функція addCorrectionBytes
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/6191475e-bb04-4acc-a240-72983f226857)
Ця функція реалізовує алгоритм Рида Соломона(створює байти корекції), нижче наведена більш детальна інформація

![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/69b9932f-94a7-4d86-a0ad-7250bbc5596a)
Ця частина зберігає значення першого елементу в масиві, і додає нуль в кінці

Перевірка, чи не є збереженний байт нулем(нуль необіхдно пропустити)
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/febba9ee-048b-4826-b674-49c8946a3cf2)

Побітове складення за модулем 2
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/fe1b8969-4c95-439a-8bf5-901324cd498d)

Повернення вектора з байтами корекції
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/23a4e5ba-ab56-40e2-a297-77cf07b946b0)

-----------------------------------------------------------------------------------------

Основна частина.
Блок 1 - Кодування інформації

Отримання інформації від користувача
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/7ca10cd5-75c2-41b9-8da0-bafb19c7c791)

Перевірка на коректність вводу і перетворення інформації на бінарну послідовність, збереження її у нову змінну
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/77316c71-b0cd-45e1-89f2-1ba0d6df5826)

Блок 2 - Додавання службової інформації і заповнення
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/343e8992-6ead-4f56-a1ac-48fa00ff995d)

![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/2c254c86-d211-4f85-b5d2-980205cc85a2)
Моя програма підтримує з першої по шосту версії QR-кода, цей код виведе помилку якщо користувач спробує закодувати забагато даних

Блок 3 - Розподіл інформації на блоки
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/3fd0dcaf-459f-4b12-810c-4427cbf324ba)

Блок 4 - Створення байтів корекції
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/0eeec021-de31-45ba-bf21-35dc2a53cf88)

Створення Поля Галуа
![image](https://github.com/Adey4k/EasyQRTool/assets/150963491/3b51f5fa-60bf-4a9f-b7dd-22564a9fbc9e)



![Untitled Diagram drawio](https://github.com/Adey4k/EasyQRTool/assets/150963491/a3909c3f-a90e-4948-ac38-17cbc59f24cf)

