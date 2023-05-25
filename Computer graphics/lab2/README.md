1.Определить куб в качестве модели объекта сцены.
2.Определить преобразования, позволяющие получить заданный вид проекции (гоизонтальная диметрия). Для демонстрации проекции добавить в сцену куб (в стандартной
ориентации, не изменяемой при модельно-видовых преобразованиях основного объекта).
3.Реализовать изменение ориентации и размеров объекта (навигацию камеры) с помощью
модельно-видовых преобразований (без gluLookAt). Управление производится интерактивно с
помощью клавиатуры и/или мыши.
4.Предусмотреть возможность переключения между каркасным и твердотельным
отображением модели (glFrontFace/ glPolygonMode).

Определены два куба, первый построен в горизонтальной косоугольной изомерии при помощи матрицы преобразования. Реализована возможность изменения длины спроецированного единичного вектора на ось Z и изменение угла горизонтального наклона (кнопками вверх, вниз, вправо, влево), возможность переключения между каркасным и твердотельным отображением (кнопка F1)