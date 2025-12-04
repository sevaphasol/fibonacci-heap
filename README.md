# Fibonacci Heap

## Описание

Фибоначчиева куча — структура данных для приоритетных операций с асимптотикой:

| Операция    | Сложность |
| ----------- | --------- |
| Insert      | O(1)      |
| GetMin      | O(1)      |
| ExtractMin  | O*(log N) |
| DecreaseKey | O*(1)     |
| Merge       | O(1)      |

---

## API

```cpp
namespace fhp {

class FHeap {
public:
    class Handle {
    public:
        Handle();
    };

    FHeap();
    Handle Insert(int key);
    int GetMin() const;
    int ExtractMin();
    void DecreaseKey(const Handle& h, int new_key);
    void Erase(const Handle& h);
    void Merge(FHeap& other);
    size_t GetSize() const;
};

}  // namespace fhp
```

---

## Зависимости

GoogleTest

---

## Сборка и запуск

Сборка
```
./scripts/build.sh
```

Запуск юнит тестов
```
./scripts/test.sh
```
